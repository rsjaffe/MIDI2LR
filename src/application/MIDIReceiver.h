#ifndef MIDI2LR_MIDIRECEIVER_H_INCLUDED
#define MIDI2LR_MIDIRECEIVER_H_INCLUDED
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
#include <functional>
#include <future>
#include <map> /* map faster than unordered_map for very few members https://playfulprogramming.blogspot.com/2017/08/performance-of-flat-maps.html */
#include <memory>
#include <vector>

#include <juce_audio_devices/juce_audio_devices.h>

#include "Concurrency.h"
#include "MidiUtilities.h"

class Devices;

#ifndef _MSC_VER
#define _In_ //-V3547
#endif

class MidiReceiver final : juce::MidiInputCallback {
 public:
   explicit MidiReceiver(Devices& devices) : devices_(devices) {}

   ~MidiReceiver() = default; // NOLINT(modernize-use-override)
   MidiReceiver(const MidiReceiver& other) = delete;
   MidiReceiver(MidiReceiver&& other) = delete;
   MidiReceiver& operator=(const MidiReceiver& other) = delete;
   MidiReceiver& operator=(MidiReceiver&& other) = delete;
   void RescanDevices();
   void Start();
   void Stop();

   template<class T>
   void AddCallback(_In_ T* const object, _In_ void (T::*const mf)(rsj::MidiMessage))
   {
      if (object && mf) { callbacks_.emplace_back(std::bind_front(mf, object)); }
   }

 private:
   void handleIncomingMidiMessage(juce::MidiInput* device,
       const juce::MidiMessage& message) override
   {
      messages_.push({rsj::MidiMessage(message), device});
   }

   void DispatchMessages();
   void DispatchCcMessage(const std::pair<rsj::MidiMessage, juce::MidiInput*>& popped);
   void DispatchNoteOnPwMessage(const std::pair<rsj::MidiMessage, juce::MidiInput*>& popped) const;
   void InitDevices();
   void TryToOpen(); /* inner code for InitDevices */

   Devices& devices_;
   rsj::ConcurrentQueue<std::pair<rsj::MidiMessage, juce::MidiInput*>> messages_;
   std::map<juce::MidiInput*, NrpnFilter> filters_ {};
#ifdef __cpp_lib_copyable_function
   std::vector<std::copyable_function<void(rsj::MidiMessage)>> callbacks_;
#else
   std::vector<std::function<void(rsj::MidiMessage)>> callbacks_;
#endif
   std::vector<std::unique_ptr<juce::MidiInput>> input_devices_;
   std::future<void> dispatch_messages_future_; /* destroy this before callbacks_ */
};

#endif
