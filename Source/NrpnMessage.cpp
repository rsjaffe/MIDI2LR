/*
==============================================================================

NrpnMessage.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

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

bool NRPN_Message::ProcessMidi(unsigned short int control, 
    unsigned short int value) noexcept(ndebug) {
  auto ret_val = true;
  switch (control) {
    case 6:
      if (ready_ >= 0b11)
        SetValueMSB(value);
      else
        ret_val = false;
      break;
    case 38:
      if (ready_ >= 0b11)
        SetValueLSB(value);
      else
        ret_val = false;
      break;
    case 98:
      SetControlLSB(value);
      break;
    case 99:
      SetControlMSB(value);
      break;
    default: //not an expected nrpn control #, handle as typical midi message
      ret_val = false;
  }
  return ret_val;
}

void NRPN_Message::Clear() noexcept {
  ready_ = 0;
  control_msb_ = 0;
  control_lsb_ = 0;
  value_msb_ = 0;
  value_lsb_ = 0;
}

void NRPN_Message::SetControlMSB(unsigned short int val) noexcept(ndebug) {
  assert(val <= 0x7F);
  control_msb_ = val & 0x7F;
  ready_ |= 0b1;
}

void NRPN_Message::SetControlLSB(unsigned short int val) noexcept(ndebug) {
  assert(val <= 0x7F);
  control_lsb_ = val & 0x7F;
  ready_ |= 0b10;
}

void NRPN_Message::SetValueMSB(unsigned short int val) noexcept(ndebug) {
  assert(val <= 0x7F);
  value_msb_ = val & 0x7F;
  ready_ |= 0b100;
}

void NRPN_Message::SetValueLSB(unsigned short int val) noexcept(ndebug) {
  assert(val <= 0x7F);
  value_lsb_ = val & 0x7F;
  ready_ |= 0b1000;
}