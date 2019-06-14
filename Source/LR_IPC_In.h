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
#include <future>
#include <memory>
#include <mutex>
#include <string>

#include "Concurrency.h"
#include <JuceLibraryCode/JuceHeader.h>
class ControlsModel;
class MidiSender;
class Profile;
class ProfileManager;

class LrIpcIn final : juce::Timer, juce::Thread {
 public:
   LrIpcIn(ControlsModel& c_model, ProfileManager& profile_manager, Profile& profile,
       std::shared_ptr<MidiSender> midi_sender);
   ~LrIpcIn();
   LrIpcIn(const LrIpcIn& other) = delete;
   LrIpcIn(LrIpcIn&& other) = delete;
   LrIpcIn& operator=(const LrIpcIn& other) = delete;
   LrIpcIn& operator=(LrIpcIn&& other) = delete;
   void Start();
   // signal exit to thread
   void PleaseStopThread();

 private:
   juce::StreamingSocket socket_{};
   // Thread interface
   void run() override;
   // Timer callback
   void timerCallback() override;
   // process a line received from the socket
   void ProcessLine();
   rsj::BlockingQueue<std::string> line_;
   std::future<void> process_line_future_;

   bool thread_started_{false};
   bool timer_off_{false};
   Profile& profile_;
   ControlsModel& controls_model_; //
   mutable std::mutex timer_mutex_;
   ProfileManager& profile_manager_;
   std::shared_ptr<MidiSender> midi_sender_{nullptr};
};

#endif // LR_IPC_IN_H_INCLUDED
