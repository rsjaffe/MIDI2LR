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
#include <queue>
#include "Misc.h"

namespace RSJ {
    struct NRPN {
        bool isValid{false};
        short control{0};
        short value{0};
        constexpr NRPN() = default;
        constexpr NRPN(bool validity, short controlno, short valueval) noexcept:
        isValid{validity}, control{controlno}, value{valueval}
        {};
    };
}

class NRPN_Message {
    // This is a simplified NRPN message class, and assumes that all NRPN messages
    // have 4 messages, though the NRPN standard allows omission of the 4th
    // message. If the 4th message is dropped, this class silently consumes the
    // message without emitting anything.
public:
    NRPN_Message() = default;
    ~NRPN_Message() = default;
    bool ProcessMidi(short control, short value) noexcept(ndebug);
    bool IsInProcess() const noexcept
    {
        std::lock_guard<decltype(guard)> lock(guard);
        return ready_ != 0;
    };
    RSJ::NRPN GetNRPNifReady() noexcept
    {
        std::lock_guard<decltype(guard)> lock(guard);
        if (!nrpn_queued_.empty()) {
            RSJ::NRPN retval{nrpn_queued_.front()};
            nrpn_queued_.pop();
            return retval;
        }
        else
            return invalidNRPN;
    };

private:
    void Clear_() noexcept;
    short GetControl_() const noexcept
    {
        return (control_msb_ << 7) + control_lsb_;
    };
    short GetValue_() const noexcept
    {
        return (value_msb_ << 7) + value_lsb_;
    };
    bool IsReady_() const noexcept
    {
        return ready_ == 0b1111;
    };

    void SetControlLSB_(short val) noexcept(ndebug);
    void SetControlMSB_(short val) noexcept(ndebug);
    void SetValueLSB_(short val) noexcept(ndebug);
    void SetValueMSB_(short val) noexcept(ndebug);
    mutable RSJ::spinlock guard;
    short control_lsb_{0};
    short control_msb_{0};
    short value_lsb_{0};
    short value_msb_{0};
    static constexpr RSJ::NRPN invalidNRPN{false, 0, 0};
    std::queue<RSJ::NRPN> nrpn_queued_{};
    unsigned char ready_{0};
};

class NRPN_Filter {
public:
    NRPN_Filter() = default;
    ~NRPN_Filter() = default;
    bool ProcessMidi(short channel, short control, short value) noexcept(ndebug)
    {
        assert(channel <= 15 && channel >= 0);
        return nrpn_messages_[(channel) & 0xF].ProcessMidi(control, value);
    };

    bool IsInProcess(short channel) const noexcept(ndebug)
    {
        assert(channel <= 15 && channel >= 0);
        return nrpn_messages_[(channel) & 0xF].IsInProcess();
    };

    RSJ::NRPN GetNRPNifReady(short channel) noexcept(ndebug)
    {
        assert(channel <= 15 && channel >= 0);
        return nrpn_messages_[(channel) & 0xF].GetNRPNifReady();
    };

private:
    std::array<NRPN_Message, 16> nrpn_messages_{};
};