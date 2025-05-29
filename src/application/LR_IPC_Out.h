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
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <asio/asio.hpp>

#include "MidiUtilities.h"
class CommandSet;
class ControlsModel;
class LrIpcOutShared;
class MidiReceiver;
class MidiSender;
class Profile;

#ifndef _MSC_VER
#define _In_ //-V3547
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

   template<class T> void AddCallback(_In_ T* const object, _In_ void (T::* const mf)(bool, bool))
   {
      if (object && mf) {
         std::scoped_lock lk(callback_mtx_);
         callbacks_.emplace_back(std::bind_front(mf, object));
      }
   }

   void SendCommand(std::string&& command) const;
   void SendCommand(const std::string& command) const;
   void SendingRestart();
   void SendingStop();

   void Start() { Connect(lr_ipc_out_shared_); }

   void Stop();

 private:
   using RepeatCmdIterator =
       const std::unordered_map<std::string, std::pair<std::string, std::string>>::const_iterator;
   [[nodiscard]] bool ShouldSetRecenter(const rsj::MidiMessage& mm) const;
   void Connect(std::shared_ptr<LrIpcOutShared> lr_ipc_out_shared);
   void ConnectionMade();
   void MidiCmdCallback(rsj::MidiMessage mm);
   void ProcessChange(const RepeatCmdIterator& repeats, const rsj::MidiMessage& mm) const;
   void ProcessMessage(const rsj::MidiMessageId& message, const rsj::MidiMessage& mm);
   void ProcessNonRepeatedCommand(const std::string& command_to_send,
       const rsj::MidiMessage& mm) const;
   void ProcessRepeatedCommand(const RepeatCmdIterator& repeats, const rsj::MidiMessage& mm,
       const rsj::MidiMessageId& message);
   void SetRecenter(rsj::MidiMessageId mm);
   asio::steady_timer recenter_timer_;
   bool connected_ {false};
   bool sending_stopped_ {false};
   const MidiSender& midi_sender_;
   const Profile& profile_;
   const std::unordered_map<std::string, std::pair<std::string, std::string>>& repeat_cmd_;
   const std::vector<std::string>& wrap_;
   ControlsModel& controls_model_;
   mutable std::mutex callback_mtx_;
   std::atomic<bool> thread_should_exit_ {false};
   std::shared_ptr<LrIpcOutShared> lr_ipc_out_shared_;
#ifdef __cpp_lib_copyable_function
   std::vector<std::copyable_function<void(bool, bool)>> callbacks_ {};
#else
   std::vector<std::function<void(bool, bool)>> callbacks_ {};
#endif
};

#endif
