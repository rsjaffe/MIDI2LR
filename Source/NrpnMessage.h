#pragma once
/*
==============================================================================

NrpnMessage.h

This file is part of MIDI2LR. Copyright 2015-2017 by Rory Jaffe.

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
#include <mutex>
#include "Misc.h"

class NRPN_Message {
    // This is a simplified NRPN message class, and assumes that all NRPN messages
    // have 4 messages, though the NRPN standard allows omission of the 4th
    // message. If the 4th message is dropped, this class silently consumes the
    // message without emitting anything.
public:
    NRPN_Message() noexcept = default;
    ~NRPN_Message() = default;
    bool ProcessMidi(short control, short value) noexcept(ndebug);
    void Clear() noexcept;

    bool IsInProcess() const noexcept
    {
        std::lock_guard<decltype(guard)> lock(guard);
        return ready_ != 0;
    };

    bool IsReady() const noexcept
    {
        std::lock_guard<decltype(guard)> lock(guard);
        return ready_ == 0b1111;
    };

    short GetValue() const noexcept
    {
        std::lock_guard<decltype(guard)> lock(guard);
        return (value_msb_ << 7) + value_lsb_;
    };

    short GetControl() const noexcept
    {
        std::lock_guard<decltype(guard)> lock(guard);
        return (control_msb_ << 7) + control_lsb_;
    };

private:
    void SetControlLSB_(short val) noexcept(ndebug);
    void SetControlMSB_(short val) noexcept(ndebug);
    void SetValueLSB_(short val) noexcept(ndebug);
    void SetValueMSB_(short val) noexcept(ndebug);
    mutable RSJ::spinlock guard;
    short control_lsb_{0};
    short control_msb_{0};
    short value_lsb_{0};
    short value_msb_{0};
    unsigned char ready_{0};
};

class NRPN_Filter {
public:
    NRPN_Filter() noexcept = default;
    ~NRPN_Filter() = default;
    bool ProcessMidi(short channel, short control, short value) noexcept(ndebug)
    {
        assert(channel <= 15 && channel >= 0);
        return nrpn_messages_[(channel) & 0xF].ProcessMidi(control, value);
    };

    void Clear(short channel) noexcept(ndebug)
    {
        assert(channel <= 15 && channel >= 0);
        return nrpn_messages_[(channel) & 0xF].Clear();
    };

    bool IsInProcess(short channel) const noexcept(ndebug)
    {
        assert(channel <= 15 && channel >= 0);
        return nrpn_messages_[(channel) & 0xF].IsInProcess();
    };

    bool IsReady(short channel) const noexcept(ndebug)
    {
        assert(channel <= 15 && channel >= 0);
        return nrpn_messages_[(channel) & 0xF].IsReady();
    };

    short GetValue(short channel) const
        noexcept(ndebug)
    {
        assert(channel <= 15 && channel >= 0);
        return nrpn_messages_[(channel) & 0xF].GetValue();
    };

    short GetControl(short channel) const
        noexcept(ndebug)
    {
        assert(channel <= 15 && channel >= 0);
        return nrpn_messages_[(channel) & 0xF].GetControl();
    };

private:
    std::array<NRPN_Message, 16> nrpn_messages_{};
};