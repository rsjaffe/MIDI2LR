/*
  ==============================================================================

    LR_IPC_OUT.cpp

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/
#include "LR_IPC_Out.h"

#include <algorithm>
#include <chrono>
#include <exception>
#include <string>
#include <unordered_map>
#include <utility>

#include "ControlsModel.h"
#include "MIDIReceiver.h"
#include "MIDISender.h"
#include "MidiUtilities.h"
#include "Misc.h"
#include "Profile.h"

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
using namespace std::literals::chrono_literals;

namespace {
   constexpr auto kDelay{8ms}; // in between recurrent actions
   constexpr auto kLrOutPort{58763};
   // give controller a minimum refractory period before resetting
   constexpr auto kMinRecenterTime(250ms);
   constexpr auto kRecenterTimer{std::max(kMinRecenterTime, kDelay + kDelay / 2)};
   constexpr auto kTerminate{"!!!@#$%^"};
} // namespace

LrIpcOut::LrIpcOut(ControlsModel& c_model, const Profile& profile, const MidiSender& midi_sender,
    MidiReceiver& midi_receiver)
    : midi_sender_{midi_sender}, profile_{profile}, controls_model_{c_model}
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
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
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
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void LrIpcOut::SendingRestart()
{
   try {
      sending_stopped_ = false;
      const auto con{connected_.load(std::memory_order_acquire)};
      for (const auto& cb : callbacks_)
         cb(con, false);
      // resync controls
      SendCommand("FullRefresh 1\n");
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void LrIpcOut::SendingStop()
{
   try {
      sending_stopped_ = true;
      const auto con{connected_.load(std::memory_order_acquire)};
      for (const auto& cb : callbacks_)
         cb(con, true);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void LrIpcOut::StartRunning()
{
   try {
      Connect();
      io_thread0_ = std::async(std::launch::async, [this] {
         rsj::LabelThread(L"LrIpcOut io_thread0_");
         io_context_.run();
      });
      io_thread1_ = std::async(std::launch::async, [this] {
         rsj::LabelThread(L"LrIpcOut io_thread1_");
         io_context_.run();
      });
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void LrIpcOut::StopRunning()
{
   thread_should_exit_.store(true, std::memory_order_seq_cst);
   // pump output queue before port closed
   if (const auto m = command_.clear_count_emplace(kTerminate))
      rsj::Log(juce::String(m) + " left in queue in LrIpcOut destructor");
   callbacks_.clear(); // no more connect/disconnect notifications
   asio::post([this] {
      if (socket_.is_open()) {
         asio::error_code ec;
         // For portable behaviour with respect to graceful closure of a connected socket, call
         // shutdown() before closing the socket.
         socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
         if (ec) {
            rsj::Log("LR_IPC_Out socket shutdown error " + ec.message());
            ec.clear();
         }
         socket_.close(ec);
         if (ec)
            rsj::Log("LR_IPC_Out socket close error " + ec.message());
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
             else if (error) {
                rsj::Log("LR_IPC_Out did not connect. " + error.message());
                asio::error_code ec2;
                socket_.close(ec2);
                if (ec2)
                   rsj::Log("LR_IPC_Out socket close error " + ec2.message());
             }
          });
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void LrIpcOut::ConnectionMade()
{
   connected_.store(true, std::memory_order_release);
   try {
      rsj::Log("Socket connected in LR_IPC_Out");
      for (const auto& cb : callbacks_)
         cb(true, sending_stopped_);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void LrIpcOut::MidiCmdCallback(rsj::MidiMessage mm)
{
   struct RepeatMessage {
      std::string cw;
      std::string ccw;
   };
   try {
      const rsj::MidiMessageId message{mm};
      static const std::unordered_map<std::string, RepeatMessage> kCmdUpDown{
          {"ChangeBrushSize", {"BrushSizeLarger 1\n", "BrushSizeSmaller 1\n"}},
          {"ChangeCurrentSlider", {"SliderIncrease 1\n", "SliderDecrease 1\n"}},
          {"ChangeFeatherSize", {"BrushFeatherLarger 1\n", "BrushFeatherSmaller 1\n"}},
          {"ChangeLastDevelopParameter",
              {"IncrementLastDevelopParameter 1\n", "DecrementLastDevelopParameter 1\n"}},
          {"IncreaseDecreaseRating", {"IncreaseRating 1\n", "DecreaseRating 1\n"}},
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
      const auto command_to_send = profile_.GetCommandForMessage(message);
      if (command_to_send == "PrevPro" || command_to_send == "NextPro"
          || command_to_send == "Unmapped")
         return; // handled by ProfileManager
      // if it is a repeated command, change command_to_send appropriately
      if (const auto a = kCmdUpDown.find(command_to_send); a != kCmdUpDown.end()) {
         static TimePoint nextresponse{};
         if (const auto now = Clock::now(); nextresponse < now) {
            nextresponse = now + std::chrono::milliseconds(kDelay);
            if (mm.message_type_byte == rsj::MessageType::Pw
                || (mm.message_type_byte == rsj::MessageType::Cc
                       && controls_model_.GetCcMethod(mm.channel, mm.control_number)
                              == rsj::CCmethod::kAbsolute)) {
               SetRecenter(mm);
            }
            const auto change = controls_model_.MeasureChange(mm);
            if (change == 0)
               return;      // don't send any signal
            if (change > 0) // turned clockwise
               SendCommand(a->second.cw);
            else // turned counterclockwise
               SendCommand(a->second.ccw);
         }
      }
      else { // not repeated command
         const auto computed_value = controls_model_.ControllerToPlugin(mm);
         SendCommand(command_to_send + ' ' + std::to_string(computed_value) + '\n');
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void LrIpcOut::SendOut()
{
   try {
      auto command_copy = std::make_shared<std::string>(command_.pop());
      if (*command_copy == kTerminate)
         return;
      // always connected when running SendOut, no need to check flag
      if (command_copy->back() != '\n') // should be terminated with \n
         *command_copy += '\n';
      asio::async_write(socket_, asio::buffer(*command_copy),
          [this, command_copy](const asio::error_code& error, std::size_t) {
             if (!error)
                SendOut();
             else {
                rsj::Log("LR_IPC_Out Write: " + error.message());
             }
          });
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void LrIpcOut::SetRecenter(rsj::MidiMessage mm)
{ // by capturing mm by copy, don't have to worry about later calls changing it--those will just
  // cancel and reschedule new one
   try {
      asio::dispatch([this] { recenter_timer_.expires_after(kRecenterTimer); });
      // self passed to next lambda only, lifetime extends beyond above call, saves one shared_ptr
      // copy + atomic increment
      recenter_timer_.async_wait([this, mm](const asio::error_code& error) {
         if (!error && !thread_should_exit_.load(std::memory_order_relaxed)) {
            switch (mm.message_type_byte) {
            case rsj::MessageType::Pw: {
               midi_sender_.SendPitchWheel(mm.channel + 1, controls_model_.SetToCenter(mm));
               break;
            }
            case rsj::MessageType::Cc: {
               midi_sender_.SendCc(
                   mm.channel + 1, mm.control_number, controls_model_.SetToCenter(mm));
               break;
            }
            default:
                /* no action */;
            }
         }
      });
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}