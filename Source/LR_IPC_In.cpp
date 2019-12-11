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
#include "LR_IPC_In.h"

#include <algorithm>
#include <exception>
// ReSharper disable once CppUnusedIncludeDirective
#include <string_view>
#include <thread>
#include <fmt/format.h>

#include <gsl/gsl>
#include "ControlsModel.h"
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
} // namespace

LrIpcIn::LrIpcIn(ControlsModel& c_model, ProfileManager& profile_manager, const Profile& profile,
    const MidiSender& midi_sender)
    : midi_sender_{midi_sender}, profile_{profile}, controls_model_{c_model}, profile_manager_{
                                                                                  profile_manager}
{
}

void LrIpcIn::Start()
{
   try {
      process_line_future_ = std::async(std::launch::async, [this] {
         rsj::LabelThread(L"LrIpcIn ProcessLine thread");
         _mm_setcsr(_mm_getcsr() | 0x8040);
         ProcessLine();
      });
      Connect();
      io_thread_ = std::async(std::launch::async, [this] {
         rsj::LabelThread(L"LrIpcIn io_thread_");
         _mm_setcsr(_mm_getcsr() | 0x8040);
         io_context_.run();
      });
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), MIDI2LR_FUNC, e);
      throw;
   }
}

void LrIpcIn::Stop()
{
   try {
      thread_should_exit_.store(true, std::memory_order_release);
      asio::post([this] {
         if (socket_.is_open()) {
            asio::error_code ec;
            /* For portable behaviour with respect to graceful closure of a connected socket, call
             * shutdown() before closing the socket. */
            socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
            if (ec) {
               rsj::Log(fmt::format("LR_IPC_In socket shutdown error {}.", ec.message()));
               ec.clear();
            }
            socket_.close(ec);
            if (ec)
               rsj::Log(fmt::format("LR_IPC_In socket close error {}.", ec.message()));
         }
         /* pump input queue after port closed */
         if (const auto m = line_.clear_count_emplace(kTerminate))
            rsj::Log(fmt::format("{} left in queue in LrIpcIn destructor.", m));
      });
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), "StopRunning", e);
      throw;
   }
}

void LrIpcIn::Connect()
{
   try {
      socket_.async_connect(asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), kLrInPort),
          [this](const asio::error_code& error) {
             if (!error) {
                rsj::Log("Socket connected in LR_IPC_In.");
                Read();
             }
             else if (error) {
                rsj::Log(fmt::format("LR_IPC_In did not connect. {}.", error.message()));
                asio::error_code ec2;
                socket_.close(ec2);
                if (ec2)
                   rsj::Log(fmt::format("LR_IPC_In socket close error {}.", ec2.message()));
             }
          });
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), MIDI2LR_FUNC, e);
      throw;
   }
}

namespace {
   std::pair<std::string_view, std::string_view> SplitLine(std::string_view msg)
   {
      rsj::Trim(msg);
      const auto first_delimiter{msg.find_first_of(" \t\n")};
      auto value_view{msg.substr(first_delimiter + 1)};
      rsj::TrimL(value_view);
      const auto command_view{msg.substr(0, first_delimiter)};
      return {command_view, value_view};
   }
} // namespace

void LrIpcIn::ProcessLine()
{
   try {
      do {
         const auto line_copy{line_.pop()};
         if (line_copy == kTerminate)
            return;
         auto [command_view, value_view] = SplitLine(line_copy);
         const auto command{std::string(command_view)};
         if (command == "TerminateApplication") {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
            return;
         }
         if (value_view.empty()) {
            rsj::Log(fmt::format("No value attached to message. Message from plugin was \"{}\".",
                rsj::ReplaceInvisibleChars(line_copy)));
         }
         else if (command == "SwitchProfile") {
            profile_manager_.SwitchToProfile(std::string(value_view));
         }
         else if (command == "Log") {
            rsj::Log(fmt::format("Plugin: {}.", value_view));
         }
         else if (command == "SendKey") {
            const auto modifiers = std::stoi(std::string(value_view));
            /* trim twice on purpose: first modifiers digits, then one space (fixed delimiter) */
            const auto first_not_digit{value_view.find_first_not_of("0123456789")};
            if (first_not_digit != std::string_view::npos) {
               value_view.remove_prefix(first_not_digit + 1);
               if (!value_view.empty()) {
                  rsj::SendKeyDownUp(
                      std::string(value_view), rsj::ActiveModifiers::FromMidi2LR(modifiers));
                  /* skip logandalert error */
                  continue;
               }
            }
            rsj::LogAndAlertError(
                fmt::format("SendKey couldn't identify keystroke. Message from plugin was \"{}\".",
                    rsj::ReplaceInvisibleChars(line_copy)));
         }
         else {
            /* send associated messages to MIDI OUT devices */
            const auto original_value = std::stod(std::string(value_view));
            for (const auto& msg : profile_.GetMessagesForCommand(command)) {
               /* following needs to run for all controls: sets saved value */
               const auto value = controls_model_.PluginToController(msg, original_value);
               if (msg.msg_id_type != rsj::MessageType::Cc
                   || controls_model_.GetCcMethod(msg) == rsj::CCmethod::kAbsolute)
                  midi_sender_.Send(msg, value);
            }
         }
      } while (true);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), MIDI2LR_FUNC, e);
      throw;
   }
}

void LrIpcIn::Read()
{
   try {
      if (!thread_should_exit_.load(std::memory_order_acquire)) {
         asio::async_read_until(socket_, streambuf_, '\n',
             [this](const asio::error_code& error, std::size_t bytes_transferred) {
                if (!error)
                   [[likely]]
                   {
                      if (!bytes_transferred)
                         [[unlikely]] std::this_thread::sleep_for(kEmptyWait);
                      else {
                         std::string command{buffers_begin(streambuf_.data()),
                             buffers_begin(streambuf_.data()) + bytes_transferred};
                         if (command == "TerminateApplication 1\n")
                            thread_should_exit_.store(true, std::memory_order_release);
                         line_.push(std::move(command));
                         streambuf_.consume(bytes_transferred);
                      }
                      Read();
                   }
                else {
                   rsj::Log(fmt::format("LR_IPC_In Read error: {}.", error.message()));
                   if (error == asio::error::misc_errors::eof)
                      /* LR closed socket */
                      juce::JUCEApplication::getInstance()->systemRequestedQuit();
                }
             });
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), MIDI2LR_FUNC, e);
      throw;
   }
}