/*
==============================================================================

NrpnMessage.cpp

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
#include "NrpnMessage.h"

#include <gsl/gsl>
#include "Misc.h"

NrpnFilter::ProcessResult NrpnFilter::operator()(rsj::MidiMessage message)
{
   try {
      if (message.channel < 0 || message.channel >= kChannels)
         throw std::range_error(
             "Channel msg.value in ProcessMIDI is " + rsj::NumToChars(message.channel) + '.');
      Expects(message.value <= 0x7F);
      Expects(message.control_number <= 0x7F);
      ProcessResult ret_val{false, false, 0, 0};
      switch (message.control_number) {
      case 6:
         if (ready_flags_[message.channel] >= 0b11) {
            ret_val.is_nrpn = true;
            value_msb_[message.channel] = message.value & 0x7F;
            ready_flags_[message.channel] |= 0b100; //"Magic number" false alarm //-V112
            if (ready_flags_[message.channel] == 0b1111) {
               ret_val.is_ready = true;
               ret_val.control = gsl::narrow_cast<short>(
                   (control_msb_[message.channel] << 7) + control_lsb_[message.channel]);
               ret_val.value = gsl::narrow_cast<short>(
                   (value_msb_[message.channel] << 7) + value_lsb_[message.channel]);
               Clear(message.channel);
            }
         }
         return ret_val;
      case 38:
         if (ready_flags_[message.channel] >= 0b11) {
            ret_val.is_nrpn = true;
            value_lsb_[message.channel] = message.value & 0x7F;
            ready_flags_[message.channel] |= 0b1000;
            if (ready_flags_[message.channel] == 0b1111) {
               ret_val.is_ready = true;
               ret_val.control = gsl::narrow_cast<short>(
                   (control_msb_[message.channel] << 7) + control_lsb_[message.channel]);
               ret_val.value = gsl::narrow_cast<short>(
                   (value_msb_[message.channel] << 7) + value_lsb_[message.channel]);
               Clear(message.channel);
            }
         }
         return ret_val;
      case 98:
         ret_val.is_nrpn = true;
         control_lsb_[message.channel] = message.value & 0x7F;
         ready_flags_[message.channel] |= 0b10;
         return ret_val;
      case 99:
         ret_val.is_nrpn = true;
         control_msb_[message.channel] = message.value & 0x7F;
         ready_flags_[message.channel] |= 0b1;
         return ret_val;
      default: // not an expected nrpn control #, handle as typical midi message
         return ret_val;
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}