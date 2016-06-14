#pragma once
/*
==============================================================================

NrpnMessage.h

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
class NRPN_Message {
public:
  NRPN_Message() noexcept {};
  ~NRPN_Message() {};
  void Clear() noexcept {
    ready_ = 0;
    control_msb_ = 0;
    control_lsb_ = 0;
    value_msb_ = 0;
    value_lsb_ = 0;
    in_process_ = false;
  };
  inline bool Ready() noexcept {
    return ready_ == 0b1111;
  };
  void SetControlMSB(unsigned short int val) noexcept {
    assert(val <= 0x7F);
    control_msb_ = val & 0x7F;
    ready_ |= 0b1;
    in_process_ = true;
  };
  void SetControlLSB(unsigned short int val) noexcept {
    assert(val <= 0x7F);
    control_lsb_ = val & 0x7F;
    ready_ |= 0b10;
    in_process_ = true;
  };
  void SetValueMSB(unsigned short int val) noexcept {
    assert(val <= 0x7F);
    value_msb_ = val & 0x7F;
    ready_ |= 0b100;
    in_process_ = true;
  };
  void SetValueLSB(unsigned short int val) noexcept {
     assert(val <= 0x7F);
    value_lsb_ = val & 0x7F;
    ready_ |= 0b1000;
    in_process_ = true;
  };
  inline unsigned short int GetValue() noexcept const {
    return value_msb_ << 7 + value_lsb_;
  };
  inline unsigned short int GetControl() noexcept const {
    return control_msb_ << 7 + control_lsb_;
  };
  inline bool InProcess() noexcept const {
    return in_process_;
  };
private:
  unsigned char ready_{0};
  bool in_process_{false};
  unsigned short int control_msb_{0};
  unsigned short int control_lsb_{0};
  unsigned short int value_msb_{0};
  unsigned short int value_lsb_{0};
};