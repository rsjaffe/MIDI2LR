#pragma once
/*
  ==============================================================================

    MIDISender.h

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
#ifndef MIDI2LR_MIDISENDER_H_INCLUDED
#define MIDI2LR_MIDISENDER_H_INCLUDED
#include <memory>
#include <vector>
namespace juce {
   class MidiOutput;
}

class MidiSender {
 public:
   void Init();

   // sends a CC message to all output devices
   void SendCc(int midi_channel, int controller, int value) const;
   // sends a PitchBend message to all output devices
   void SendPitchWheel(int midi_channel, int value) const;

   void SendNoteOn(int midi_channel, int controller, int value) const;

   // re-enumerates MIDI OUT devices
   void RescanDevices();

 private:
   void InitDevices();
   std::vector<std::unique_ptr<juce::MidiOutput>> output_devices_;
};

#endif // MIDISENDER_H_INCLUDED
