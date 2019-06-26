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

#include <algorithm>
#include <exception>
// ReSharper disable once CppUnusedIncludeDirective
#include <string_view>
#include <thread>

#include <gsl/gsl>
#include "ControlsModel.h"
#include "LR_IPC_Out.h"
#include "MIDISender.h"
#include "MidiUtilities.h"
#include "Misc.h"
#include "Profile.h"
#include "ProfileManager.h"
#include "SendKeys.h"
using namespace std::literals::chrono_literals;

namespace {
   constexpr auto kEmptyWait = 100ms;
   constexpr auto kLrInPort = 58764;
   constexpr auto kTerminate = "MBxegp3VXilFy0";

   void Trim(std::string_view& value) noexcept
   {
      value.remove_prefix(std::min(value.find_first_not_of(" \t\n"), value.size()));
      if (const auto tr = value.find_last_not_of(" \t\n"); tr != std::string_view::npos)
         value.remove_suffix(value.size() - tr - 1);
   }
} // namespace

LrIpcIn::LrIpcIn(ControlsModel& c_model, ProfileManager& profile_manager, Profile& profile,
    std::shared_ptr<MidiSender> midi_sender, std::weak_ptr<LrIpcOut>&& out)
    : controls_model_{c_model}, profile_{profile}, profile_manager_{profile_manager},
      midi_sender_{std::move(midi_sender)}, ipc_out_{std::move(out)}
{
}

void LrIpcIn::StartRunning()
{
   try {
      process_line_future_ = std::async(std::launch::async, &LrIpcIn::ProcessLine, this);
      Connect();
      io_thread_ = std::async(std::launch::async, [this] { io_context_.run(); });
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void LrIpcIn::StopRunning()
{
   try {
      thread_should_exit_ = true;
      if (connected_) {
         const auto self = shared_from_this();
         asio::post([this, self] {
            asio::error_code ec;
            // For portable behaviour with respect to graceful closure of a connected socket, call
            // shutdown() before closing the socket.
            socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
            if (ec) {
               rsj::Log("LR_IPC_In socket shutdown error " + ec.message());
               ec.clear();
            }
            socket_.close(ec);
            if (ec)
               rsj::Log("LR_IPC_In socket close error " + ec.message());
         });
      }
      if (const auto m = line_.clear_count_emplace(kTerminate))
         rsj::Log(juce::String(m) + " left in queue in LrIpcIn destructor");
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), "StopRunning", e);
      throw;
   }
}

void LrIpcIn::Connect()
{
   try {
      const auto self{shared_from_this()};
      socket_.async_connect(asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), kLrInPort),
          [this, self](const asio::error_code& error) {
             if (!error) {
                rsj::Log("Socket connected in LR_IPC_In");
                connected_ = true;
                Read();
             }
             else if (error) {
                asio::error_code ec2;
                socket_.close(ec2);
                if (ec2)
                   rsj::Log("LR_IPC_In socket close error " + ec2.message());
             }
          });
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void LrIpcIn::ProcessLine()
{
   try {
      const static std::unordered_map<std::string, int> kCmds = {
          {"SwitchProfile", 1}, {"SendKey", 2}, {"TerminateApplication", 3}};
      do {
         // process input into [parameter] [Value]
         auto line_copy{line_.pop()};
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
            // trim twice on purpose: first digit, then space, as key may be digit
            value_string.remove_prefix(
                std::min(value_string.find_first_not_of("0123456789"), value_string.size()));
            value_string.remove_prefix(1); // one space between number and character
            if (value_string.empty()) {
               rsj::LogAndAlertError(
                   "SendKey couldn't identify keystroke. Message from plugin was \""
                   + juce::String(rsj::ReplaceInvisibleChars(line_copy)) + "\".");
               break;
            }
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
               for (const auto& msg : profile_.GetMessagesForCommand(command)) {
                  const auto value = controls_model_.PluginToController(msg, original_value);
                  if (midi_sender_) {
                     switch (msg.msg_id_type) {
                     case rsj::MessageType::NoteOn:
                        midi_sender_->SendNoteOn(msg.channel, msg.control_number, value);
                        break;
                     case rsj::MessageType::Cc:
                        if (controls_model_.GetCcMethod(msg) == rsj::CCmethod::kAbsolute)
                           midi_sender_->SendCc(msg.channel, msg.control_number, value);
                        break;
                     case rsj::MessageType::Pw:
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

void LrIpcIn::Read()
{
   try {
      if (!thread_should_exit_) {
         const auto self{shared_from_this()};
         asio::async_read_until(socket_, streambuf_, '\n',
             [this, self](const asio::error_code& error, std::size_t bytes_transferred) {
                if (!error) {
                   if (!bytes_transferred)
                      std::this_thread::sleep_for(kEmptyWait);
                   else {
                      std::string command{buffers_begin(streambuf_.data()),
                          buffers_begin(streambuf_.data()) + bytes_transferred};
                      if (command == "TerminateApplication 1\n")
                         thread_should_exit_ = true;
                      line_.push(std::move(command));
                      streambuf_.consume(bytes_transferred);
                   }
                   Read(); // read again
                }
                else {
                   rsj::Log("LR_IPC_In Read: " + error.message());
                   if (error == asio::error::misc_errors::eof) // LR closed socket
                      juce::JUCEApplication::getInstance()->systemRequestedQuit();
                }
             });
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}