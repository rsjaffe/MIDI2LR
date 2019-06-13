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

NrpnFilter::ProcessResult NrpnFilter::operator()(short channel, short control, short value)
{
   try {
      if (channel < 0 || channel >= kChannels)
         throw std::range_error(
             "Channel value in ProcessMIDI is " + rsj::NumToChars(channel) + '.');
      Expects(value <= 0x7F);
      Expects(control <= 0x7F);
      ProcessResult ret_val{false, false, 0, 0};
      switch (control) {
      case 6:
         if (ready_flags_[channel] >= 0b11) {
            ret_val.is_nrpn = true;
            value_msb_[channel] = value & 0x7F;
            ready_flags_[channel] |= 0b100; //"Magic number" false alarm //-V112
            if (ready_flags_[channel] == 0b1111) {
               ret_val.is_ready = true;
               ret_val.control =
                   gsl::narrow_cast<short>((control_msb_[channel] << 7) + control_lsb_[channel]);
               ret_val.value =
                   gsl::narrow_cast<short>((value_msb_[channel] << 7) + value_lsb_[channel]);
               Clear(channel);
            }
         }
         return ret_val;
      case 38u:
         if (ready_flags_[channel] >= 0b11) {
            ret_val.is_nrpn = true;
            value_lsb_[channel] = value & 0x7F;
            ready_flags_[channel] |= 0b1000;
            if (ready_flags_[channel] == 0b1111) {
               ret_val.is_ready = true;
               ret_val.control =
                   gsl::narrow_cast<short>((control_msb_[channel] << 7) + control_lsb_[channel]);
               ret_val.value =
                   gsl::narrow_cast<short>((value_msb_[channel] << 7) + value_lsb_[channel]);
               Clear(channel);
            }
         }
         return ret_val;
      case 98u:
         ret_val.is_nrpn = true;
         control_lsb_[channel] = value & 0x7F;
         ready_flags_[channel] |= 0b10;
         return ret_val;
      case 99u:
         ret_val.is_nrpn = true;
         control_msb_[channel] = value & 0x7F;
         ready_flags_[channel] |= 0b1;
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
