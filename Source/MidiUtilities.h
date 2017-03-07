#pragma once
/*
==============================================================================

MidiUtilities.h

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

/* NOTE: Channel and Number are zero-based */
#include <functional>
#include "../JuceLibraryCode/JuceHeader.h"
#include "Misc.h"

namespace RSJ {
    constexpr short kNoteOffFlag = 0x8;
    constexpr short kNoteOnFlag = 0x9;
    constexpr short kKeyPressureFlag = 0xA; //Individual Key Pressure
    constexpr short kCCFlag = 0xB;
    constexpr short kPgmChangeFlag = 0xC;
    constexpr short kChanPressureFlag = 0xD; //Max Key Pressure
    constexpr short kPWFlag = 0xE;//Pitch Wheel
    constexpr short kSystemFlag = 0xF;

    struct Message {
        short MessageType{0};
        short Channel{0};
        short Number{0};
        short Value{0};
        constexpr Message() noexcept
        {
        };
        constexpr Message(short mt, short ch, short nu, short va) noexcept:
        MessageType(mt), Channel(ch), Number(nu), Value(va)
        {
        };
        Message(const juce::MidiMessage& mm) noexcept(ndebug);
    };

    enum MessageType: short {
        NOTE, CC, PITCHBEND
    };

    struct MIDI_Message_ID {
        MessageType messageType;
        int channel;
        union {
            int controller;
            int pitch;
            int data;
        };

        MIDI_Message_ID() noexcept:
        messageType(NOTE),
            channel(0),
            data(0)

        {
        }

        MIDI_Message_ID(int ch, int dat, MessageType msgType) noexcept:
        messageType(msgType),
            channel(ch),
            data(dat)
        {
        }

        MIDI_Message_ID(const Message& rhs) noexcept(ndebug);

        bool operator==(const MIDI_Message_ID &other) const noexcept
        {
            return (messageType==other.messageType && channel==other.channel && data==other.data);
        }

        bool operator<(const MIDI_Message_ID& other) const noexcept
        {
            if (channel<other.channel) return true;
            if (channel==other.channel) {
                if (data<other.data) return true;
                if (data==other.data && messageType<other.messageType) return true;
            }
            return false;
        }
    };
}
// hash functions
namespace std {
    template <>
    struct hash<RSJ::MIDI_Message_ID> {
        size_t operator()(const RSJ::MIDI_Message_ID& k) const noexcept
        {
            return hash<int_fast32_t>()((int_fast32_t(k.messageType)<<8)|
                int_fast32_t(k.channel)|(int_fast32_t(k.controller)<<16));
        } //channel is one byte, messagetype is one byte, controller is two bytes
    };
}
