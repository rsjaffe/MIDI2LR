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
#include <map> /* map faster than unordered_map for very few members */
#include <memory>
#include <vector>

#include <juce_audio_devices/juce_audio_devices.h>

#include "Concurrency.h"
#include "MidiUtilities.h"

class Devices;

#ifndef _MSC_VER
#define _In_
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
   void AddCallback(_In_ T* const object, _In_ void (T::*const mf)(const rsj::MidiMessage&))
   {
      if (object && mf)
         callbacks_.emplace_back([=](const rsj::MidiMessage& a) { (object->*mf)(a); });
   }

 private:
   void DispatchMessages();
   void handleIncomingMidiMessage(juce::MidiInput*, const juce::MidiMessage&) override;
   void InitDevices();
   void TryToOpen(); /* inner code for InitDevices */

   Devices& devices_;
   rsj::ConcurrentQueue<rsj::MidiMessage> messages_;
   std::future<void> dispatch_messages_future_;
   std::map<juce::MidiInput*, NrpnFilter> filters_ {};
   std::vector<std::function<void(const rsj::MidiMessage&)>> callbacks_;
   std::vector<std::unique_ptr<juce::MidiInput>> input_devices_;
};

#endif
