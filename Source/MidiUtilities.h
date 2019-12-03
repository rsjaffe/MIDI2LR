#ifndef MIDI2LR_MIDIUTILITIES_H_INCLUDED
#define MIDI2LR_MIDIUTILITIES_H_INCLUDED
/*
 * This file is part of MIDI2LR. Copyright (C) 2015 by Rory Jaffe.
 *
 * MIDI2LR is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with MIDI2LR.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

/* Get the declaration of the primary std::hash template. We are not permitted to declare it
 * ourselves. <typeindex> is guaranteed to provide such a declaration, and is much cheaper to
 * include than <functional>. See https://en.cppreference.com/w/cpp/language/extending_std. */
#include <typeindex>
#include <type_traits>

#include "Misc.h"
namespace juce {
   class MidiMessage;
}

namespace rsj {
   enum struct MessageType : uint8_t {
      NoteOff = 0x8,
      NoteOn = 0x9,
      KeyPressure = 0xA, /* Individual key pressure */
      Cc = 0xB,
      PgmChange = 0xC,
      ChanPressure = 0xD, /* max key pressure */
      Pw = 0xE,           /* pitch wheel */
      System = 0xF
   };

   constexpr bool ValidMessageType(uint8_t value)
   {
      static_assert(std::is_unsigned_v<decltype(value)>, "Avoid sign extension");
      const auto from = value >> 4 & 0xF;
      return from >= static_cast<decltype(from)>(MessageType::NoteOff);
   }

   constexpr MessageType ToMessageType(uint8_t value)
   {
      static_assert(std::is_unsigned_v<decltype(value)>, "Avoid sign extension");
      const auto from = value >> 4 & 0xF;
      if (from < static_cast<decltype(from)>(MessageType::NoteOff))
         throw std::out_of_range("ToMessageType: MessageType range error, must be 0x8 to 0xF");
      return static_cast<MessageType>(from);
   }

   inline const char* MessageTypeToName(MessageType from) noexcept
   {
      static std::array translation_table{"Note Off", "Note On", "Key Pressure", "Control Change",
          "Program Change", "Channel Pressure", "Pitch Bend", "System"};
#pragma warning(suppress : 26446 26482)
      return translation_table[static_cast<size_t>(from)
                               - static_cast<size_t>(MessageType::NoteOff)];
   }

   inline const char* MessageTypeToLabel(MessageType from) noexcept
   {
      static std::array translation_table{"NOTE OFF", "NOTE ON", "KEY PRESSURE", "CC",
          "PROGRAM CHANGE", "CHANNEL PRESSURE", "PITCHBEND", "SYSTEM"};
#pragma warning(suppress : 26446 26482)
      return translation_table[static_cast<size_t>(from) - static_cast<size_t>(MessageType::NoteOff)];
   }

   /* channel is 0-based in MidiMessage, 1-based in MidiMessageId */
   struct MidiMessage {
      MessageType message_type_byte{MessageType::NoteOn};
      int channel{0}; /* 0-based */
      int control_number{0};
      int value{0};
      constexpr MidiMessage() noexcept = default;

      constexpr MidiMessage(MessageType mt, int ch, int nu, int va) noexcept
          : message_type_byte(mt), channel(ch), control_number(nu), value(va)
      {
      }

      explicit MidiMessage(const juce::MidiMessage& mm);
   };

   constexpr bool operator==(const rsj::MidiMessage& lhs, const rsj::MidiMessage& rhs) noexcept
   {
      return lhs.message_type_byte == rhs.message_type_byte && lhs.channel == rhs.channel
             && lhs.control_number == rhs.control_number && lhs.value == rhs.value;
   }

   /* channel is 0-based in MidiMessage, 1-based in MidiMessageId */
   struct MidiMessageId {
      MessageType msg_id_type{MessageType::NoteOn};
      int channel{1}; /* 1-based */
      int control_number{0};

      constexpr MidiMessageId() noexcept = default;

      constexpr MidiMessageId(int ch, int dat, MessageType msgType) noexcept
          : msg_id_type(msgType), channel(ch), control_number(dat)
      {
      }

      explicit constexpr MidiMessageId(const MidiMessage& other) noexcept
          : msg_id_type{other.message_type_byte}, channel{other.channel + 1},
            control_number{other.control_number}
      {
      }

      constexpr bool operator==(const MidiMessageId& other) const noexcept
      {
         return msg_id_type == other.msg_id_type && channel == other.channel
                && control_number == other.control_number;
      }

      constexpr bool operator<(const MidiMessageId& other) const noexcept
      {
         if (channel < other.channel)
            return true;
         if (channel == other.channel) {
            if (control_number < other.control_number)
               return true;
            if (control_number == other.control_number && msg_id_type < other.msg_id_type)
               return true;
         }
         return false;
      }
   };
} // namespace rsj

namespace std {
   /*It is allowed to add template specializations for any standard library class template to the
    * namespace std only if the declaration depends on at least one program-defined type and the
    * specialization satisfies all requirements for the original template, except where such
    * specializations are prohibited. */
   template<> struct hash<rsj::MidiMessageId> {
      size_t operator()(rsj::MidiMessageId k) const noexcept
      {
         /* channel is one byte, messagetype is one byte, controller (data) is two bytes */
         return hash<int_fast32_t>()(int_fast32_t(k.channel) | int_fast32_t(k.msg_id_type) << 8
                                     | int_fast32_t(k.control_number) << 16);
      }
   };
} // namespace std

#endif
