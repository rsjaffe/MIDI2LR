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
   enum MessageType : short {
      NoteOff = 0x8,
      NoteOn = 0x9,
      KeyPressure = 0xA, // Individual Key Pressure
      Cc = 0xB,
      PgmChange = 0xC,
      ChanPressure = 0xD, // Max Key Pressure
      Pw = 0xE,           // Pitch Wheel
      System = 0xF
   };

   constexpr MessageType ToMessageType(short from)
   {
      if (from < 0x9 || from > 0xf)
         throw std::range_error("MessageType range error, must be 0x9 to 0xf");
      return static_cast<MessageType>(from);
   }

   struct MidiMessage {
      MessageType message_type_byte{NoteOn};
      short channel{0};
      short number{0};
      short value{0};
      constexpr MidiMessage() noexcept = default;
      constexpr MidiMessage(MessageType mt, short ch, short nu, short va) noexcept
          : message_type_byte(mt), channel(ch), number(nu), value(va)
      {
      }
   };

   constexpr bool operator==(const rsj::MidiMessage& lhs, const rsj::MidiMessage& rhs) noexcept
   {
      return lhs.message_type_byte == rhs.message_type_byte && lhs.channel == rhs.channel
             && lhs.number == rhs.number && lhs.value == rhs.value;
   }

   struct MidiMessageId {
      MessageType msg_id_type;
      int channel;
      int data;

      constexpr MidiMessageId() noexcept : msg_id_type(MessageType::NoteOn), channel(0), data(0) {}
      constexpr MidiMessageId(int ch, int dat, MessageType msgType) noexcept
          : msg_id_type(msgType), channel(ch), data(dat)
      {
      }
      constexpr MidiMessageId(const MidiMessage& other)
          : msg_id_type{other.message_type_byte}, channel{other.channel}, data{other.number}
      {
      }
      constexpr MidiMessageId(const MidiMessageId& other) = default;
      constexpr MidiMessageId(MidiMessageId&& other) noexcept = default;
      constexpr MidiMessageId& operator=(const MidiMessageId& other) = default;
      constexpr MidiMessageId& operator=(MidiMessageId&& other) noexcept = default;
      constexpr bool operator==(const MidiMessageId& other) const noexcept
      {
         return msg_id_type == other.msg_id_type && channel == other.channel && data == other.data;
      }
      constexpr bool operator<(const MidiMessageId& other) const noexcept
      {
         return (channel < other.channel) || (channel == other.channel && data < other.data)
                || (channel == other.channel && data == other.data
                       && msg_id_type < other.msg_id_type);
      }
   };

   class MidiMessageFactory { // use this in midireceiver
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
