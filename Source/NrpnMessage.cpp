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
#include "NrpnMessage.h"

#include <exception>
#include <fmt/format.h>

#include <gsl/gsl>
#include "Misc.h"

NrpnFilter::ProcessResult NrpnFilter::operator()(const rsj::MidiMessage& message)
{
   try {
      if (message.channel < 0 || message.channel >= kChannels)
         throw std::out_of_range(fmt::format("Channel in ProcessMIDI is {}.", message.channel));
      Expects(message.value <= 0x7F && message.value >= 0);
      Expects(message.control_number <= 0x7F && message.control_number >= 0);
#pragma warning(push)
#pragma warning(disable : 26482 26446) /* message.channel range-checked already */
      ProcessResult ret_val {false, false, 0, 0};
      switch (message.control_number) {
      case 6: {
         // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
         auto& i_ref {intermediate_results_[message.channel]};
         auto lock {std::scoped_lock(filter_mutex_)};
         if (i_ref.ready_flags_ >= 0b11) {
            ret_val.is_nrpn = true;
            i_ref.value_msb_ = message.value & 0x7F;
            i_ref.ready_flags_ |= 0b100; //"Magic number" false alarm //-V112
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
         // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
         auto& i_ref {intermediate_results_[message.channel]};
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
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            auto& i_ref {intermediate_results_[message.channel]};
            auto lock {std::scoped_lock(filter_mutex_)};
            i_ref.control_lsb_ = message.value & 0x7F;
            i_ref.ready_flags_ |= 0b10;
         }
         return ret_val;
      case 99:
         ret_val.is_nrpn = true;
         {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            auto& i_ref {intermediate_results_[message.channel]};
            auto lock {std::scoped_lock(filter_mutex_)};
            i_ref.control_msb_ = message.value & 0x7F;
            i_ref.ready_flags_ |= 0b1;
         }
         return ret_val;
      default:
         /* not an expected nrpn control #, handle as typical midi message */
         return ret_val;
      }
#pragma warning(pop)
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), MIDI2LR_FUNC, e);
      throw;
   }
}