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
#include <map> //map faster than unordered_map for very few members
#include <memory>
#include <vector>

#include <JuceLibraryCode/JuceHeader.h>
#include "Concurrency.h"
#include "MidiUtilities.h"
#include "NrpnMessage.h"

#ifndef _MSC_VER
#define _In_
#endif

class MidiReceiver final : juce::MidiInputCallback {
 public:
   MidiReceiver() = default;
   ~MidiReceiver() = default;
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
      using namespace std::placeholders;
      if (object && mf)
         /* only store non-empty functions */
         callbacks_.emplace_back(std::bind(mf, object, _1));
   }

 private:
   void DispatchMessages();
   void handleIncomingMidiMessage(juce::MidiInput*, const juce::MidiMessage&) override;
   void InitDevices();
   void TryToOpen(); /* inner code for InitDevices */

   rsj::ConcurrentQueue<rsj::MidiMessage> messages_;
   std::future<void> dispatch_messages_future_;
   std::map<juce::MidiInput*, NrpnFilter> filters_{};
   std::vector<std::function<void(const rsj::MidiMessage&)>> callbacks_;
   std::vector<std::unique_ptr<juce::MidiInput>> devices_;
};

#endif // MIDI2LR_MIDIRECEIVER_H_INCLUDED
