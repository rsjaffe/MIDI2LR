// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
==============================================================================

NrpnMessage.cpp

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
#include "NrpnMessage.h"

bool NRPN_Message::ProcessMidi(short control,
    short value) noexcept(ndebug)
{
    assert(value <= 0x7Fu);
    assert(control <= 0x7Fu);
    auto ret_val = true;
    switch (control) {
    case 6:
    {
        std::lock_guard<decltype(guard)> lock(guard);
        if (ready_ >= 0b11) {
            SetValueMSB_(value);
            if (IsReady_()) {
                nrpn_queued_.emplace(true, GetControl_(), GetValue_());
                Clear_();
            }
        }
        else
            ret_val = false;
        break;
    }
    case 38u:
    {
        std::lock_guard<decltype(guard)> lock(guard);
        if (ready_ >= 0b11) {
            SetValueLSB_(value);
            if (IsReady_()) {
                nrpn_queued_.emplace(true, GetControl_(), GetValue_());
                Clear_();
            }
        }
        else
            ret_val = false;
        break;
    }
    case 98u:
    {
        std::lock_guard<decltype(guard)> lock(guard);
        SetControlLSB_(value);
    }
    break;
    case 99u:
    {
        std::lock_guard<decltype(guard)> lock(guard);
        SetControlMSB_(value);
    }
    break;
    default: //not an expected nrpn control #, handle as typical midi message
        ret_val = false;
    }
    return ret_val;
}

RSJ::NRPN NRPN_Message::GetNRPNifReady() noexcept
{
    std::lock_guard<decltype(guard)> lock(guard);
    if (!nrpn_queued_.empty()) {
        RSJ::NRPN retval{nrpn_queued_.front()};
        nrpn_queued_.pop();
        return retval;
    }
    else
        return RSJ::invalidNRPN;
}

void NRPN_Message::Clear_() noexcept
{
    ready_ = 0;
    control_msb_ = 0;
    control_lsb_ = 0;
    value_msb_ = 0;
    value_lsb_ = 0;
}