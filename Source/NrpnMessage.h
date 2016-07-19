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
#include "Utilities/Utilities.h"

class NRPN_Message {
  // This is a simplified NRPN message class, and assumes that all NRPN messages
  // have 4 messages, though the NRPN standard allows ommission of the 4th
  // message. If the 4th message is dropped, this class silently consumes the
  // message without emitting anything.
public:
  NRPN_Message() noexcept {};
  ~NRPN_Message() {};

  bool ProcessMidi(unsigned short int control, unsigned short int value)
    noexcept(ndebug);
  void Clear() noexcept;
  void SetControlLSB(unsigned short int val) noexcept(ndebug);
  void SetControlMSB(unsigned short int val) noexcept(ndebug);
  void SetValueLSB(unsigned short int val) noexcept(ndebug);
  void SetValueMSB(unsigned short int val) noexcept(ndebug);

  inline bool IsInProcess() const noexcept {
    return ready_ != 0u;
  };

  inline bool IsReady() const noexcept {
    return ready_ == 0b1111u;
  };

  inline unsigned short int GetValue() const noexcept {
    return (value_msb_ << 7) + value_lsb_;
  };

  inline unsigned short int GetControl() const noexcept {
    return (control_msb_ << 7) + control_lsb_;
  };

private:
  unsigned char ready_{0u};
  unsigned short int control_msb_{0u};
  unsigned short int control_lsb_{0u};
  unsigned short int value_msb_{0u};
  unsigned short int value_lsb_{0u};
};

class NRPN_Filter {
public:
  NRPN_Filter() noexcept {};
  ~NRPN_Filter() {};
  inline bool ProcessMidi(unsigned short int channel,
    unsigned short int control, unsigned short int value) noexcept(ndebug) {
    assert(channel - 1u < 16u);
    return nrpn_messages_[(channel - 1u) & 0xF].ProcessMidi(control, value);
  };

  inline void Clear(unsigned short int channel) noexcept(ndebug) {
    assert(channel - 1u < 16u);
    return nrpn_messages_[(channel - 1u) & 0xF].Clear();
  };

  inline void SetControlLSB(unsigned short int channel, unsigned short int val)
    noexcept(ndebug) {
    assert(channel - 1u < 16u);
    return nrpn_messages_[(channel - 1u) & 0xF].SetControlLSB(val);
  };

  inline void SetControlMSB(unsigned short int channel, unsigned short int val)
    noexcept(ndebug) {
    assert(channel - 1u < 16u);
    return nrpn_messages_[(channel - 1u) & 0xF].SetControlMSB(val);
  };

  inline void SetValueLSB(unsigned short int channel, unsigned short int val)
    noexcept(ndebug) {
    assert(channel - 1u < 16u);
    return nrpn_messages_[(channel - 1u) & 0xF].SetValueLSB(val);
  };

  inline void SetValueMSB(unsigned short int channel, unsigned short int val)
    noexcept(ndebug) {
    assert(channel - 1u < 16u);
    return nrpn_messages_[(channel - 1u) & 0xF].SetValueMSB(val);
  };

  inline bool IsInProcess(unsigned short int channel) const noexcept(ndebug) {
    assert(channel - 1u < 16u);
    return nrpn_messages_[(channel - 1u) & 0xF].IsInProcess();
  };

  inline bool IsReady(unsigned short int channel) const noexcept(ndebug) {
    assert(channel - 1u < 16u);
    return nrpn_messages_[(channel - 1u) & 0xF].IsReady();
  };

  inline unsigned short int GetValue(unsigned short int channel) const
    noexcept(ndebug) {
    assert(channel - 1u < 16u);
    return nrpn_messages_[(channel - 1u) & 0xF].GetValue();
  };

  inline unsigned short int GetControl(unsigned short int channel) const
    noexcept(ndebug) {
    assert(channel - 1u < 16u);
    return nrpn_messages_[(channel - 1u) & 0xF].GetControl();
  };

private:
  std::array<NRPN_Message, 16> nrpn_messages_;
};