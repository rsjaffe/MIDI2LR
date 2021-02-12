#ifndef MIDI2LR_LR_IPC_IN_H_INCLUDED
#define MIDI2LR_LR_IPC_IN_H_INCLUDED
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
#include <atomic>
#include <future>
#include <string>

#include <asio.hpp>

#include "Concurrency.h"
class ControlsModel;
class MidiSender;
class Profile;
class ProfileManager;
class LrIpcIn {
 public:
   LrIpcIn(ControlsModel& c_model, ProfileManager& profile_manager, const Profile& profile,
       const MidiSender& midi_sender, asio::io_context& io_context);
   ~LrIpcIn() = default;
   LrIpcIn(const LrIpcIn& other) = delete;
   LrIpcIn(LrIpcIn&& other) = delete;
   LrIpcIn& operator=(const LrIpcIn& other) = delete;
   LrIpcIn& operator=(LrIpcIn&& other) = delete;
   void Start();
   void Stop();

 private:
   void Connect();
   void ProcessLine();
   void Read();

   asio::ip::tcp::socket socket_;
   asio::streambuf streambuf_ {};
   const MidiSender& midi_sender_;
   const Profile& profile_;
   ControlsModel& controls_model_;
   ProfileManager& profile_manager_;
   rsj::ConcurrentQueue<std::string> line_;
   std::atomic<bool> thread_should_exit_ {false};
   std::future<void> process_line_future_;
};

#endif
