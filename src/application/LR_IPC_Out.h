#ifndef MIDI2LR_LR_IPC_OUT_H_INCLUDED
#define MIDI2LR_LR_IPC_OUT_H_INCLUDED
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
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <version>

#include <asio/asio.hpp>

#include "Concurrency.h"
#include "MidiUtilities.h"
class CommandSet;
class ControlsModel;
class MidiReceiver;
class MidiSender;
class Profile;

#ifdef __cpp_lib_semaphore
#include <semaphore>
#else
#include <condition_variable>
#include <mutex>
#endif

#ifndef _MSC_VER
#define _In_
#endif

class LrIpcOut {
 public:
   LrIpcOut(const CommandSet& command_set, ControlsModel& c_model, const Profile& profile,
       const MidiSender& midi_sender, MidiReceiver& midi_receiver, asio::io_context& io_context);
   ~LrIpcOut() = default;
   LrIpcOut(const LrIpcOut& other) = delete;
   LrIpcOut(LrIpcOut&& other) = delete;
   LrIpcOut& operator=(const LrIpcOut& other) = delete;
   LrIpcOut& operator=(LrIpcOut&& other) = delete;

   template<class T> void AddCallback(_In_ T* const object, _In_ void (T::*const mf)(bool, bool))
   {
      if (object && mf)
         callbacks_.emplace_back([=](bool a, bool b) { (object->*mf)(a, b); });
   }
   void SendCommand(std::string&& command)
   {
      if (!sending_stopped_)
         command_.push(std::move(command));
   }
   void SendCommand(const std::string& command)
   {
      if (!sending_stopped_)
         command_.push(command);
   }
   void SendingRestart();
   void SendingStop();
   void Start() { Connect(); }
   void Stop();

 private:
   void Connect();
   void ConnectionMade();
   void MidiCmdCallback(const rsj::MidiMessage&);
   void SendOut();
   void SetRecenter(rsj::MidiMessageId mm);
   asio::ip::tcp::socket socket_;
   asio::steady_timer recenter_timer_;
   bool sending_stopped_ {false};
   const MidiSender& midi_sender_;
   const Profile& profile_;
   const std::unordered_map<std::string, std::pair<std::string, std::string>>& repeat_cmd_;
   const std::vector<std::string>& wrap_;
   ControlsModel& controls_model_;
   rsj::ConcurrentQueue<std::string> command_;
   std::atomic<bool> connected_ {false};
   std::atomic<bool> thread_should_exit_ {false};
#ifdef __cpp_lib_semaphore
   std::binary_semaphore sendout_running_ {1};
#else
   bool sendout_running_ {false};
   std::mutex mtx_;
   std::condition_variable cv_;
#endif
   std::vector<std::function<void(bool, bool)>> callbacks_ {};
};

#endif
