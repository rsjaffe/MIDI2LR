// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
==============================================================================

ControlsModel.cpp

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
#include "ControlsModel.h"
#include "MidiUtilities.h"

double ChannelModel::OffsetResult_(short diff, size_t controlnumber) noexcept(ndebug)
{
    Expects(ccHigh_.at(controlnumber) > 0); //CCLow will always be 0 for offset controls
    Expects(diff <= kMaxNRPN && diff >= -kMaxNRPN);
    Expects(controlnumber <= kMaxNRPN);
    lastUpdate_.store(RSJ::now_ms(), std::memory_order_release);
    short cv = currentV_[controlnumber].fetch_add(diff, std::memory_order_relaxed) + diff;
    if (cv < 0) {//fix currentV unless another thread has already altered it
        currentV_[controlnumber].compare_exchange_strong(cv, static_cast<short>(0),
            std::memory_order_relaxed, std::memory_order_relaxed);
        return 0.0;
    }
    if (cv > ccHigh_[controlnumber]) {//fix currentV unless another thread has already altered it
        currentV_[controlnumber].compare_exchange_strong(cv, ccHigh_[controlnumber],
            std::memory_order_relaxed, std::memory_order_relaxed);
        return 1.0;
    }
    return static_cast<double>(cv) / static_cast<double>(ccHigh_[controlnumber]);
}

double ChannelModel::ControllerToPlugin(short controltype, size_t controlnumber, short value) noexcept(ndebug)
{
    Expects((controltype == RSJ::kCCFlag && ccMethod_.at(controlnumber) == RSJ::CCmethod::absolute) ? (ccLow_.at(controlnumber) < ccHigh_.at(controlnumber)) : 1);
    Expects((controltype == RSJ::kPWFlag) ? (pitchWheelMax_ > pitchWheelMin_) : 1);
    Expects((controltype == RSJ::kPWFlag) ? value >= pitchWheelMin_ && value <= pitchWheelMax_ : 1);
    //note that the value is not msb,lsb, but rather the calculated value. Since lsb is only 7 bits, high bits are shifted one right when placed into short.
    switch (controltype) {
        case RSJ::kPWFlag:
            pitchWheelCurrent_.store(value, std::memory_order_release);
            return static_cast<double>(value - pitchWheelMin_) / static_cast<double>(pitchWheelMax_ - pitchWheelMin_);
        case RSJ::kCCFlag:
            switch (ccMethod_[controlnumber]) {
                case RSJ::CCmethod::absolute:
                    currentV_[controlnumber].store(value, std::memory_order_release);
                    return static_cast<double>(value - ccLow_[controlnumber]) / static_cast<double>(ccHigh_[controlnumber] - ccLow_[controlnumber]);
                case RSJ::CCmethod::binaryoffset:
                    if (IsNRPN_(controlnumber))
                        return OffsetResult_(value - kBit14, controlnumber);
                    return OffsetResult_(value - kBit7, controlnumber);
                case RSJ::CCmethod::signmagnitude:
                    if (IsNRPN_(controlnumber))
                        return OffsetResult_((value & kBit14) ? -(value & kLow13Bits) : value, controlnumber);
                    return OffsetResult_((value & kBit7) ? -(value & kLow6Bits) : value, controlnumber);
                case RSJ::CCmethod::twoscomplement: //see https://en.wikipedia.org/wiki/Signed_number_representations#Two.27s_complement
                    if (IsNRPN_(controlnumber)) //flip twos comp and subtract--independent of processor architecture
                        return OffsetResult_((value & kBit14) ? -((value ^ kMaxNRPN) + 1) : value, controlnumber);
                    return OffsetResult_((value & kBit7) ? -((value ^ kMaxMIDI) + 1) : value, controlnumber);
                default:
                    Expects(!"Should be unreachable code in ControllerToPlugin--unknown CCmethod");
                    return 0.0;
            }
        case RSJ::kNoteOnFlag:
            return static_cast<double>(value) / static_cast<double>((IsNRPN_(controlnumber) ? kMaxNRPN : kMaxMIDI));
        case RSJ::kNoteOffFlag:
            return 0.0;
        default:
            Expects(!"Should be unreachable code in ControllerToPlugin--unknown control type");
            return 0.0;
    }
}

void ChannelModel::SetToCenter(short controltype, size_t controlnumber) noexcept
{
    switch (controltype) {
        case RSJ::kPWFlag:
            pitchWheelCurrent_.store((pitchWheelMax_ - pitchWheelMin_) / 2 + pitchWheelMin_, std::memory_order_release);
        case RSJ::kCCFlag:
            if (ccMethod_[controlnumber] == RSJ::CCmethod::absolute)
                currentV_[controlnumber].store((ccHigh_[controlnumber] - ccLow_[controlnumber]) / 2 + ccLow_[controlnumber], std::memory_order_release);
        default:
            /* */;
    }
}

std::pair<short, short> ChannelModel::MeasureChange(short controltype, size_t controlnumber, short value, bool recenter) noexcept(ndebug)
{
    Expects((controltype == RSJ::kCCFlag && ccMethod_.at(controlnumber) == RSJ::CCmethod::absolute) ? (ccLow_.at(controlnumber) < ccHigh_.at(controlnumber)) : 1);
    Expects((controltype == RSJ::kPWFlag) ? (pitchWheelMax_ > pitchWheelMin_) : 1);
    Expects((controltype == RSJ::kPWFlag) ? value >= pitchWheelMin_ && value <= pitchWheelMax_ : 1);
    //note that the value is not msb,lsb, but rather the calculated value. Since lsb is only 7 bits, high bits are shifted one right when placed into short.
    switch (controltype) {
        case RSJ::kPWFlag:
        {
            short newvalue = recenter ? (pitchWheelMax_ - pitchWheelMin_) / 2 + pitchWheelMin_ : value;
            return {value - pitchWheelCurrent_.exchange(newvalue), newvalue};
        }
        case RSJ::kCCFlag:
            switch (ccMethod_[controlnumber]) {
                case RSJ::CCmethod::absolute:
                {
                    short newvalue = recenter ? (ccHigh_[controlnumber] - ccLow_[controlnumber]) / 2 + ccLow_[controlnumber] : value;
                    return {value - currentV_[controlnumber].exchange(newvalue), newvalue};
                }
                case RSJ::CCmethod::binaryoffset:
                    if (IsNRPN_(controlnumber))
                        return {value - kBit14, 0};
                    return {value - kBit7, 0};
                case RSJ::CCmethod::signmagnitude:
                    if (IsNRPN_(controlnumber))
                        return {(value & kBit14) ? -(value & kLow13Bits) : value, 0};
                    return {(value & kBit7) ? -(value & kLow6Bits) : value, 0};
                case RSJ::CCmethod::twoscomplement: //see https://en.wikipedia.org/wiki/Signed_number_representations#Two.27s_complement
                    if (IsNRPN_(controlnumber)) //flip twos comp and subtract--independent of processor architecture
                        return {(value & kBit14) ? -((value ^ kMaxNRPN) + 1) : value, 0};
                    return {(value & kBit7) ? -((value ^ kMaxMIDI) + 1) : value, 0};
                default:
                    Expects(!"Should be unreachable code in ControllerToPlugin--unknown CCmethod");
                    return {0, 0};
            }
        case RSJ::kNoteOnFlag:
            return {0, 0};
        case RSJ::kNoteOffFlag:
            return {0, 0};
        default:
            Expects(!"Should be unreachable code in ControllerToPlugin--unknown control type");
            return {0, 0};
    }
}

short ChannelModel::PluginToController(short controltype, size_t controlnumber, double pluginV) noexcept(ndebug)
{
    Expects(controlnumber <= kMaxNRPN);
    Expects(pluginV >= 0.0 && pluginV <= 1.0);
    switch (controltype) {
        case RSJ::kPWFlag:
        {
            const short newv = static_cast<short>(round(pluginV * (pitchWheelMax_ - pitchWheelMin_))) + pitchWheelMin_;
            pitchWheelCurrent_.store(newv, std::memory_order_release);
            return newv;
        }
        case RSJ::kCCFlag:
        {
            const short newv = static_cast<short>(round(pluginV *
                (ccHigh_[controlnumber] - ccLow_[controlnumber]))) + ccLow_[controlnumber];
            currentV_[controlnumber].store(newv, std::memory_order_release);
            return newv;
        }
        case RSJ::kNoteOnFlag:
            return kMaxMIDI;
        default:
            Expects(!"Unexpected control type");
    }
    return 0;
}

void ChannelModel::setCC(size_t controlnumber, short min, short max, RSJ::CCmethod controltype) noexcept(ndebug)
{
    setCCmethod(controlnumber, controltype); //has to be set before others or ranges won't be correct
    setCCmin(controlnumber, min);
    setCCmax(controlnumber, max);
}

void ChannelModel::setCCall(size_t controlnumber, short min, short max, RSJ::CCmethod controltype) noexcept(ndebug)
{
    if (IsNRPN_(controlnumber))
        for (short a = kMaxMIDI + 1; a <= kMaxNRPN; ++a)
            setCC(a, min, max, controltype);
    else
        for (short a = 0; a <= kMaxMIDI; ++a)
            setCC(a, min, max, controltype);
}

void ChannelModel::setCCmax(size_t controlnumber, short value) noexcept(ndebug)
{
    Expects(controlnumber <= kMaxNRPN);
    Expects(value <= kMaxNRPN);
    Expects(value >= 0);
    if (ccMethod_[controlnumber] != RSJ::CCmethod::absolute)
        ccHigh_[controlnumber] = (value < 0) ? 1000 : value;
    else {
        const auto max = (IsNRPN_(controlnumber) ? kMaxNRPN : kMaxMIDI);
        ccHigh_[controlnumber] = (value <= ccLow_[controlnumber] || value > max) ? max : value;
    }
    currentV_[controlnumber].store((ccHigh_[controlnumber] - ccLow_[controlnumber]) / 2, std::memory_order_release);
}

void ChannelModel::setCCmin(size_t controlnumber, short value) noexcept(ndebug)
{
    Expects(controlnumber <= kMaxNRPN);
    Expects(value <= kMaxNRPN);
    Expects(value >= 0);
    if (ccMethod_[controlnumber] != RSJ::CCmethod::absolute)
        ccLow_[controlnumber] = 0;
    else
        ccLow_[controlnumber] = (value < 0 || value >= ccHigh_[controlnumber]) ? 0 : value;
    currentV_[controlnumber].store((ccHigh_[controlnumber] - ccLow_[controlnumber]) / 2, std::memory_order_release);
}

void ChannelModel::setPWmax(short value) noexcept(ndebug)
{
    Expects(value <= kMaxNRPN);
    Expects(value >= 0);
    if (value > kMaxNRPN || value <= pitchWheelMin_)
        pitchWheelMax_ = kMaxNRPN;
    else
        pitchWheelMax_ = value;
}

void ChannelModel::setPWmin(short value) noexcept(ndebug)
{
    Expects(value <= kMaxNRPN);
    Expects(value >= 0);
    if (value < 0 || value >= pitchWheelMax_)
        pitchWheelMin_ = 0;
    else
        pitchWheelMin_ = value;
}

void ChannelModel::activeToSaved()  const
{
    settingsToSave_.clear();
    for (short i = 0; i <= kMaxMIDI; ++i)
        if (ccMethod_[i] != RSJ::CCmethod::absolute || ccHigh_[i] != kMaxMIDI || ccLow_[i] != 0)
            settingsToSave_.emplace_back(i, ccLow_[i], ccHigh_[i], ccMethod_[i]);
    for (short i = kMaxMIDI + 1; i <= kMaxNRPN; ++i)
        if (ccMethod_[i] != RSJ::CCmethod::absolute || ccHigh_[i] != kMaxNRPN || ccLow_[i] != 0)
            settingsToSave_.emplace_back(i, ccLow_[i], ccHigh_[i], ccMethod_[i]);
}

void ChannelModel::savedToActive() noexcept(ndebug)
{
    //program defaults
    ccLow_.fill(0);
    ccHigh_.fill(0x3FFF);//XCode throws linker error when use ChannelModel::kMaxNRPN here
    ccMethod_.fill(RSJ::CCmethod::absolute);
    for (auto &v : currentV_) //can't use fill as copy/assign deleted for atomic
        v.store(kMaxNRPNHalf, std::memory_order_relaxed);
    for (size_t a = 0; a <= kMaxMIDI; ++a) {
        ccHigh_[a] = kMaxMIDI;
        currentV_[a].store(kMaxMIDIHalf, std::memory_order_relaxed);
    }
    for (const auto& set : settingsToSave_)
        setCC(set.number, set.low, set.high, set.method);
}

ChannelModel::ChannelModel()
{
    //program defaults
    ccLow_.fill(0);
    ccHigh_.fill(0x3FFF);//XCode throws linker error when use ChannelModel::kMaxNRPN here
    ccMethod_.fill(RSJ::CCmethod::absolute);
    for (auto &v : currentV_) //can't use fill as copy/assign deleted for atomic
        v.store(kMaxNRPNHalf, std::memory_order_relaxed);
    for (size_t a = 0; a <= kMaxMIDI; ++a) {
        ccHigh_[a] = kMaxMIDI;
        currentV_[a].store(kMaxMIDIHalf, std::memory_order_relaxed);
    }
    //load settings
}