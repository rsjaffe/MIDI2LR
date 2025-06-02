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
#include <charconv>
#include <exception>
#include <string>
#include <string_view> //ReSharper false alarm
#include <thread>
#include <tuple>
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
      rsj::ExceptionResponse(e, std::source_location::current());
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
         try { /* ignore exceptions from shutdown, always close */
            std::ignore = sock.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
            if (ec) {
               rsj::Log(fmt::format("LR_IPC_In socket shutdown error {}.", ec.message()),
                   std::source_location::current());
               ec.clear();
            }
         }
         catch (const std::exception& e) {
            rsj::Log(fmt::format("Exception during socket shutdown: {}", e.what()),
                std::source_location::current());
         }
         std::ignore = sock.close(ec);
         if (ec) {
            rsj::Log(fmt::format("LR_IPC_In socket close error {}.", ec.message()),
                std::source_location::current());
         }
      }
      /* clear input queue after port closed */
      if (const auto m {lr_ipc_in_shared_->line_.clear_count_emplace(kTerminate)}) {
         rsj::Log(fmt::format("{} left in queue in LrIpcIn destructor.", m),
             std::source_location::current());
      }
   }

   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
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
            rsj::Log("Socket connected in LR_IPC_In.", std::source_location::current());
            LrIpcInShared::Read(std::move(lr_ipc_shared));
         }
         else {
            rsj::Log(fmt::format("LR_IPC_In did not connect. {}.", error.message()),
                std::source_location::current());
            asio::error_code ec2;
            std::ignore = lr_ipc_shared->socket_.close(ec2);
            if (ec2) {
               rsj::Log(fmt::format("LR_IPC_In socket close error {}.", ec2.message()),
                   std::source_location::current());
            }
         }
      });
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

namespace {
   [[nodiscard]] std::string_view Trim(std::string_view str)
   {
      const auto first {str.find_first_not_of(" \t\n")};
      if (first == std::string_view::npos) { return {}; }
      const auto last {str.find_last_not_of(" \t\n")};
      return str.substr(first, last - first + 1);
   }

   [[nodiscard]] std::string_view TrimL(std::string_view str)
   {
      const auto first {str.find_first_not_of(" \t\n")};
      if (first == std::string_view::npos) { return {}; }
      return str.substr(first);
   }

   std::pair<std::string_view, std::string_view> SplitLine(std::string_view msg)
   {
      msg = Trim(msg);
      const auto first_delimiter {msg.find_first_of(" \t\n")};
      if (first_delimiter == std::string_view::npos) { return {msg, {}}; }
      return {msg.substr(0, first_delimiter), TrimL(msg.substr(first_delimiter + 1))};
   }
} // namespace

// ReSharper disable once CppPassValueParameterByConstReference
void LrIpcIn::ProcessLine(std::shared_ptr<LrIpcInShared> lr_ipc_shared)
{
   try {
      decltype(lr_ipc_shared->line_)::value_type line_copy;
      while ((line_copy = lr_ipc_shared->line_.pop()) != kTerminate) {
#pragma warning(suppress : 26445) /* copying views; otherwise dangling references */
         auto [command_view, value_view] = SplitLine(line_copy);

         /* Fast path for known commands */
         if (command_view == "TerminateApplication") {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
            return;
         }

         if (value_view.empty()) {
            rsj::Log(fmt::format("No value attached to message. Message from plugin was \"{}\".",
                         rsj::ReplaceInvisibleChars(line_copy)),
                std::source_location::current());
            continue;
         }

         if (command_view == "SwitchProfile") {
            profile_manager_.SwitchToProfile(std::string(value_view));
            continue;
         }

         if (command_view == "Log") {
            rsj::Log(fmt::format("Plugin: {}.", value_view), std::source_location::current());
            continue;
         }

         if (command_view == "SendKey") {
            int modifiers {0};
            auto [ptr, ec] {std::from_chars(value_view.data(),
                value_view.data() + value_view.size(), modifiers)};
            /* Find the first non-digit after parsing modifiers */
            std::string_view key_view {value_view};
            key_view.remove_prefix(static_cast<size_t>(ptr - value_view.data()));
            if (!key_view.empty() && (key_view.front() == ' ' || key_view.front() == '\t')) {
               key_view.remove_prefix(1);
            }
            if (!key_view.empty() && ec == std::errc()) {
               rsj::SendKeyDownUp(std::string(key_view),
                   rsj::ActiveModifiers::FromMidi2LR(modifiers));
               continue;
            }
            rsj::LogAndAlertError(fmt::format("SendKey couldn't identify keystroke. Message from "
                                              "plugin was \"{}\".",
                                      rsj::ReplaceInvisibleChars(line_copy)),
                std::source_location::current());
            continue;
         }

         /* Default: send associated messages to MIDI OUT devices */
         double original_value {0.0};
#if defined(_MSC_VER) || (__MAC_OS_X_VERSION_MIN_REQUIRED >= __MAC_13_3)
         auto [ptr, ec] {std::from_chars(value_view.data(), value_view.data() + value_view.size(),
             original_value)};
         if (ec != std::errc()) {
            rsj::LogAndAlertError(fmt::format("Failed to parse double from \"{}\".", value_view),
                std::source_location::current());
            continue;
         }
#else
         try {
            original_value = std::stod(std::string(value_view));
         }
         catch (const std::exception& ex) {
            rsj::LogAndAlertError(fmt::format("Failed to parse double from \"{}\": {}", value_view,
                                      ex.what()),
                std::source_location::current());
            continue;
         }
#endif
         const auto messages {profile_.GetMessagesForCommand(std::string(command_view))};
         for (const auto& msg : messages) {
            const auto value {controls_model_.PluginToController(msg, original_value)};
            if (msg.msg_id_type != rsj::MessageType::kCc
                || controls_model_.GetCcMethod(msg) == rsj::CCmethod::kAbsolute) {
               midi_sender_.Send(msg, value);
            }
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void LrIpcInShared::Read(std::shared_ptr<LrIpcInShared> lr_ipc_shared)
{
   using namespace std::string_literals;
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
               rsj::Log(fmt::format("LR_IPC_In Read error: {}.", error.message()),
                   std::source_location::current());

               if (error == asio::error::misc_errors::eof) { /* LR closed socket */
                  juce::JUCEApplication::getInstance()->systemRequestedQuit();
               }
            }
         });
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}