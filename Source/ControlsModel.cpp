// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
==============================================================================

ControlsModel.cpp

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
#include "ControlsModel.h"

double ChannelModel::ControllerToPlugin(short controltype, size_t controlnumber, short value) noexcept(ndebug) {
  assert((controltype == RSJ::kCCFlag && ccMethod_[controlnumber] == RSJ::CCmethod::absolute) ? (ccLow_[controlnumber] < ccHigh_[controlnumber]) : 1);
  assert((controltype == RSJ::kPWFlag) ? (pitchWheelMax_ > pitchWheelMin_) : 1);
  assert((controltype == RSJ::kPWFlag) ? value >= pitchWheelMin_ && value <= pitchWheelMax_ : 1);
  //note that the value is not msb,lsb, but rather the calculated value. Since lsb is only 7 bits, high bits are shifted one right when placed into short.
  switch (controltype) {
    case RSJ::kPWFlag:
      return static_cast<double>(value - pitchWheelMin_) / static_cast<double>(pitchWheelMax_ - pitchWheelMin_);
    case RSJ::kCCFlag:
      switch (ccMethod_[controlnumber]) {
        case RSJ::CCmethod::absolute:
          return static_cast<double>(value - ccLow_[controlnumber]) / static_cast<double>(ccHigh_[controlnumber] - ccLow_[controlnumber]);
        case RSJ::CCmethod::binaryoffset:
          if (IsNRPN_(controlnumber))
            return OffsetResult_(value - kBit14, controlnumber);
          else
            return OffsetResult_(value - kBit7, controlnumber);
        case RSJ::CCmethod::signmagnitude:
          if (IsNRPN_(controlnumber))
            return OffsetResult_((value & kBit14) ? -(value & kLow13Bits) : value, controlnumber);
          else
            return OffsetResult_((value & kBit7) ? -(value & kLow6Bits) : value, controlnumber);
        case RSJ::CCmethod::twoscomplement: //see https://en.wikipedia.org/wiki/Signed_number_representations#Two.27s_complement
          if (IsNRPN_(controlnumber)) //flip twos comp and subtract--independent of processor architecture
            return OffsetResult_((value & kBit14) ? -((value ^ kMaxNRPN) + 1) : value, controlnumber);
          else
            return OffsetResult_((value & kBit7) ? -((value ^ kMaxMIDI) + 1) : value, controlnumber);
        default:
          assert(!"Should be unreachable code in ControllerToPlugin--unknown CCmethod");
          return 0.0;
      }
    case RSJ::kNoteOnFlag:
      return 1.0;
    case RSJ::kNoteOffFlag:
      return 0.0;
    default:
      assert(!"Should be unreachable code in ControllerToPlugin--unknown control type");
      return 0.0;
  }
}

short ChannelModel::PluginToController(short controltype, size_t controlnumber, double pluginV) noexcept(ndebug) {
  assert(controlnumber <= kMaxNRPN);
  assert(pluginV >= 0.0 && pluginV <= 1.0);
  switch (controltype) {
    case RSJ::kPWFlag:
      return static_cast<short>(round(pluginV * (pitchWheelMax_ - pitchWheelMin_))) + pitchWheelMin_;
    case RSJ::kCCFlag:
      {
        if (ccMethod_[controlnumber] == RSJ::CCmethod::absolute)
          return static_cast<short>(round(pluginV *
          (ccHigh_[controlnumber] - ccLow_[controlnumber]))) + ccLow_[controlnumber];
        short cv = static_cast<short>(round(pluginV * ccHigh_[controlnumber])); //ccLow == 0 for non-absolute
        if (RSJ::now_ms() - kUpdateDelay > lastUpdate_.load(std::memory_order_acquire))
          currentV_[controlnumber].store(cv, std::memory_order_release);
        return cv;
      }
    case RSJ::kNoteOnFlag:
      return kMaxMIDI;
  }
  return 0;
}

void ChannelModel::setCC(size_t controlnumber, short min, short max, RSJ::CCmethod controltype) noexcept(ndebug) {
  setCCmin(controlnumber, min);
  setCCmax(controlnumber, max);
  setCCmethod(controlnumber, controltype);
}

void ChannelModel::setCCall(size_t controlnumber, short min, short max, RSJ::CCmethod controltype) noexcept(ndebug) {
  if (IsNRPN_(controlnumber))
    for (short a = kMaxMIDI + 1; a <= kMaxNRPN; ++a)
      setCC(a, min, max, controltype);
  else
    for (short a = 0; a <= kMaxMIDI; ++a)
      setCC(a, min, max, controltype);
}

void ChannelModel::setCCmax(size_t controlnumber, short value) noexcept(ndebug) {
  assert(controlnumber <= kMaxNRPN);
  assert(value <= kMaxNRPN);
  assert(value >= 0);
  if (ccMethod_[controlnumber] != RSJ::CCmethod::absolute) {
    ccHigh_[controlnumber] = (value < 0) ? 1000 : value;
  }
  else {
    short max = (IsNRPN_(controlnumber) ? kMaxNRPN : kMaxMIDI);
    ccHigh_[controlnumber] = (value <= ccLow_[controlnumber] || value > max) ? max : value;
  }
  currentV_[controlnumber].store((ccHigh_[controlnumber] - ccLow_[controlnumber]) / 2, std::memory_order_release);
}

void ChannelModel::setCCmethod(size_t controlnumber, RSJ::CCmethod value) noexcept(ndebug) {
  assert(controlnumber <= kMaxNRPN);
  ccMethod_[controlnumber] = value;
}

void ChannelModel::setCCmin(size_t controlnumber, short value) noexcept(ndebug) {
  assert(controlnumber <= kMaxNRPN);
  assert(value <= kMaxNRPN);
  assert(value >= 0);
  if (ccMethod_[controlnumber] != RSJ::CCmethod::absolute)
    ccLow_[controlnumber] = 0;
  else
    ccLow_[controlnumber] = (value < 0 || value >= ccHigh_[controlnumber]) ? 0 : value;
  currentV_[controlnumber].store((ccHigh_[controlnumber] - ccLow_[controlnumber]) / 2, std::memory_order_release);
}

void ChannelModel::setPWmax(short value) noexcept(ndebug) {
  assert(value <= kMaxNRPN);
  assert(value >= 0);
  if (value > kMaxNRPN || value <= pitchWheelMin_)
    pitchWheelMax_ = kMaxNRPN;
  else
    pitchWheelMax_ = value;
}

void ChannelModel::setPWmin(short value) noexcept(ndebug) {
  assert(value <= kMaxNRPN);
  assert(value >= 0);
  if (value < 0 || value >= pitchWheelMax_)
    pitchWheelMin_ = 0;
  else
    pitchWheelMin_ = value;
}

void ChannelModel::activeToSaved()  const {
  settingsToSave_.clear();
  for (short i = 0; i <= kMaxMIDI; ++i)
    if (ccMethod_[i] != RSJ::CCmethod::absolute || ccHigh_[i] != kMaxMIDI || ccLow_[i] != 0)
      settingsToSave_.emplace_back(i, ccHigh_[i], ccLow_[i], ccMethod_[i]);
  for (short i = kMaxMIDI + 1; i <= kMaxNRPN; ++i)
    if (ccMethod_[i] != RSJ::CCmethod::absolute || ccHigh_[i] != kMaxNRPN || ccLow_[i] != 0)
      settingsToSave_.emplace_back(i, ccHigh_[i], ccLow_[i], ccMethod_[i]);
}

void ChannelModel::savedToActive() noexcept(ndebug) {
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
  for (auto set : settingsToSave_) {
    setCCall(set.number, set.low, set.high, set.method);
  }
}
ControlsModel::ControlsModel() {}
ChannelModel::ChannelModel() {
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