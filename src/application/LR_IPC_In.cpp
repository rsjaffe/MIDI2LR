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
#include <atomic>
#include <exception>
#include <string>
#include <string_view> //ReSharper false alarm
#include <thread>
#include <utility>

#include <fmt/format.h>
#include <gsl/gsl>

#include <juce_audio_devices/juce_audio_devices.h> //ReSharper false alarm
#include <juce_gui_basics/juce_gui_basics.h>

#include "Concurrency.h"
#include "ControlsModel.h"
#include "MIDISender.h"
#include "MidiUtilities.h"
#include "Misc.h"
#include "Profile.h"
#include "ProfileManager.h"
#include "SendKeys.h"

using namespace std::literals::chrono_literals;
using namespace std::string_literals;

namespace {
   constexpr auto kEmptyWait {100ms};
   constexpr auto kLrInPort {58764};
   constexpr auto kTerminate {"MBxegp3VXilFy0"};
} // namespace

class LrIpcInShared {
 private:
   friend LrIpcIn;
   asio::ip::tcp::socket socket_;
   asio::streambuf streambuf_ {};
   rsj::ConcurrentQueue<std::string> line_;
   std::atomic<bool> thread_should_exit_ {false};
   static void Read(std::shared_ptr<LrIpcInShared> lr_ipc_shared);

 public:
   explicit LrIpcInShared(asio::io_context& io_context) : socket_ {asio::make_strand(io_context)} {}
};

LrIpcIn::LrIpcIn(ControlsModel& c_model, ProfileManager& profile_manager, const Profile& profile,
    const MidiSender& midi_sender, asio::io_context& io_context)
    : midi_sender_ {midi_sender}, profile_ {profile}, controls_model_ {c_model},
      profile_manager_ {profile_manager},
      lr_ipc_in_shared_ {std::make_shared<LrIpcInShared>(io_context)}
{
}

void LrIpcIn::Start()
{
   try {
      process_line_future_ =
          std::async(std::launch::async, [this, shared = lr_ipc_in_shared_]() mutable {
             rsj::LabelThread(MIDI2LR_UC_LITERAL("LrIpcIn ProcessLine thread"));
             MIDI2LR_FAST_FLOATS;
             ProcessLine(std::move(shared));
          });
      Connect(lr_ipc_in_shared_);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void LrIpcIn::Stop()
{
   try {
      lr_ipc_in_shared_->thread_should_exit_.store(true, std::memory_order_release);
      if (auto& sock {lr_ipc_in_shared_->socket_}; sock.is_open()) {
         asio::error_code ec;
         /* For portable behaviour with respect to graceful closure of a connected socket, call
          * shutdown() before closing the socket. */
         sock.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
         if (ec) {
            rsj::Log(fmt::format(FMT_STRING("LR_IPC_In socket shutdown error {}."), ec.message()));
            ec.clear();
         }
         sock.close(ec);
         if (ec) {
            rsj::Log(fmt::format(FMT_STRING("LR_IPC_In socket close error {}."), ec.message()));
         }
      }
      /* clear input queue after port closed */
      if (const auto m {lr_ipc_in_shared_->line_.clear_count_emplace(kTerminate)}) {
         rsj::Log(fmt::format(FMT_STRING("{} left in queue in LrIpcIn destructor."), m));
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void LrIpcIn::Connect(std::shared_ptr<LrIpcInShared> lr_ipc_shared)
{
   try {
      lr_ipc_shared->socket_.async_connect(asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(),
                                               kLrInPort),
          [lr_ipc_shared](const asio::error_code& error) mutable {
         if (!error) {
            rsj::Log("Socket connected in LR_IPC_In.");
            LrIpcInShared::Read(std::move(lr_ipc_shared));
         }
         else {
            rsj::Log(fmt::format(FMT_STRING("LR_IPC_In did not connect. {}."), error.message()));
            asio::error_code ec2;
            lr_ipc_shared->socket_.close(ec2);
            if (ec2) {
               rsj::Log(fmt::format(FMT_STRING("LR_IPC_In socket close error {}."), ec2.message()));
            }
         }
      });
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

namespace {
   std::pair<std::string_view, std::string_view> SplitLine(std::string_view msg)
   {
      rsj::Trim(msg);
      const auto first_delimiter {msg.find_first_of(" \t\n")};
      auto value_view {msg.substr(first_delimiter + 1)};
      rsj::TrimL(value_view);
      const auto command_view {msg.substr(0, first_delimiter)};
      return {command_view, value_view};
   }
} // namespace

void LrIpcIn::ProcessLine(std::shared_ptr<LrIpcInShared> lr_ipc_shared)
{
   try {
      for (auto line_copy = lr_ipc_shared->line_.pop(); line_copy != kTerminate;
           line_copy = lr_ipc_shared->line_.pop()) {
         auto [command_view, value_view] {SplitLine(line_copy)};
         const auto command {std::string(command_view)};
         if (command == "TerminateApplication"s) {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
            return;
         }
         if (value_view.empty()) {
            rsj::Log(fmt::format(FMT_STRING("No value attached to message. Message from plugin was "
                                            "\"{}\"."),
                rsj::ReplaceInvisibleChars(line_copy)));
         }
         else if (command == "SwitchProfile"s) {
            profile_manager_.SwitchToProfile(std::string(value_view));
         }
         else if (command == "Log"s) {
            rsj::Log(fmt::format(FMT_STRING("Plugin: {}."), value_view));
         }
         else if (command == "SendKey"s) {
            const auto modifiers {std::stoi(std::string(value_view))};
            /* trim twice on purpose: first modifiers digits, then one space (fixed delimiter) */
            const auto first_not_digit {value_view.find_first_not_of("0123456789")};
            if (first_not_digit != std::string_view::npos) {
               value_view.remove_prefix(first_not_digit + 1);
               if (!value_view.empty()) {
                  rsj::SendKeyDownUp(std::string(value_view),
                      rsj::ActiveModifiers::FromMidi2LR(modifiers));
                  continue; /* skip log and alert error */
               }
            }
            rsj::LogAndAlertError(fmt::format(FMT_STRING("SendKey couldn't identify keystroke. "
                                                         "Message from plugin was \"{}\"."),
                rsj::ReplaceInvisibleChars(line_copy)));
         }
         else { /* send associated messages to MIDI OUT devices */
            const auto original_value {std::stod(std::string(value_view))};
            for (const auto& msg : profile_.GetMessagesForCommand(command)) {
               /* following needs to run for all controls: sets saved value */
               const auto value {controls_model_.PluginToController(msg, original_value)};
               if (msg.msg_id_type != rsj::MessageType::kCc
                   || controls_model_.GetCcMethod(msg) == rsj::CCmethod::kAbsolute) {
                  midi_sender_.Send(msg, value);
               }
            }
         }
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void LrIpcInShared::Read(std::shared_ptr<LrIpcInShared> lr_ipc_shared)
{
   try {
      if (!lr_ipc_shared->thread_should_exit_.load(std::memory_order_acquire)) {
         asio::async_read_until(lr_ipc_shared->socket_, lr_ipc_shared->streambuf_, '\n',
             [lr_ipc_shared](const asio::error_code& error,
                 const std::size_t bytes_transferred) mutable {
            if (!error) [[likely]] {
               if (bytes_transferred == 0) [[unlikely]] { std::this_thread::sleep_for(kEmptyWait); }
               else {
                  auto& buf {lr_ipc_shared->streambuf_};
                  std::string command {buffers_begin(buf.data()),
                      buffers_begin(buf.data()) + gsl::narrow<std::ptrdiff_t>(bytes_transferred)};
                  if (command == "TerminateApplication 1\n"s) {
                     lr_ipc_shared->thread_should_exit_.store(true, std::memory_order_release);
                  }
                  lr_ipc_shared->line_.push(std::move(command));
                  buf.consume(bytes_transferred);
               }
               Read(std::move(lr_ipc_shared));
            }
            else {
               rsj::Log(fmt::format(FMT_STRING("LR_IPC_In Read error: {}."), error.message()));

               if (error == asio::error::misc_errors::eof) { /* LR closed socket */
                  juce::JUCEApplication::getInstance()->systemRequestedQuit();
               }
            }
         });
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE_F;
      throw;
   }
}