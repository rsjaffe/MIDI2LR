#pragma once
/*
==============================================================================

NrpnMessage.h

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

#ifndef MIDI2LR_NRPNMESSAGE_H_INCLUDED
#define MIDI2LR_NRPNMESSAGE_H_INCLUDED

#include <array>
#include <mutex>
#include "MoodyCamel/concurrentqueue.h"
#include <gsl/gsl>
#include "Misc.h"

namespace rsj {
  struct Nrpn {
    bool is_valid{false};
    short control{0};
    short value{0};
    constexpr Nrpn() = default;
    constexpr Nrpn(bool validity, short controlno, short valueval) noexcept
        : is_valid{validity}, control{controlno}, value{valueval}
    {
    }
  };
  static constexpr Nrpn kInvalidNrpn{false, 0, 0};
} // namespace rsj

class NrpnMessage {
  // This is a simplified NRPN message class, and assumes that all NRPN messages
  // have 4 messages, though the NRPN standard allows omission of the 4th
  // message. If the 4th message is dropped, this class silently consumes the
  // message without emitting anything.
public:
  [[nodiscard]] bool IsInProcess() const noexcept;
  bool ProcessMidi(short control, short value) noexcept(kNdebug);
  rsj::Nrpn GetNrpnIfReady() noexcept;

private:
  [[nodiscard]] bool IsReady() const noexcept;
  [[nodiscard]] short GetControl() const noexcept;
  [[nodiscard]] short GetValue() const noexcept;
  void Clear() noexcept;
  void SetControlLsb(short val) noexcept(kNdebug);
  void SetControlMsb(short val) noexcept(kNdebug);
  void SetValueLsb(short val) noexcept(kNdebug);
  void SetValueMsb(short val) noexcept(kNdebug);

  mutable rsj::RelaxTTasSpinLock data_guard_;
  short control_lsb_{0};
  short control_msb_{0};
  short value_lsb_{0};
  short value_msb_{0};
  moodycamel::ConcurrentQueue<rsj::Nrpn> nrpn_queued_{};
  unsigned char ready_{0};
};

class NrpnFilter {
public:
  bool ProcessMidi(short channel, short control, short value) noexcept(kNdebug)
  {
    Expects(channel <= 15 && channel >= 0);
    return nrpn_messages_.at(channel & 0xF).ProcessMidi(control, value);
  }

  bool IsInProcess(short channel) const noexcept(kNdebug)
  {
    Expects(channel <= 15 && channel >= 0);
    return nrpn_messages_.at(channel & 0xF).IsInProcess();
  }

  rsj::Nrpn GetNrpnIfReady(short channel) noexcept(kNdebug)
  {
    Expects(channel <= 15 && channel >= 0);
    return nrpn_messages_.at(channel & 0xF).GetNrpnIfReady();
  }

private:
  std::array<NrpnMessage, 16> nrpn_messages_{};
};

inline bool NrpnMessage::IsInProcess() const noexcept
{
  std::lock_guard<decltype(data_guard_)> lock(data_guard_);
  return ready_ != 0;
}

inline bool NrpnMessage::IsReady() const noexcept
{
  return ready_ == 0b1111;
}

inline short NrpnMessage::GetControl() const noexcept
{
  return (control_msb_ << 7) + control_lsb_;
}

inline short NrpnMessage::GetValue() const noexcept
{
  return (value_msb_ << 7) + value_lsb_;
}

inline void NrpnMessage::SetControlLsb(short val) noexcept(kNdebug)
{
  Expects(val <= 0x7F);
  control_lsb_ = val & 0x7F;
  ready_ |= 0b10;
}

inline void NrpnMessage::SetControlMsb(short val) noexcept(kNdebug)
{
  Expects(val <= 0x7F);
  control_msb_ = val & 0x7F;
  ready_ |= 0b1;
}

inline void NrpnMessage::SetValueLsb(short val) noexcept(kNdebug)
{
  Expects(val <= 0x7F);
  value_lsb_ = val & 0x7F;
  ready_ |= 0b1000;
}

inline void NrpnMessage::SetValueMsb(short val) noexcept(kNdebug)
{
  Expects(val <= 0x7F);
  value_msb_ = val & 0x7F;
  ready_ |= 0b100; //"Magic number" false alarm //-V112
}

#endif