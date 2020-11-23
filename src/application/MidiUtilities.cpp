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
#include "MidiUtilities.h"

#include <exception>

#include <fmt/format.h>
#include <gsl/gsl>

#include <juce_audio_basics/juce_audio_basics.h>

#include "Misc.h"
/*****************************************************************************/
/*************MidiMessage*****************************************************/
/*****************************************************************************/
rsj::MidiMessage::MidiMessage(const juce::MidiMessage& mm)
{
   /* anything not set below is set to zero by default constructor */
#pragma warning(push)
#pragma warning(disable : 26481) /* doing raw pointer arithmetic, parsing low-level structure */
   // ReSharper disable CppClangTidyCppcoreguidelinesProBoundsPointerArithmetic
   const auto raw {mm.getRawData()};
   Ensures(raw);
   if (rsj::ValidMessageType(raw[0])) {
      message_type_byte = rsj::ToMessageType(raw[0]);
      channel = raw[0] & 0xF;
      switch (message_type_byte) {
      case MessageType::Pw:
         value = raw[2] << 7 | raw[1];
         break;
      case MessageType::Cc:
      case MessageType::KeyPressure:
      case MessageType::NoteOff:
      case MessageType::NoteOn:
         value = raw[2];
         control_number = raw[1];
         break;
      case MessageType::PgmChange:
         control_number = raw[1];
         break;
      case MessageType::ChanPressure:
         value = raw[1];
         break;
      case MessageType::System:
         break; /* no action */
      }
   }
   else
      message_type_byte = MessageType::System;
#pragma warning(pop)
   // ReSharper restore CppClangTidyCppcoreguidelinesProBoundsPointerArithmetic
}
/*****************************************************************************/
/*************NrpnFilter******************************************************/
/*****************************************************************************/
NrpnFilter::ProcessResult NrpnFilter::operator()(const rsj::MidiMessage& message)
{
   try {
      Expects(message.value <= 0x7F && message.value >= 0);
      Expects(message.control_number <= 0x7F && message.control_number >= 0);
      ProcessResult ret_val {false, false, 0, 0};
      switch (message.control_number) {
      case 6: {
         auto& i_ref {intermediate_results_.at(message.channel)};
         auto lock {std::scoped_lock(filter_mutex_)};
         if (i_ref.ready_flags_ >= 0b11) {
            ret_val.is_nrpn = true;
            i_ref.value_msb_ = message.value & 0x7F;
            i_ref.ready_flags_ |= 0b100; //-V112
            if (i_ref.ready_flags_ == 0b1111) {
               ret_val.is_ready = true;
               ret_val.control = (i_ref.control_msb_ << 7) + i_ref.control_lsb_;
               ret_val.value = (i_ref.value_msb_ << 7) + i_ref.value_lsb_;
               Clear(message.channel);
            }
         }
      }
         return ret_val;
      case 38: {
         auto& i_ref {intermediate_results_.at(message.channel)};
         auto lock {std::scoped_lock(filter_mutex_)};
         if (i_ref.ready_flags_ >= 0b11) {
            ret_val.is_nrpn = true;
            i_ref.value_lsb_ = message.value & 0x7F;
            i_ref.ready_flags_ |= 0b1000;
            if (i_ref.ready_flags_ == 0b1111) {
               ret_val.is_ready = true;
               ret_val.control = (i_ref.control_msb_ << 7) + i_ref.control_lsb_;
               ret_val.value = (i_ref.value_msb_ << 7) + i_ref.value_lsb_;
               Clear(message.channel);
            }
         }
      }
         return ret_val;
      case 98:
         ret_val.is_nrpn = true;
         {
            auto& i_ref {intermediate_results_.at(message.channel)};
            auto lock {std::scoped_lock(filter_mutex_)};
            i_ref.control_lsb_ = message.value & 0x7F;
            i_ref.ready_flags_ |= 0b10;
         }
         return ret_val;
      case 99:
         ret_val.is_nrpn = true;
         {
            auto& i_ref {intermediate_results_.at(message.channel)};
            auto lock {std::scoped_lock(filter_mutex_)};
            i_ref.control_msb_ = message.value & 0x7F;
            i_ref.ready_flags_ |= 0b1;
         }
         return ret_val;
      default:
         /* not an expected nrpn control #, handle as typical midi message */
         return ret_val;
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}