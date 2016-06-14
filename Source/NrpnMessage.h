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
  NRPN_Message() noexcept {}
  NRPN_Message(unsigned short int cmsb, unsigned short int clsb = 0,
    unsigned short int vmsb = 0, unsigned short int vlsb = 0) noexcept {
    assert(cmsb <= 0x7F && clsb <= 0x7F && vmsb <= 0x7F && vlsb <= 0x7F);
    channel_msb_ = cmsb & 0x7F;
    channel_lsb_ = clsb & 0x7F;
    value_msb_ = vmsb & 0x7F;
    value_lsb_ = vlsb & 0x7F;
  }
  void Clear() noexcept {
    ready_ = 0;
    channel_msb_ = 0;
    channel_lsb_ = 0;
    value_msb_ = 0;
    value_lsb_ = 0;
  }
  inline bool Ready() noexcept {
    return ready_ == 0b1111;
  }
  void SetChannelMSB(unsigned short int val) noexcept {
    assert(val <= 0x7F);
    channel_msb_ = val & 0x7F;
    ready_ |= 0b1;
  }
  void SetChannelLSB(unsigned short int val) noexcept {
    assert(val <= 0x7F);
    channel_lsb_ = val & 0x7F;
    ready_ |= 0b10;
  }
  void SetValueMSB(unsigned short int val) noexcept {
    assert(val <= 0x7F);
    value_msb_ = val & 0x7F;
    ready_ |= 0b100;
  }
  void SetValueLSB(unsigned short int val) noexcept {
    assert(val <= 0x7F);
    value_lsb_ = val & 0x7F;
    ready_ |= 0b1000;
  }
  inline unsigned short int GetValue() noexcept const {
    return value_msb_ << 7 + value_lsb_;
  }
  inline unsigned short int GetChannel() noexcept const {
    return channel_msb_ << 7 + channel_lsb_;
  }
private:
  uchar ready_{0};
  unsigned short int channel_msb_{0};
  unsigned short int channel_lsb_{0};
  unsigned short int value_msb_{0};
  unsigned short int value_lsb_{0};
};