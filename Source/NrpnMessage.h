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
#include "Utilities\mutexpp.h"

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
    static constexpr NRPN invalidNRPN{false, 0, 0};
}

class NRPN_Message {
    // This is a simplified NRPN message class, and assumes that all NRPN messages
    // have 4 messages, though the NRPN standard allows omission of the 4th
    // message. If the 4th message is dropped, this class silently consumes the
    // message without emitting anything.
public:
    NRPN_Message() = default;
    ~NRPN_Message() = default;
    bool IsInProcess() const noexcept;
    bool ProcessMidi(short control, short value) noexcept(ndebug);
    RSJ::NRPN GetNRPNifReady() noexcept;

private:
    bool IsReady_() const noexcept;
    short GetControl_() const noexcept;
    short GetValue_() const noexcept;
    void Clear_() noexcept;
    void SetControlLSB_(short val) noexcept(ndebug);
    void SetControlMSB_(short val) noexcept(ndebug);
    void SetValueLSB_(short val) noexcept(ndebug);
    void SetValueMSB_(short val) noexcept(ndebug);

    mutable mutexpp::adaptive_spin_mutex data_guard_;
    mutable mutexpp::adaptive_spin_mutex queue_guard_;
    short control_lsb_{0};
    short control_msb_{0};
    short value_lsb_{0};
    short value_msb_{0};
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

inline bool NRPN_Message::IsInProcess() const noexcept
{
    std::lock_guard<decltype(data_guard_)> lock(data_guard_);
    return ready_ != 0;
}

inline bool NRPN_Message::IsReady_() const noexcept
{
    return ready_ == 0b1111;
}

inline short NRPN_Message::GetControl_() const noexcept
{
    return (control_msb_ << 7) + control_lsb_;
}

inline short NRPN_Message::GetValue_() const noexcept
{
    return (value_msb_ << 7) + value_lsb_;
}

inline void NRPN_Message::SetControlLSB_(short val) noexcept(ndebug)
{
    assert(val <= 0x7Fu);
    control_lsb_ = val & 0x7Fu;
    ready_ |= 0b10;
}

inline void NRPN_Message::SetControlMSB_(short val) noexcept(ndebug)
{
    assert(val <= 0x7Fu);
    control_msb_ = val & 0x7Fu;
    ready_ |= 0b1;
}

inline void NRPN_Message::SetValueLSB_(short val) noexcept(ndebug)
{
    assert(val <= 0x7Fu);
    value_lsb_ = val & 0x7Fu;
    ready_ |= 0b1000;
}

inline void NRPN_Message::SetValueMSB_(short val) noexcept(ndebug)
{
    assert(val <= 0x7Fu);
    value_msb_ = val & 0x7Fu;
    ready_ |= 0b100;  //"Magic number" false alarm //-V112
}
