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

std::optional<rsj::MidiMessage> rsj::MidiMessageFactory::ProcessMidi(
    const juce::MidiMessage& juce_mm)
{
#pragma warning(push)
#pragma warning(disable : 26481) // doing raw pointer arithmetic, parsing low-level structure
   rsj::MidiMessage m;
   const auto raw = juce_mm.getRawData();
   if (!raw)
      return std::nullopt;
   m.message_type_byte = rsj::ToMessageType(raw[0] >> 4);
   m.channel = raw[0] & 0xF;
   switch (m.message_type_byte) {
   case kPw:
      m.value = (raw[2] << 7) | raw[1];
      return m;
   case kCc:
      m.value = raw[2];
      m.control_number = raw[1];
      // run through nrpn return rsj::MidiMessage CC number value OR {} if nRPN not ready
      if (nrpn_filter_.ProcessMidi(m.channel, m.control_number, m.value)) { // true if nrpn
         const auto nrpn = nrpn_filter_.GetNrpnIfReady(m.channel);
         if (nrpn.is_valid) {
            m.control_number = nrpn.control;
            m.value = nrpn.value;
            return m;
         }
         return std::nullopt; // finished with an nrpn piece
      }
      else
         return m;
   case kKeyPressure:
   case kNoteOff:
   case kNoteOn:
      m.value = raw[2];
      m.control_number = raw[1];
      return m;
   case kPgmChange:
      m.control_number = raw[1];
      return m;
   case kChanPressure:
      m.value = raw[1];
      return m;
   case kSystem:
      break; // no action
   default:
      Ensures(!"Default should be unreachable in MidiMessageFactory::ProcessMidi");
   }
   return std::nullopt;
#pragma warning(pop)
}