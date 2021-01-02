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
#include <utility>

#include <fmt/format.h>

#include "CommandSet.h"
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
   constexpr auto kDelay {8ms}; /* in between recurrent actions */
   constexpr auto kLrOutPort {58763};
   constexpr auto kMinRecenterTime {250ms}; /* minimum period before recentering */
   constexpr auto kRecenterTimer {std::max(kMinRecenterTime, kDelay + kDelay / 2)};
   constexpr auto kTerminate {"!!!@#$%^"};
} // namespace

LrIpcOut::LrIpcOut(const CommandSet& command_set, ControlsModel& c_model, const Profile& profile,
    const MidiSender& midi_sender, MidiReceiver& midi_receiver)
    : midi_sender_ {midi_sender}, profile_ {profile}, repeat_cmd_ {command_set.GetRepeats()},
      wrap_ {command_set.GetWraps()}, controls_model_ {c_model}
{
   midi_receiver.AddCallback(this, &LrIpcOut::MidiCmdCallback);
}

void LrIpcOut::SendingRestart()
{
   try {
      sending_stopped_ = false;
      const auto con {connected_.load(std::memory_order_acquire)};
      for (const auto& cb : callbacks_) cb(con, false);
      SendCommand("FullRefresh 1\n"); /* synchronize controls */
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
      for (const auto& cb : callbacks_) cb(con, true);
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
         MIDI2LR_FAST_FLOATS;
         if constexpr (kNdebug)
            io_context_.run();
         else
            rsj::Log(
                fmt::format(FMT_STRING("LrIpcOut thread0 ran {} handlers."), io_context_.run()));
      });
      io_thread1_ = std::async(std::launch::async, [this] {
         rsj::LabelThread(L"LrIpcOut io_thread1_");
         MIDI2LR_FAST_FLOATS;
         if constexpr (kNdebug)
            io_context_.run();
         else
            rsj::Log(
                fmt::format(FMT_STRING("LrIpcOut thread1 ran {} handlers."), io_context_.run()));
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
   /* clear output queue before port closed */
   if (const auto m {command_.clear_count_emplace(kTerminate)})
      rsj::Log(fmt::format(FMT_STRING("{} left in queue in LrIpcOut destructor."), m));
   callbacks_.clear(); /* no more connect/disconnect notifications */
   asio::post([this] {
      if (socket_.is_open()) {
         asio::error_code ec;
         /* For portable behaviour with respect to graceful closure of a connected socket, call
          * shutdown() before closing the socket. */
         socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
         if (ec) {
            rsj::Log(fmt::format(FMT_STRING("LR_IPC_Out socket shutdown error {}."), ec.message()));
            ec.clear();
         }
         socket_.close(ec);
         if (ec)
            rsj::Log(fmt::format(FMT_STRING("LR_IPC_Out socket close error {}."), ec.message()));
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
                rsj::Log(
                    fmt::format(FMT_STRING("LR_IPC_Out did not connect. {}."), error.message()));
                asio::error_code ec2;
                socket_.close(ec2);
                if (ec2)
                   rsj::Log(
                       fmt::format(FMT_STRING("LR_IPC_Out socket close error {}."), ec2.message()));
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
      for (const auto& cb : callbacks_) cb(true, sending_stopped_);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void LrIpcOut::MidiCmdCallback(const rsj::MidiMessage& mm)
{
   try {
      const rsj::MidiMessageId message {mm};
      if (profile_.MessageExistsInMap(message)) {
         const auto command_to_send {profile_.GetCommandForMessage(message)};
         if (command_to_send != "PrevPro" && command_to_send != "NextPro"
             && command_to_send != CommandSet::kUnassigned) { /* handled elsewhere */
            if (const auto a {repeat_cmd_.find(command_to_send)}; a != repeat_cmd_.end())
               [[unlikely]]
               {
                  static TimePoint next_response {};
                  if (const auto now {Clock::now()}; next_response < now) {
                     next_response = now + kDelay;
                     if ((mm.message_type_byte == rsj::MessageType::kCc
                             && controls_model_.GetCcMethod(message) == rsj::CCmethod::kAbsolute)
                         || mm.message_type_byte == rsj::MessageType::kPw)
                        SetRecenter(message);
                     const auto change {controls_model_.MeasureChange(mm)};
                     const auto [cw, ccw] {a->second};
                     if (change > 0)
                        SendCommand(cw); /* turned clockwise */
                     else if (change < 0)
                        SendCommand(ccw); /* turned counterclockwise */
                     /* do nothing if change == 0 */
                  }
               }
            else { /* not repeated command */
               const auto wrap {
                   std::find(wrap_.begin(), wrap_.end(), command_to_send) != wrap_.end()};
               const auto computed_value {controls_model_.ControllerToPlugin(mm, wrap)};
               SendCommand(fmt::format(FMT_STRING("{} {}\n"), command_to_send, computed_value));
            }
         }
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
      if (command_copy->back() != '\n') /* should be terminated with \n */
         [[unlikely]] command_copy->push_back('\n');
      // ReSharper disable once CppLambdaCaptureNeverUsed
      asio::async_write(socket_, asio::buffer(*command_copy),
          [this, command_copy](const asio::error_code& error, std::size_t) {
             if (!error)
                [[likely]] SendOut();
             else {
                rsj::Log(fmt::format(FMT_STRING("LR_IPC_Out Write: {}."), error.message()));
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