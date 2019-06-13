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
#include <functional>
#include <future>
#include <mutex>
#include <string>
#include <vector>

#include <MoodyCamel/blockingconcurrentqueue.h>
#include <JuceLibraryCode/JuceHeader.h>
#include "Concurrency.h"
#include "MidiUtilities.h"
class ControlsModel;
class MidiReceiver;
class MidiSender;
class Profile;
#ifndef _MSC_VER
#define _In_
#endif

class LrIpcOut final : juce::InterprocessConnection {
 public:
   LrIpcOut(ControlsModel& c_model, const Profile& profile, std::shared_ptr<MidiSender> midi_sender,
       MidiReceiver& midi_receiver) noexcept;
   ~LrIpcOut();
   LrIpcOut(const LrIpcOut& other) = delete;
   LrIpcOut(LrIpcOut&& other) = delete;
   LrIpcOut& operator=(const LrIpcOut& other) = delete;
   LrIpcOut& operator=(LrIpcOut&& other) = delete;
   void Start();

   template<class T> void AddCallback(_In_ T* const object, _In_ void (T::*const mf)(bool, bool))
   {
      using namespace std::placeholders;
      if (object && mf) // only store non-empty functions
         callbacks_.emplace_back(std::bind(mf, object, _1, _2));
   }

   // sends a command to the plugin
   void SendCommand(std::string&& command);
   void SendCommand(const std::string& command);

   void Stop();
   void Restart();

 private:
   void connectionLost() override;
   void connectionMade() override;
   void messageReceived(const juce::MemoryBlock& msg) noexcept override;
   void MidiCmdCallback(rsj::MidiMessage);
   void SendOut();

   bool sending_stopped_{false};
   const Profile& profile_;
   ControlsModel& controls_model_;
   moodycamel::BlockingConcurrentQueue<std::string> command_;
   std::future<void> send_out_future_;
   std::shared_ptr<MidiSender> midi_sender_{nullptr};
   std::vector<std::function<void(bool, bool)>> callbacks_{};

   // helper classes
   class ConnectTimer final : public juce::Timer {
    public:
      explicit ConnectTimer(LrIpcOut& owner) noexcept : owner_(owner) {}
      void Start();
      void Stop();

    private:
      void timerCallback() override;
      LrIpcOut& owner_;
      bool timer_off_{false};
      mutable std::mutex connect_mutex_; // fix race during shutdown
   };
   class Recenter final : public juce::Timer {
    public:
      explicit Recenter(LrIpcOut& owner) noexcept : owner_{owner} {}
      void SetMidiMessage(rsj::MidiMessage mm);

    private:
      void timerCallback() override;
      LrIpcOut& owner_;
      mutable rsj::RelaxTTasSpinLock mtx_;
      rsj::MidiMessage mm_{};
   };
   ConnectTimer connect_timer_{*this};
   Recenter recenter_{*this};
};

#endif // LR_IPC_OUT_H_INCLUDED
