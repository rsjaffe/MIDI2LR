/*
  ==============================================================================

    LR_IPC_In.cpp

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
#include "LR_IPC_In.h"

#include <array>
#include <exception>
#include <string>
#include <string_view>

#include <gsl/gsl>
#include "CommandMap.h"
#include "ControlsModel.h"
#include "MIDISender.h"
#include "Misc.h"
#include "MidiUtilities.h"
#include "ProfileManager.h"
#include "SendKeys.h"

namespace {
   constexpr auto kHost = "127.0.0.1";
   constexpr auto kTerminate{"MBxegp3VXilFy0"};
   constexpr int kBufferSize = 1024;
   constexpr int kConnectTryTime = 100;
   constexpr int kEmptyWait = 100;
   constexpr int kLrInPort = 58764;
   constexpr int kNotConnectedWait = 333;
   constexpr int kReadyWait = 1000;
   constexpr int kStopWait = 1000;
   constexpr int kConnectTimer = 1000;
} // namespace

LrIpcIn::LrIpcIn(ControlsModel& c_model, ProfileManager& profile_manager, CommandMap& command_map)
    : juce::Thread{"LR_IPC_IN"}, command_map_{command_map}, controls_model_{c_model},
      profile_manager_{profile_manager}
{
}

#pragma warning(push)
#pragma warning(disable : 26447)
LrIpcIn::~LrIpcIn()
{
   try {
      {
         auto lock = std::lock_guard(timer_mutex_);
         timer_off_ = true;
         juce::Timer::stopTimer();
      }
      if (!juce::Thread::stopThread(kStopWait))
         rsj::Log("stopThread failed in LrIpcIn destructor");
      if (const auto m = line_.size_approx())
         rsj::Log(juce::String(m) + " left in queue in LrIpcIn destructor");
      std::string line_copy{};
      while (line_.try_dequeue(line_copy)) {
         /* pump the queue empty */
      }
      line_.enqueue(kTerminate);
      socket_.close();
   }
   catch (...) {
      rsj::LogAndAlertError("Exception thrown in LrIpcIn destructor.");
      std::terminate();
   }
}
#pragma warning(pop)

void LrIpcIn::Init(std::shared_ptr<MidiSender> midi_sender)
{
   try {
      midi_sender_ = std::move(midi_sender);
      // start the timer
      juce::Timer::startTimer(kConnectTimer);
      process_line_future_ = std::async(std::launch::async, &LrIpcIn::ProcessLine, this);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void LrIpcIn::PleaseStopThread()
{
   juce::Thread::signalThreadShouldExit();
   juce::Thread::notify();
}

void LrIpcIn::run()
{
   try {
      auto _ = gsl::finally([this] {
         auto lock = std::lock_guard(timer_mutex_);
         timer_off_ = true;
         juce::Timer::stopTimer();
      });
      while (!juce::Thread::threadShouldExit()) {
         std::array<char, kBufferSize> line{}; // zero filled by {} initialization
         // if not connected, executes a wait 333 then goes back to while. if connected, tries to
         // read a line, checks thread status and connection status before each read attempt.
         // Doesn't terminate thread if disconnected, as currently don't have graceful way to
         // restart thread.
         if (!socket_.isConnected()) {
            juce::Thread::wait(kNotConnectedWait);
         } // end if (is not connected)
         else {
            line.fill(0); // zero out buffer
            auto size_read = 0;
            // parse input until we have a line, then process that line, quit if connection lost
            while ((size_read == 0 || line.at(gsl::narrow_cast<size_t>(size_read) - 1) != '\n')
                   && socket_.isConnected()) {
               if (juce::Thread::threadShouldExit())
                  return; // after final action
               const auto wait_status = socket_.waitUntilReady(true, kReadyWait);
               switch (wait_status) {
               case -1:
#pragma warning(suppress : 26438)
                  goto dumpLine; // read line failed, break out of switch and while
               case 0:
                  juce::Thread::wait(kEmptyWait);
                  break; // try again to read until char shows up
               case 1:
                  switch (socket_.read(&line.at(size_read), 1, false)) {
                  case -1:
#pragma warning(suppress : 26438)
                     goto dumpLine; // read error
                  case 1:
                     size_read++;
                     break;
                  case 0:
                     // waitUntilReady returns 1 but read will is 0: it's an indication of a broken
                     // socket.
                     juce::JUCEApplication::getInstance()->systemRequestedQuit();
                     break;
                  default:
                     Ensures(!"Unexpected value for read status");
                  }
                  break;
               default:
                  Ensures(!"Unexpected value for wait_status");
               }
            } // end while !\n and is connected
            // if lose connection, line may not be terminated
            {
               std::string param{line.data()};
               if (param.back() == '\n') {
                  line_.enqueue(std::move(param));
               }
            } // scope param
         dumpLine: /* empty statement */;
         } // end else (is connected)
      }    // while not threadshouldexit, finally handles exit code
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void LrIpcIn::timerCallback()
{
   try {
      auto lock = std::lock_guard(timer_mutex_);
      if (!timer_off_ && !socket_.isConnected() && !juce::Thread::threadShouldExit()) {
         if (socket_.connect(kHost, kLrInPort, kConnectTryTime))
            if (!thread_started_) {
               juce::Thread::startThread(); // avoid starting thread during shutdown
               thread_started_ = true;
            }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

namespace {
   void Trim(std::string_view& value) noexcept
   {
      value.remove_prefix(std::min(value.find_first_not_of(" \t\n"), value.size()));
      if (const auto tr = value.find_last_not_of(" \t\n"); tr != std::string_view::npos)
         value.remove_suffix(value.size() - tr - 1);
   }
} // namespace

void LrIpcIn::ProcessLine()
{
   using namespace std::literals::string_literals;
   try {
      const static std::unordered_map<std::string, int> kCmds = {
          {"SwitchProfile"s, 1}, {"SendKey"s, 2}, {"TerminateApplication"s, 3}};
      do {
         // process input into [parameter] [Value]
         std::string line_copy{};
         if (!line_.try_dequeue(line_copy))
            line_.wait_dequeue(line_copy);
         if (line_copy == kTerminate)
            return;
         std::string_view v{line_copy};
         Trim(v);
         auto value_string{v.substr(v.find_first_of(" \t\n") + 1)};
         value_string.remove_prefix(
             std::min(value_string.find_first_not_of(" \t\n"), value_string.size()));
         const auto command{std::string(
             v.substr(0, v.find_first_of(" \t\n")))}; // use this a lot, so convert to string once

         switch (kCmds.count(command) ? kCmds.at(command) : 0) {
         case 1: // SwitchProfile
            profile_manager_.SwitchToProfile(std::string(value_string));
            break;
         case 2: // SendKey
         {
            const auto modifiers = std::stoi(std::string(value_string));
            // trim twice on purpose: first digit, then spaces, as key may be digit
            value_string.remove_prefix(
                std::min(value_string.find_first_not_of("0123456789"), value_string.size()));
            value_string.remove_prefix(
                std::min(value_string.find_first_not_of(" \t\n"), value_string.size()));
            rsj::ActiveModifiers am;
            if (modifiers & 0x1)
               am.alt_opt = true;
            if (modifiers & 0x2)
               am.control = true;
            if (modifiers & 0x4)
               am.shift = true;
            if (modifiers & 0x8)
               am.command = true;
            rsj::SendKeyDownUp(std::string(value_string), am);
            break;
         }
         case 3: // TerminateApplication
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
            return;
         case 0:
            // send associated messages to MIDI OUT devices
            if (midi_sender_) {
               const auto original_value = std::stod(std::string(value_string));
               for (const auto msg : command_map_.GetMessagesForCommand(command)) {
                  short msgtype{0};
                  switch (msg.msg_id_type) {
                  case rsj::MsgIdEnum::kNote:
                     msgtype = rsj::kNoteOnFlag;
                     break;
                  case rsj::MsgIdEnum::kCc:
                     msgtype = rsj::kCcFlag;
                     break;
                  case rsj::MsgIdEnum::kPitchBend:
                     msgtype = rsj::kPwFlag;
                  }
                  const auto value = controls_model_.PluginToController(msgtype,
                      gsl::narrow_cast<size_t>(msg.channel - 1), gsl::narrow_cast<short>(msg.data),
                      original_value);
                  if (midi_sender_) {
                     switch (msgtype) {
                     case rsj::kNoteOnFlag:
                        midi_sender_->SendNoteOn(msg.channel, msg.data, value);
                        break;
                     case rsj::kCcFlag:
                        if (controls_model_.GetCcMethod(gsl::narrow_cast<size_t>(msg.channel - 1),
                                gsl::narrow_cast<short>(msg.data))
                            == rsj::CCmethod::kAbsolute)
                           midi_sender_->SendCc(msg.channel, msg.data, value);
                        break;
                     case rsj::kPwFlag:
                        midi_sender_->SendPitchWheel(msg.channel, value);
                        break;
                     default:
                        Ensures(!"Unexpected result for msgtype");
                     }
                  }
               }
            }
            break;
         default:
            Ensures(!"Unexpected result for cmds");
         }
      } while (true);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}