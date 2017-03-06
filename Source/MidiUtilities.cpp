// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
==============================================================================

MidiUtilities.cpp

This file is part of MIDI2LR. Copyright 2015-2017 by Rory Jaffe.

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
#include "MidiUtilities.h"
#include <cassert>

RSJ::Message::Message(const juce::MidiMessage& mm) noexcept(ndebug)
{//anything not set below is set to zero by default constructor
    const auto raw = mm.getRawData();
    if (raw==nullptr) {
        assert(!"Nullptr returned from getRawData");
    }
    MessageType = raw[0]>>4;
    Channel = raw[0]&0xF;
    switch (MessageType) {
    case kPWFlag:
        Value = (raw[2]<<7)|raw[1];
        break;
    case kCCFlag:
    case kKeyPressureFlag:
    case kNoteOffFlag:
    case kNoteOnFlag:
        Value = raw[2];
        Number = raw[1];
        break;
    case kPgmChangeFlag:
        Number = raw[1];
        break;
    case kChanPressureFlag:
        Value = raw[1];
        break;
    case kSystemFlag:
        break; //no action
    default:
        assert(!"Default should be unreachable in ParseMidi");
    }
}

RSJ::MIDI_Message_ID::MIDI_Message_ID(const Message& rhs) noexcept(ndebug):
    channel(rhs.Channel+1), controller(rhs.Number) //channel 1-based
{
    switch (rhs.MessageType) {//this is needed because mapping uses custom structure
    case kCCFlag:
        messageType = CC;
        break;
    case kNoteOnFlag:
        messageType = NOTE;
        break;
    case kPWFlag:
        messageType = PITCHBEND;
        break;
    default: //should be unreachable--MIDI_Message_ID only handles a few message types
        assert(0);
        messageType = CC;
    }
}