/*
 * This file is part of MIDI2LR. Copyright (C) 2015 by Rory Jaffe.
 *
 * MIDI2LR is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with MIDI2LR.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include "LR_IPC_Out.h"

#include <algorithm>
#include <chrono>
#include <exception>
#include <string>
#include <unordered_map>
#include <utility>

#include <fmt/format.h>

#include "ControlsModel.h"
#include "MIDIReceiver.h"
#include "MIDISender.h"
#include "MidiUtilities.h"
#include "Misc.h"
#include "Profile.h"

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;
using namespace std::literals::chrono_literals;

namespace {
   /* in between recurrent actions */
   constexpr auto kDelay {8ms};
   constexpr auto kLrOutPort {58763};
   /* give controller a minimum refractory period before resetting */
   constexpr auto kMinRecenterTime {250ms};
   constexpr auto kRecenterTimer {std::max(kMinRecenterTime, kDelay + kDelay / 2)};
   constexpr auto kTerminate {"!!!@#$%^"};
} // namespace

LrIpcOut::LrIpcOut(ControlsModel& c_model, const Profile& profile, const MidiSender& midi_sender,
    MidiReceiver& midi_receiver)
    : midi_sender_ {midi_sender}, profile_ {profile}, controls_model_ {c_model}
{
   midi_receiver.AddCallback(this, &LrIpcOut::MidiCmdCallback);
}

void LrIpcOut::SendCommand(std::string&& command)
{
   try {
      if (sending_stopped_)
         return;
      command_.push(std::move(command));
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void LrIpcOut::SendCommand(const std::string& command)
{
   try {
      if (sending_stopped_)
         return;
      command_.push(command);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void LrIpcOut::SendingRestart()
{
   try {
      sending_stopped_ = false;
      const auto con {connected_.load(std::memory_order_acquire)};
      for (const auto& cb : callbacks_)
         cb(con, false);
      /* resync controls */
      SendCommand("FullRefresh 1\n");
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void LrIpcOut::SendingStop()
{
   try {
      sending_stopped_ = true;
      const auto con {connected_.load(std::memory_order_acquire)};
      for (const auto& cb : callbacks_)
         cb(con, true);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void LrIpcOut::Start()
{
   try {
      Connect();
      io_thread0_ = std::async(std::launch::async, [this] {
         rsj::LabelThread(L"LrIpcOut io_thread0_");
         _mm_setcsr(_mm_getcsr() | 0x8040);
         io_context_.run();
      });
      io_thread1_ = std::async(std::launch::async, [this] {
         rsj::LabelThread(L"LrIpcOut io_thread1_");
         _mm_setcsr(_mm_getcsr() | 0x8040);
         io_context_.run();
      });
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void LrIpcOut::Stop()
{
   thread_should_exit_.store(true, std::memory_order_release);
   /* pump output queue before port closed */
   if (const auto m {command_.clear_count_emplace(kTerminate)})
      rsj::Log(fmt::format("{} left in queue in LrIpcOut destructor.", m));
   /* no more connect/disconnect notifications */
   callbacks_.clear();
   asio::post([this] {
      if (socket_.is_open()) {
         asio::error_code ec;
         /* For portable behaviour with respect to graceful closure of a connected socket, call
          * shutdown() before closing the socket. */
         socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
         if (ec) {
            rsj::Log(fmt::format("LR_IPC_Out socket shutdown error {}.", ec.message()));
            ec.clear();
         }
         socket_.close(ec);
         if (ec)
            rsj::Log(fmt::format("LR_IPC_Out socket close error {}.", ec.message()));
      }
      recenter_timer_.cancel();
   });
}

void LrIpcOut::Connect()
{
   try {
      socket_.async_connect(asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), kLrOutPort),
          [this](const asio::error_code& error) {
             if (!error) {
                ConnectionMade();
                SendOut();
             }
             else {
                rsj::Log(fmt::format("LR_IPC_Out did not connect. {}.", error.message()));
                asio::error_code ec2;
                socket_.close(ec2);
                if (ec2)
                   rsj::Log(fmt::format("LR_IPC_Out socket close error {}.", ec2.message()));
             }
          });
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void LrIpcOut::ConnectionMade()
{
   connected_.store(true, std::memory_order_release);
   try {
      rsj::Log("Socket connected in LR_IPC_Out.");
      for (const auto& cb : callbacks_)
         cb(true, sending_stopped_);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void LrIpcOut::MidiCmdCallback(const rsj::MidiMessage& mm)
{
   struct RepeatMessage {
      std::string cw;
      std::string ccw;
   };
   try {
      const rsj::MidiMessageId message {mm};
      static const std::unordered_map<std::string, RepeatMessage> kCmdUpDown {
          {"ChangeBrushSize", {"BrushSizeLarger 1\n", "BrushSizeSmaller 1\n"}},
          {"ChangeCurrentSlider", {"SliderIncrease 1\n", "SliderDecrease 1\n"}},
          {"ChangeFeatherSize", {"BrushFeatherLarger 1\n", "BrushFeatherSmaller 1\n"}},
          {"ChangeLastDevelopParameter",
              {"IncrementLastDevelopParameter 1\n", "DecrementLastDevelopParameter 1\n"}},
          {"IncreaseDecreaseRating", {"IncreaseRating 1\n", "DecreaseRating 1\n"}},
          {"Key2Key1", {"Key2 1\n", "Key1 1\n"}},
          {"Key4Key3", {"Key4 1\n", "Key3 1\n"}},
          {"Key6Key5", {"Key6 1\n", "Key5 1\n"}},
          {"Key8Key7", {"Key8 1\n", "Key7 1\n"}},
          {"Key10Key9", {"Key10 1\n", "Key9 1\n"}},
          {"Key12Key11", {"Key12 1\n", "Key11 1\n"}},
          {"Key14Key13", {"Key14 1\n", "Key13 1\n"}},
          {"Key16Key15", {"Key16 1\n", "Key15 1\n"}},
          {"Key18Key17", {"Key18 1\n", "Key17 1\n"}},
          {"Key20Key19", {"Key20 1\n", "Key19 1\n"}},
          {"Key22Key21", {"Key22 1\n", "Key21 1\n"}},
          {"Key24Key23", {"Key24 1\n", "Key23 1\n"}},
          {"Key26Key25", {"Key26 1\n", "Key25 1\n"}},
          {"Key28Key27", {"Key28 1\n", "Key27 1\n"}},
          {"Key30Key29", {"Key30 1\n", "Key29 1\n"}},
          {"Key32Key31", {"Key32 1\n", "Key31 1\n"}},
          {"Key34Key33", {"Key34 1\n", "Key33 1\n"}},
          {"Key36Key35", {"Key36 1\n", "Key35 1\n"}},
          {"Key38Key37", {"Key38 1\n", "Key37 1\n"}},
          {"Key40Key39", {"Key40 1\n", "Key39 1\n"}},
          {"NextPrev", {"Next 1\n", "Prev 1\n"}},
          {"RedoUndo", {"Redo 1\n", "Undo 1\n"}},
          {"SelectRightLeft", {"Select1Right 1\n", "Select1Left 1\n"}},
          {"ZoomInOut", {"ZoomInSmallStep 1\n", "ZoomOutSmallStep 1\n"}},
          {"ZoomOutIn", {"ZoomOutSmallStep 1\n", "ZoomInSmallStep 1\n"}},
      };
      if (!profile_.MessageExistsInMap(message))
         return;
      const auto command_to_send {profile_.GetCommandForMessage(message)};
      if (command_to_send == "PrevPro" || command_to_send == "NextPro"
          || command_to_send == CommandSet::kUnassigned)
         /* handled by ProfileManager or unassigned */
         [[unlikely]] return;
      /* if it is a repeated command, change command_to_send appropriately */
      if (const auto a {kCmdUpDown.find(command_to_send)}; a != kCmdUpDown.end())
         [[unlikely]]
         {
            static TimePoint next_response {};
            if (const auto now {Clock::now()}; next_response < now) {
               next_response = now + kDelay;
               if (mm.message_type_byte == rsj::MessageType::Pw
                   || (mm.message_type_byte == rsj::MessageType::Cc
                       && controls_model_.GetCcMethod(message) == rsj::CCmethod::kAbsolute))
                  SetRecenter(message);
               const auto change {controls_model_.MeasureChange(mm)};
               if (change == 0)
                  /* don't send any signal */
                  return;
               if (change > 0)
                  /* turned clockwise */
                  SendCommand(a->second.cw);
               else
                  /* turned counterclockwise */
                  SendCommand(a->second.ccw);
            }
         }
      else {
         /* not repeated command */
         const auto computed_value {controls_model_.ControllerToPlugin(mm)};
         SendCommand(command_to_send + ' ' + std::to_string(computed_value) + '\n');
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void LrIpcOut::SendOut()
{
   try {
      auto command_copy {std::make_shared<std::string>(command_.pop())};
      if (*command_copy == kTerminate)
         [[unlikely]] return;
      /* always connected when running SendOut, no need to check flag */
      if (command_copy->back() != '\n')
         /* should be terminated with \n */
         [[unlikely]]* command_copy += '\n';
      asio::async_write(socket_, asio::buffer(*command_copy),
          [this, command_copy](const asio::error_code& error, std::size_t) {
             if (!error)
                [[likely]] SendOut();
             else {
                rsj::Log(fmt::format("LR_IPC_Out Write: {}.", error.message()));
             }
          });
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void LrIpcOut::SetRecenter(rsj::MidiMessageId mm)
{
   /* by capturing mm by copy, don't have to worry about later calls changing it--those will just
    * cancel and reschedule new one */
   try {
      asio::dispatch([this] { recenter_timer_.expires_after(kRecenterTimer); });
      recenter_timer_.async_wait([this, mm](const asio::error_code& error) {
         if (!error && !thread_should_exit_.load(std::memory_order_acquire))
            midi_sender_.Send(mm, controls_model_.SetToCenter(mm));
      });
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}