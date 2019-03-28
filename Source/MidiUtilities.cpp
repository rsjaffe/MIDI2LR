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

rsj::MidiMessage::MidiMessage(const juce::MidiMessage& mm) noexcept(kNdebug)
{ // anything not set below is set to zero by default constructor
#pragma warning(push)
#pragma warning(disable : 26481) // doing raw pointer arithmetic, parsing low-level structure
   const auto raw = mm.getRawData();
   Ensures(raw);
   message_type_byte = rsj::ToMessageType(raw[0] >> 4);
   channel = raw[0] & 0xF;
   switch (message_type_byte) {
   case Pw:
      value = (raw[2] << 7) | raw[1];
      break;
   case Cc:
   case KeyPressure:
   case NoteOff:
   case NoteOn:
      value = raw[2];
      number = raw[1];
      break;
   case PgmChange:
      number = raw[1];
      break;
   case ChanPressure:
      value = raw[1];
      break;
   case System:
      break; // no action
   default:
      Ensures(!"Default should be unreachable in ParseMidi");
   }
#pragma warning(pop)
}

rsj::MidiMessageId::MidiMessageId(const MidiMessage& rhs) noexcept(kNdebug)
    : msg_id_type{rhs.message_type_byte}, channel(rhs.channel + 1),
      data(rhs.number) // channel 1-based
{
}

std::optional<rsj::MidiMessage> rsj::MidiMessageFactory::ProcessMidi(
    const juce::MidiMessage& juce_mm)
{
   rsj::MidiMessage m;
   const auto raw = juce_mm.getRawData();
   m.message_type_byte = rsj::ToMessageType(raw[0] >> 4);
   m.channel = raw[0] & 0xF;
   switch (m.message_type_byte) {
   case Pw:
      m.value = (raw[2] << 7) | raw[1];
      return m;
   case Cc:
      m.value = raw[2];
      m.number = raw[1];
      // run through nrpn return rsj::MidiMessage CC number value OR {} if nRPN not ready
      if (nrpn_filter_.ProcessMidi(m.channel, m.number, m.value)) { // true if nrpn
                                                                    // piece
         const auto nrpn = nrpn_filter_.GetNrpnIfReady(m.channel);
         if (nrpn.is_valid) {
            m.number = nrpn.control;
            m.value = nrpn.value;
            return m;
         }
         return {}; // finished with nrpn piece
      }
      else
         return m;
   case NoteOn:
      m.value = raw[2];
      m.number = raw[1];
      return m;
   case KeyPressure:
   case NoteOff:
   case PgmChange:
      m.number = raw[1];
      break;
   case ChanPressure:
      m.value = raw[1];
      break;
   case System:
      break; // no action
   default:
      Ensures(!"Default should be unreachable in ParseMidi");
   }
   return {};
}