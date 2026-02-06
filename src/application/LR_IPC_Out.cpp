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

#include <dry-comparisons/dry-comparisons.hpp>
#include <fmt/format.h>

#include "CommandSet.h"
#include "Concurrency.h"
#include "ControlsModel.h"
#include "MIDIReceiver.h"
#include "MIDISender.h"
#include "Misc.h"
#include "Profile.h"

using namespace std::literals::chrono_literals;

namespace {
   constexpr auto kDelay {8ms}; /* in between recurrent actions */
   constexpr auto kLrOutPort {58763};
   constexpr auto kMinRecenterTime {250ms}; /* minimum period before recentering */
   constexpr auto kRecenterTimer {std::max(kMinRecenterTime, kDelay + kDelay / 2)};
   constexpr auto kTerminate {"!!!@#$%^"};
} // namespace

class LrIpcOutShared {
 private:
   friend LrIpcOut;
   asio::ip::tcp::socket socket_;
   rsj::ConcurrentQueue<std::string> command_;
   /* owner_alive_ is a shared atomic token that async handlers check before touching owner state.
    * Stop() sets this to false so outstanding async handlers can bail safely without dereferencing
    * the owning object. */
   std::shared_ptr<std::atomic<bool>> owner_alive_ {std::make_shared<std::atomic<bool>>(true)};
   static void SendOut(std::shared_ptr<LrIpcOutShared> lr_ipc_out_shared);

 public:
   explicit LrIpcOutShared(asio::io_context& io_context) : socket_ {asio::make_strand(io_context)}
   {
   }
};

LrIpcOut::LrIpcOut(const CommandSet& command_set, ControlsModel& c_model, const Profile& profile,
    const MidiSender& midi_sender, MidiReceiver& midi_receiver, asio::io_context& io_context)
    : timer_strand_ {asio::make_strand(io_context)}, midi_sender_ {midi_sender}, profile_ {profile},
      repeat_cmd_ {command_set.GetRepeats()}, wrap_ {command_set.GetWraps()},
      controls_model_ {c_model}, lr_ipc_out_shared_ {std::make_shared<LrIpcOutShared>(io_context)}
{
   midi_receiver.AddCallback(this, &LrIpcOut::MidiCmdCallback);
}

void LrIpcOut::SendCommand(std::string&& command) const
{
   if (!sending_stopped_.load(std::memory_order_acquire)) {
      lr_ipc_out_shared_->command_.push(std::move(command));
   }
}

void LrIpcOut::SendCommand(const std::string& command) const
{
   if (!sending_stopped_.load(std::memory_order_acquire)) {
      lr_ipc_out_shared_->command_.push(command);
   }
}

void LrIpcOut::SendingRestart()
{
   try {
      {
         std::scoped_lock lk(callback_mtx_);
         sending_stopped_.store(false, std::memory_order_release);
         for (const auto& cb : callbacks_) { cb(connected_, false); }
      }
      SendCommand("FullRefresh 1\n"); /* synchronize controls */
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void LrIpcOut::SendingStop()
{
   try {
      std::scoped_lock lk(callback_mtx_);
      sending_stopped_.store(true, std::memory_order_release);
      for (const auto& cb : callbacks_) { cb(connected_, true); }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void LrIpcOut::Stop()
{
   lr_ipc_out_shared_->owner_alive_->store(false, std::memory_order_release);
   /* clear output queue before port closed */
   if (const auto m {lr_ipc_out_shared_->command_.clear_count_emplace(kTerminate)}) {
      rsj::Log(fmt::format("{} left in queue in LrIpcOut destructor.", m),
          std::source_location::current());
   }
   {
      std::scoped_lock lk(callback_mtx_);
      callbacks_.clear(); /* no more connect/disconnect notifications */
   }
   {
      std::scoped_lock lk2(recenter_timers_mtx_);
      for (auto& [_, centerinfo] : recenter_timers_) {
         asio::error_code ec;
         centerinfo.timer.cancel(ec);
         if (ec) {
            rsj::Log(fmt::format("Error cancelling recenter timer: {}.", ec.message()),
                std::source_location::current());
         }
      }
   }
   if (auto& sock {lr_ipc_out_shared_->socket_}; sock.is_open()) {
      asio::error_code ec;
      /* For portable behaviour with respect to graceful closure of a connected socket, call
       * shutdown() before closing the socket. */
      try { /* ignore exceptions from shutdown, always close */
         std::ignore = sock.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
         if (ec) {
            rsj::Log(fmt::format("LR_IPC_Out socket shutdown error {}.", ec.message()),
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
         rsj::Log(fmt::format("LR_IPC_Out socket close error {}.", ec.message()),
             std::source_location::current());
      }
   }
}

void LrIpcOut::Connect(std::shared_ptr<LrIpcOutShared> lr_ipc_out_shared)
{
   try {
      lr_ipc_out_shared->socket_.async_connect(
          asio::ip::tcp::endpoint(asio::ip::address_v4::loopback(), kLrOutPort),
          [this, lr_ipc_out_shared](const asio::error_code& error) mutable {
         // copy shared alive token so the handler can detect owner shutdown
         auto alive {lr_ipc_out_shared->owner_alive_};
         if (!alive || !alive->load(std::memory_order_acquire)) { return; }
         if (!error) {
            // check again just before calling member functions
            if (alive->load(std::memory_order_acquire)) { ConnectionMade(); }
            LrIpcOutShared::SendOut(std::move(lr_ipc_out_shared));
         }
         else {
            rsj::Log(fmt::format("LR_IPC_Out did not connect. {}.", error.message()),
                std::source_location::current());
            asio::error_code ec2;
            std::ignore = lr_ipc_out_shared->socket_.close(ec2);
            if (ec2) {
               rsj::Log(fmt::format("LR_IPC_Out socket close error {}.", ec2.message()),
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

void LrIpcOut::ConnectionMade()
{
   try {
      {
         std::scoped_lock lk(callback_mtx_);
         connected_ = true;
         auto stopped {sending_stopped_.load(std::memory_order_acquire)};
         for (const auto& cb : callbacks_) { cb(true, stopped); }
      }
      rsj::Log("Socket connected in LR_IPC_Out.", std::source_location::current());
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void LrIpcOut::MidiCmdCallback(rsj::MidiMessage mm)
{
   try {
      const rsj::MidiMessageId message {mm};
      if (profile_.MessageExistsInMap(message)) { ProcessMessage(message, mm); }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void LrIpcOut::ProcessMessage(const rsj::MidiMessageId& message, const rsj::MidiMessage& mm)
{
   const auto command_to_send {profile_.GetCommandForMessage(message)};
   if (rollbear::none_of("PrevPro", "NextPro", CommandSet::kUnassigned) == command_to_send) {
      if (const auto repeats {repeat_cmd_.find(command_to_send)}; repeats == repeat_cmd_.end()) {
         SendNonRepeatedCommand(command_to_send, mm);
      }
      else {
         ProcessRepeatedCommand(repeats, mm, message);
      }
   }
}

void LrIpcOut::ProcessRepeatedCommand(const RepeatCmdIterator& repeats, const rsj::MidiMessage& mm,
    const rsj::MidiMessageId& message)
{
   auto lock {std::unique_lock(recenter_timers_mtx_)}; // Always lock first keep iterator stable
   auto [it, _] {recenter_timers_.try_emplace(message, timer_strand_)};
   if (const auto now {Clock::now()}; it->second.timepoint < now) {
      it->second.timepoint = now + kDelay;
      if (ShouldSetRecenter(mm)) { SetRecenter(message, it->second); }
      lock.unlock(); // Unlock before sending command
      SendRepeatedCommand(repeats, mm);
   }
}

bool LrIpcOut::ShouldSetRecenter(const rsj::MidiMessage& mm) const
{
   return (mm.message_type_byte == rsj::MessageType::kCc
              && controls_model_.GetCcMethod(mm.channel, mm.control_number)
                     == rsj::CCmethod::kAbsolute)
          || mm.message_type_byte == rsj::MessageType::kPw;
}

void LrIpcOut::SendRepeatedCommand(const RepeatCmdIterator& repeats,
    const rsj::MidiMessage& mm) const
{
   if (const auto change {controls_model_.MeasureChange(mm)}; change > 0) {
      SendCommand(repeats->second.first); /* turned clockwise */
   }
   else if (change < 0) {
      SendCommand(repeats->second.second); /* turned counterclockwise */
   }
}

void LrIpcOut::SendNonRepeatedCommand(const std::string& command_to_send,
    const rsj::MidiMessage& mm) const
{
#ifdef __cpp_lib_ranges_contains
   const auto wrap {std::ranges::contains(wrap_, command_to_send)};
#else
   const auto wrap {std::ranges::find(wrap_, command_to_send) != wrap_.end()};
#endif
   const auto computed_value {controls_model_.ControllerToPlugin(mm, wrap)};
   SendCommand(fmt::format("{} {}\n", command_to_send, computed_value));
}

void LrIpcOutShared::SendOut(std::shared_ptr<LrIpcOutShared> lr_ipc_out_shared)
{
   try {
      auto command_copy {std::make_shared<std::string>(lr_ipc_out_shared->command_.pop())};
      if (*command_copy == kTerminate) [[unlikely]] { return; }
      if (command_copy->empty() || command_copy->back() != '\n') [[unlikely]] {
         command_copy->push_back('\n');
      } // ReSharper disable once CppLambdaCaptureNeverUsed
      asio::async_write(lr_ipc_out_shared->socket_, asio::buffer(*command_copy),
          [command_copy, lr_ipc_out_shared](const asio::error_code& error, std::size_t) mutable {
         // check owner token before re-arming
         const auto alive {lr_ipc_out_shared->owner_alive_};
         if (!alive || !alive->load(std::memory_order_acquire)) { return; }

         if (!error) [[likely]] { SendOut(std::move(lr_ipc_out_shared)); }
         else {
            rsj::Log(fmt::format("LR_IPC_Out Write: {}.", error.message()),
                std::source_location::current());
         }
      });
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void LrIpcOut::SetRecenter(const rsj::MidiMessageId& mm, RecenterInfo& info)
{
   try {
      info.timer.expires_after(kRecenterTimer);
      info.timer.async_wait(
          [this, mm, alive = lr_ipc_out_shared_->owner_alive_](const asio::error_code& error) {
         if (error) { return; } // Handle cancellation/errors
         if (!alive || !alive->load(std::memory_order_acquire)) { return; }
         midi_sender_.Send(mm, controls_model_.SetToCenter(mm));
      });
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}