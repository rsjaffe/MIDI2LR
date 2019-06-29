#ifndef MIDI2LR_LR_IPC_IN_H_INCLUDED
#define MIDI2LR_LR_IPC_IN_H_INCLUDED
/*
  ==============================================================================

    LR_IPC_In.h

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
#include <atomic>
#include <future>
#include <memory>
#include <string>

#include <asio.hpp>
#include "Concurrency.h"
class ControlsModel;
class LrIpcOut;
class MidiSender;
class Profile;
class ProfileManager;
class LrIpcIn final : public std::enable_shared_from_this<LrIpcIn> {
 public:
   LrIpcIn(ControlsModel& c_model, ProfileManager& profile_manager, Profile& profile,
       std::shared_ptr<MidiSender> midi_sender, std::weak_ptr<LrIpcOut>&& out);
   ~LrIpcIn() = default;
   LrIpcIn(const LrIpcIn& other) = delete;
   LrIpcIn(LrIpcIn&& other) = delete;
   LrIpcIn& operator=(const LrIpcIn& other) = delete;
   LrIpcIn& operator=(LrIpcIn&& other) = delete;
   void StartRunning();
   void StopRunning();

 private:
   asio::io_context io_context_{1};
   asio::ip::tcp::socket socket_{io_context_};
   asio::steady_timer connect_timer_{io_context_};
   asio::streambuf streambuf_{};
   ControlsModel& controls_model_;
   Profile& profile_;
   ProfileManager& profile_manager_;
   rsj::BlockingQueue<std::string> line_;
   std::atomic<bool> thread_should_exit_{false};
   std::future<void> io_thread_;
   std::future<void> process_line_future_;
   std::shared_ptr<MidiSender> midi_sender_{nullptr};
   std::weak_ptr<LrIpcOut> ipc_out_;
   void Connect();
   void ProcessLine();
   void Read();
};

#endif // LR_IPC_IN_H_INCLUDED
