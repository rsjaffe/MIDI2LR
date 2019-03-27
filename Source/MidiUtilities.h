#ifndef MIDI2LR_MIDIUTILITIES_H_INCLUDED
#define MIDI2LR_MIDIUTILITIES_H_INCLUDED
/*
==============================================================================

MidiUtilities.h

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

/* NOTE: Channel and Number are zero-based */
// Get the declaration of the primary std::hash template.
// We are not permitted to declare it ourselves.
// <typeindex> is guaranteed to provide such a declaration,
// and is much cheaper to include than <functional>.
// See https://en.cppreference.com/w/cpp/language/extending_std.
#include <optional>
#include <typeindex>

#include <JuceLibraryCode/JuceHeader.h>
#include "Misc.h"
#include "NrpnMessage.h"

namespace rsj {
   constexpr short kNoteOffFlag = 0x8;
   constexpr short kNoteOnFlag = 0x9;
   constexpr short kKeyPressureFlag = 0xA; // Individual Key Pressure
   constexpr short kCcFlag = 0xB;
   constexpr short kPgmChangeFlag = 0xC;
   constexpr short kChanPressureFlag = 0xD; // Max Key Pressure
   constexpr short kPwFlag = 0xE;           // Pitch Wheel
   constexpr short kSystemFlag = 0xF;

   struct MidiMessage {
      short message_type_byte{0};
      short channel{0};
      short number{0};
      short value{0};
      constexpr MidiMessage() noexcept = default;

      constexpr MidiMessage(short mt, short ch, short nu, short va) noexcept
          : message_type_byte(mt), channel(ch), number(nu), value(va)
      {
      }

      MidiMessage(const juce::MidiMessage& mm) noexcept(kNdebug);
   };

   constexpr bool operator==(const rsj::MidiMessage& lhs, const rsj::MidiMessage& rhs) noexcept
   {
      return lhs.message_type_byte == rhs.message_type_byte && lhs.channel == rhs.channel
             && lhs.number == rhs.number && lhs.value == rhs.value;
   }

   enum class MsgIdEnum : short { kNote, kCc, kPitchBend };

   struct MidiMessageId {
      MsgIdEnum msg_id_type;
      int channel;
      int data;

      constexpr MidiMessageId() noexcept : msg_id_type(rsj::MsgIdEnum::kNote), channel(0), data(0)
      {
      }

      constexpr MidiMessageId(int ch, int dat, MsgIdEnum msgType) noexcept
          : msg_id_type(msgType), channel(ch), data(dat)
      {
      }

      MidiMessageId(const MidiMessage& rhs) noexcept(kNdebug);

      constexpr bool operator==(const MidiMessageId& other) const noexcept
      {
         return msg_id_type == other.msg_id_type && channel == other.channel && data == other.data;
      }

      constexpr bool operator<(const MidiMessageId& other) const noexcept
      {
         if (channel < other.channel)
            return true;
         if (channel == other.channel) {
            if (data < other.data)
               return true;
            if (data == other.data && msg_id_type < other.msg_id_type)
               return true;
         }
         return false;
      }
   };

   class MidiMessageFactory { //use this in midireceiver
    public:
      std::optional<rsj::MidiMessage> ProcessMidi(const juce::MidiMessage& juce_mm);
   private:
      NrpnFilter nrpn_filter_{};
   };
} // namespace rsj
// hash functions
// It is allowed to add template specializations for any standard library class template to the
// namespace std only if the declaration depends on at least one program-defined type and the
// specialization satisfies all requirements for the original template, except where such
// specializations are prohibited.
namespace std {
   template<> struct hash<rsj::MidiMessageId> {
      size_t operator()(const rsj::MidiMessageId& k) const noexcept
      {
         return hash<int_fast32_t>()(int_fast32_t(k.channel) | int_fast32_t(k.msg_id_type) << 8
                                     | int_fast32_t(k.data) << 16);
      } // channel is one byte, messagetype is one byte, controller (data) is two bytes
   };
} // namespace std

#endif
