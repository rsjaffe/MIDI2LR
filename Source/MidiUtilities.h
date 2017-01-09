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
#include <cassert>
#include "../JuceLibraryCode/JuceHeader.h"
namespace RSJ {
  constexpr short NoteOffFlag = 0x8;
  constexpr short NoteOnFlag = 0x9;
  constexpr short KeyPressureFlag = 0xA; //Individual Key Pressure
  constexpr short CCflag = 0xB;
  constexpr short PgmChangeFlag = 0xC;
  constexpr short ChanPressureFlag = 0xD; //Max Key Pressure
  constexpr short PWflag = 0xE;//Pitch Wheel
  constexpr short SystemFlag = 0xF;

  struct Message {
    short MessageType;
    short Channel;
    short Number;
    short Value;
    Message():
      MessageType(0), Channel(0), Number(0), Value(0) {};
    Message(short mt, short ch, short nu, short va):
      MessageType(mt), Channel(ch), Number(nu), Value(va) {};
  };

  inline Message ParseMidi(const juce::MidiMessage& mm) noexcept {
    Message mess{0,0,0,0};
    auto raw = mm.getRawData();
    if (raw == nullptr) {
      assert(!"Nullptr returned from getRawData");
      return mess;
    }
    short mt = raw[0] >> 4;
    //don't process system common messages
    if (mt == SystemFlag) return mess;
    mess.MessageType = mt;
    mess.Channel = raw[0] & 0xF;
    switch (mess.MessageType) {
      case PWflag:
        mess.Value = raw[2] << 7 | raw[1];
        mess.Number = 0;
        break;
      case CCflag:
      case KeyPressureFlag:
      case NoteOffFlag:
      case NoteOnFlag:
        mess.Value = raw[2];
        mess.Number = raw[1];
        break;
      case PgmChangeFlag:
        mess.Number = raw[1];
        break;
      case ChanPressureFlag:
        mess.Value = raw[1];
        break;
    }
    return mess;
  }
}
