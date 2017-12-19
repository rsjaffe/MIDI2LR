// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
==============================================================================

MidiUtilities.cpp

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
#include "MidiUtilities.h"
#include <gsl/gsl>

RSJ::MidiMessage::MidiMessage(const juce::MidiMessage& mm) noexcept(ndebug)
{//anything not set below is set to zero by default constructor
    const auto raw = mm.getRawData();
    Expects(raw != nullptr);
    message_type_byte = raw[0] >> 4;
    channel = raw[0] & 0xF;
    switch (message_type_byte) {
        case kPWFlag:
            value = (raw[2] << 7) | raw[1];
            break;
        case kCCFlag:
        case kKeyPressureFlag:
        case kNoteOffFlag:
        case kNoteOnFlag:
            value = raw[2];
            number = raw[1];
            break;
        case kPgmChangeFlag:
            number = raw[1];
            break;
        case kChanPressureFlag:
            value = raw[1];
            break;
        case kSystemFlag:
            break; //no action
        default:
            Expects(!"Default should be unreachable in ParseMidi");
    }
}

RSJ::MidiMessageId::MidiMessageId(const MidiMessage& rhs) noexcept(ndebug):
    channel(rhs.channel + 1), controller(rhs.number) //channel 1-based
{
    switch (rhs.message_type_byte) {//this is needed because mapping uses custom structure
        case kCCFlag:
            msg_id_type = MsgIdEnum::CC;
            break;
        case kNoteOnFlag:
            msg_id_type = MsgIdEnum::NOTE;
            break;
        case kPWFlag:
            msg_id_type = MsgIdEnum::PITCHBEND;
            break;
        default: //should be unreachable--MidiMessageId only handles a few message types
            Expects(0);
    }
}