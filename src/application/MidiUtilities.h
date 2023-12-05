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
#include <array>
#include <stdexcept>
#include <type_traits>
#include <typeindex> /*declaration of std::hash template*/
#include <utility>
#include <version>

#include <fmt/format.h>

namespace juce {
   class MidiMessage;
} // namespace juce

#ifdef __cpp_lib_three_way_comparison
#include <compare>
#endif

/*****************************************************************************/
/*************MessageType*****************************************************/
/*****************************************************************************/
namespace rsj {
   enum struct MessageType : uint8_t {
      kNoteOff = 0x8,
      kNoteOn = 0x9,
      kKeyPressure = 0xA, /* Individual key pressure */
      kCc = 0xB,
      kPgmChange = 0xC,
      kChanPressure = 0xD, /* max key pressure */
      kPw = 0xE,           /* pitch wheel */
      kSystem = 0xF
   };

   constexpr bool ValidMessageType(std::underlying_type_t<MessageType> value) noexcept
   {
      static_assert(std::is_unsigned_v<decltype(value)>, "Avoid sign extension");
      const auto from {value >> 4U & 0xFU};
      return std::cmp_greater_equal(from,
          static_cast<std::underlying_type_t<MessageType>>(MessageType::kNoteOff));
   }

   constexpr MessageType ToMessageType(std::underlying_type_t<MessageType> value)
   {
      static_assert(std::is_unsigned_v<decltype(value)>, "Avoid sign extension");
      const auto from {value >> 4U & 0xFU};
      if (std::cmp_less(from,
              static_cast<std::underlying_type_t<MessageType>>(MessageType::kNoteOff))) {
         throw std::out_of_range("ToMessageType: MessageType range error, must be 0x8 to 0xF");
      }
      return static_cast<MessageType>(from);
   }

   inline const char* MessageTypeToName(MessageType from) noexcept
   {
      static const std::array translation_table {"Note Off", "Note On", "Key Pressure",
          "Control Change", "Program Change", "Channel Pressure", "Pitch Bend", "System"};
#pragma warning(suppress : 26446 26482)
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      return translation_table[static_cast<size_t>(from)
                               - static_cast<size_t>(MessageType::kNoteOff)];
   }

   inline const char* MessageTypeToLabel(MessageType from) noexcept
   {
      static const std::array kTranslationTable {"NOTE OFF", "NOTE ON", "KEY PRESSURE", "CC",
          "PROGRAM CHANGE", "CHANNEL PRESSURE", "PITCHBEND", "SYSTEM"};
#pragma warning(suppress : 26446 26482)
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      return kTranslationTable[static_cast<size_t>(from)
                               - static_cast<size_t>(MessageType::kNoteOff)];
   }
} // namespace rsj

template<typename Char> struct fmt::formatter<rsj::MessageType, Char> {
   template<typename ParseContext> constexpr auto parse(ParseContext& ctx)
   { /* parsing copied from fmt's chrono.h */
      auto it {ctx.begin()};
      if (!it) { return ctx.end(); }
      if (it != ctx.end() && *it == ':') { std::advance(it, 1); }
      auto end {std::find(it, ctx.end(), '}')};
      tm_format_.reserve(detail::to_unsigned(end - it + 1));
      tm_format_.append(it, end);
      tm_format_.push_back('\0');
      return end;
   }

   template<typename FormatContext> auto format(const rsj::MessageType& p, FormatContext& ctx) const
   {
      if (tm_format_[0] == 'n') {
         return fmt::format_to(ctx.out(), "{}", rsj::MessageTypeToName(p));
      }
      return fmt::format_to(ctx.out(), "{}", rsj::MessageTypeToLabel(p));
   }

 private:
   basic_memory_buffer<Char> tm_format_;
}; // namespace fmt

/*****************************************************************************/
/*************MidiMessage*****************************************************/
/*****************************************************************************/
namespace rsj {
   /* channel is 0-based in MidiMessage, 1-based in MidiMessageId */
   struct MidiMessage {
      MessageType message_type_byte {MessageType::kNoteOn};
      int channel {0}; /* 0-based */
      int control_number {0};
      int value {0};
      constexpr MidiMessage() noexcept = default;

      constexpr MidiMessage(MessageType mt, int ch, int nu, int va) noexcept
          : message_type_byte(mt), channel(ch), control_number(nu), value(va)
      {
      }

      explicit MidiMessage(const juce::MidiMessage& mm);
   };

   constexpr bool operator==(rsj::MidiMessage lhs, rsj::MidiMessage rhs) noexcept
   {
      return lhs.message_type_byte == rhs.message_type_byte && lhs.channel == rhs.channel
             && lhs.control_number == rhs.control_number && lhs.value == rhs.value;
   }

   /* channel is 0-based in MidiMessage, 1-based in MidiMessageId */
   struct MidiMessageId {
      int channel {1}; /* 1-based */
      int control_number {0};
      MessageType msg_id_type {MessageType::kNoteOn};

      constexpr MidiMessageId() noexcept = default;

      constexpr MidiMessageId(int ch, int dat, MessageType msgType) noexcept
          : channel(ch), control_number(dat), msg_id_type(msgType)
      {
      }

      explicit constexpr MidiMessageId(const MidiMessage& other) noexcept
          : channel {other.channel + 1}, control_number {other.control_number},
            msg_id_type {other.message_type_byte}
      {
      }
#ifdef __cpp_lib_three_way_comparison
      [[nodiscard]] constexpr std::strong_ordering operator<=>(
          const MidiMessageId& other) const noexcept = default;
#else
      [[nodiscard]] constexpr bool operator==(const MidiMessageId& other) const noexcept
      {
         return msg_id_type == other.msg_id_type && channel == other.channel
                && control_number == other.control_number;
      }

      [[nodiscard]] constexpr bool operator<(const MidiMessageId& other) const noexcept
      {
         if (channel < other.channel) { return true; }
         if (channel == other.channel) {
            if (control_number < other.control_number) { return true; }
            if (control_number == other.control_number && msg_id_type < other.msg_id_type) {
               return true;
            }
         }
         return false;
      }
#endif
   };
} // namespace rsj

/*It is allowed to add template specializations for any standard library class template to the
 * namespace std only if the declaration depends on at least one program-defined type and the
 * specialization satisfies all requirements for the original template, except where such
 * specializations are prohibited. */
template<> struct std::hash<rsj::MidiMessageId> {
   size_t operator()(rsj::MidiMessageId k) const noexcept
   {
      /* channel is one byte, messagetype is one byte, controller (data) is two bytes */
      return hash<int_fast32_t>()(static_cast<int_fast32_t>(k.channel)
                                  | static_cast<int_fast32_t>(k.msg_id_type) << 8
                                  | static_cast<int_fast32_t>(k.control_number) << 16);
   }
};

/*****************************************************************************/
/*************NrpnFilter******************************************************/
/*****************************************************************************/
class NrpnFilter {
   /* This  assumes that all NRPN messages have 4 messages, though the NRPN standard allows omission
    * of the 4th message. If the 4th message is dropped, this class silently consumes the message
    * without emitting anything. */

 public:
   struct ProcessResult {
      bool is_nrpn {};
      bool is_ready {};
      int control {};
      int value {};
   };

   ProcessResult operator()(rsj::MidiMessage message);

 private:
   void Clear(int channel) noexcept
   {
#pragma warning(suppress : 26446 26482) /* Channel bounds-checked in calling functions */
      // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
      intermediate_results_[channel] = {0, 0, 0, 0, 0};
   }

   struct InternalStructure {
      int control_lsb_ {0};
      int control_msb_ {0};
      int ready_flags_ {0};
      int value_lsb_ {0};
      int value_msb_ {0};
   };

   static constexpr int kChannels {16};
   std::array<InternalStructure, kChannels> intermediate_results_ {};
};

#endif
