/*
  ==============================================================================

    MIDISender.h
    Created: 22 Aug 2015 9:44:21pm
    Author:  Parth, Jaffe

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe, derived from code
by Parth.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/
#ifndef MIDISENDER_H_INCLUDED
#define MIDISENDER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class MIDISender
{
public:
    static MIDISender& getInstance();

    // sends a CC message to all output devices
    void sendCC(int midi_channel, int controller, int value);

    // re-enumerates MIDI OUT devices
    void rescanDevices();
private:
    MIDISender();
    ~MIDISender();

    MIDISender(MIDISender const&) = delete;
    void operator=(MIDISender const&) = delete;

    OwnedArray<MidiOutput> _outputDevices;

    void initDevices();
};


#endif  // MIDISENDER_H_INCLUDED
