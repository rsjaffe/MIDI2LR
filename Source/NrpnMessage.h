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
#include <array>
#include <cassert>

class NRPN_Message {
public:
  NRPN_Message() noexcept {};
  ~NRPN_Message() {};

  bool ProcessMidi(unsigned short int control, unsigned short int value) noexcept;
  void Clear() noexcept;
  void SetControlLSB(unsigned short int val) noexcept;
  void SetControlMSB(unsigned short int val) noexcept;
  void SetValueLSB(unsigned short int val) noexcept;
  void SetValueMSB(unsigned short int val) noexcept;

  const inline bool IsInProcess() noexcept {
    return in_process_;
  };

  const inline bool IsReady() noexcept {
    return ready_ == 0b1111;
  };

  const inline unsigned short int GetValue() noexcept {
    return value_msb_ << 7 + value_lsb_;
  };

  const inline unsigned short int GetControl() noexcept {
    return control_msb_ << 7 + control_lsb_;
  };

private:
  unsigned char ready_{0};
  bool in_process_{false};
  unsigned short int control_msb_{0};
  unsigned short int control_lsb_{0};
  unsigned short int value_msb_{0};
  unsigned short int value_lsb_{0};
};

class NRPN_Filter {
public:
  NRPN_Filter() noexcept {};
  ~NRPN_Filter() {};
  inline bool ProcessMidi(unsigned short int channel, unsigned short int control,
    unsigned short int value) noexcept {
    assert(channel < 16);
    return nrpn_messages_[channel - 1].ProcessMidi(control, value);
  };
  inline void Clear(unsigned short int channel) noexcept {
    assert(channel < 16);
    return nrpn_messages_[channel - 1].Clear();
  };
  inline void SetControlLSB(unsigned short int channel, unsigned short int val)
    noexcept {
    assert(channel < 16);
    return nrpn_messages_[channel - 1].SetControlLSB(val);
  };
  inline void SetControlMSB(unsigned short int channel, unsigned short int val)
    noexcept {
    assert(channel < 16);
    return nrpn_messages_[channel - 1].SetControlMSB(val);
  };
  inline void SetValueLSB(unsigned short int channel, unsigned short int val) 
    noexcept {
    assert(channel < 16);
    return nrpn_messages_[channel - 1].SetValueLSB(val);
  };
  inline void SetValueMSB(unsigned short int channel, unsigned short int val) 
    noexcept {
    assert(channel < 16);
    return nrpn_messages_[channel - 1].SetValueMSB(val);
  };
  const inline bool IsInProcess(unsigned short int channel) noexcept {
    assert(channel < 16);
    return nrpn_messages_[channel - 1].IsInProcess();
  };

  const inline bool IsReady(unsigned short int channel) noexcept {
    assert(channel < 16);
    return nrpn_messages_[channel - 1].IsReady();
  };

  const inline unsigned short int GetValue(unsigned short int channel) 
    noexcept {
    assert(channel < 16);
    return nrpn_messages_[channel - 1].GetValue();
  };

  const inline unsigned short int GetControl(unsigned short int channel) 
    noexcept {
    assert(channel < 16);
    return nrpn_messages_[channel - 1].GetControl();
  };
private:
  std::array<NRPN_Message, 16> nrpn_messages_;
};