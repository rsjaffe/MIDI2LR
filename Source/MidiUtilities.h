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

    struct MidiMessage {
        short message_type_byte{0};
        short channel{0};
        short number{0};
        short value{0};
        constexpr MidiMessage() noexcept
        {};
        constexpr MidiMessage(short mt, short ch, short nu, short va) noexcept:
        message_type_byte(mt), channel(ch), number(nu), value(va)
        {};
        MidiMessage(const juce::MidiMessage& mm) noexcept(ndebug);
    };

    enum class MsgIdEnum: short {
        NOTE, CC, PITCHBEND
    };

    struct MidiMessageId {
        MsgIdEnum msg_id_type;
        int channel;
        union {
            int controller;
            int pitch;
            int data;
        };

        constexpr MidiMessageId() noexcept:
        msg_id_type(MsgIdEnum::NOTE),
            channel(0),
            data(0)

        {}

        constexpr MidiMessageId(int ch, int dat, MsgIdEnum msgType) noexcept:
        msg_id_type(msgType),
            channel(ch),
            data(dat)
        {}

        MidiMessageId(const MidiMessage& rhs) noexcept(ndebug);

        constexpr bool operator==(const MidiMessageId &other) const noexcept
        {
            return (msg_id_type == other.msg_id_type && channel == other.channel && data == other.data);
        }

        constexpr bool operator<(const MidiMessageId& other) const noexcept
        {
            if (channel < other.channel) return true;
            if (channel == other.channel) {
                if (data < other.data) return true;
                if (data == other.data && msg_id_type < other.msg_id_type) return true;
            }
            return false;
        }
    };
}
// hash functions
namespace std {
    template <>
    struct hash<RSJ::MidiMessageId> {
        size_t operator()(const RSJ::MidiMessageId& k) const noexcept
        {
            return hash<int_fast32_t>()((int_fast32_t(k.msg_id_type) << 8) |
                int_fast32_t(k.channel) | (int_fast32_t(k.controller) << 16));
        } //channel is one byte, messagetype is one byte, controller is two bytes
    };
}
