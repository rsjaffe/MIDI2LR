#ifndef MIDI2LR_MIDISENDER_H_INCLUDED
#define MIDI2LR_MIDISENDER_H_INCLUDED
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
#include <memory>
#include <vector>

namespace juce {
   class MidiOutput;
}

namespace rsj {
   struct MidiMessageId;
}
//-V813_MINSIZE=13 //warn if passing structure by value > 12 bytes (3*sizeof(int))

// juce MIDI send functions have 1-based channel, so does rsj::MidiMessageId
class MidiSender {
 public:
   void RescanDevices();
   void Send(rsj::MidiMessageId id, int value) const;
   void StartRunning();

 private:
   void InitDevices();

   std::vector<std::unique_ptr<juce::MidiOutput>> output_devices_;
};

#endif // MIDISENDER_H_INCLUDED
