#ifndef MIDI2LR_LR_IPC_OUT_H_INCLUDED
#define MIDI2LR_LR_IPC_OUT_H_INCLUDED
/*
  ==============================================================================

    LR_IPC_Out.h

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
#include <functional>
#include <future>
#include <string>
#include <vector>

#include <asio.hpp>
#include "Concurrency.h"
#include "MidiUtilities.h"
class ControlsModel;
class MidiReceiver;
class MidiSender;
class Profile;
#ifndef _MSC_VER
#define _In_
#endif

class LrIpcOut {
 public:
   LrIpcOut(ControlsModel& c_model, const Profile& profile, const MidiSender& midi_sender,
       MidiReceiver& midi_receiver);
   ~LrIpcOut() = default;
   LrIpcOut(const LrIpcOut& other) = delete;
   LrIpcOut(LrIpcOut&& other) = delete;
   LrIpcOut& operator=(const LrIpcOut& other) = delete;
   LrIpcOut& operator=(LrIpcOut&& other) = delete;

   template<class T> void AddCallback(_In_ T* const object, _In_ void (T::*const mf)(bool, bool))
   {
      using namespace std::placeholders;
      if (object && mf) // only store non-empty functions
         callbacks_.emplace_back(std::bind(mf, object, _1, _2));
   }

   void SendCommand(std::string&& command);
   void SendCommand(const std::string& command);
   void SendingRestart();
   void SendingStop();
   void StartRunning();
   void StopRunning();

 private:
   void Connect();
   void ConnectionMade();
   void MidiCmdCallback(const rsj::MidiMessage&);
   void SendOut();
   void SetRecenter(rsj::MidiMessageId mm);

   asio::io_context io_context_{};
   asio::ip::tcp::socket socket_{io_context_};
   asio::steady_timer recenter_timer_{io_context_};
   bool sending_stopped_{false};
   const MidiSender& midi_sender_;
   const Profile& profile_;
   ControlsModel& controls_model_;
   rsj::ConcurrentQueue<std::string> command_;
   std::atomic<bool> connected_{false};
   std::atomic<bool> thread_should_exit_{false};
   std::future<void> io_thread0_;
   std::future<void> io_thread1_; // need second thread for recenter timer
   std::vector<std::function<void(bool, bool)>> callbacks_{};
};

#endif // LR_IPC_OUT_H_INCLUDED
