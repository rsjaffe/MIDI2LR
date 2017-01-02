#pragma once
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
#include <array>
#include <cassert>
#include "Utilities/Utilities.h"
#include <cereal/access.hpp>
#include <cereal/types/array.hpp>
#include "MidiUtilities.h"
#include "../JuceLibraryCode/JuceHeader.h"
namespace RSJ {
  enum struct CCmethod: char {
    absolute, twoscomplement, binaryoffset, signmagnitude
  };
}

class ChannelModel {
private:
  constexpr static short kBit14 = 0x2000;
  constexpr static short kBit7 = 0x40;
  constexpr static short kLow13Bits = 0x1FFF;
  constexpr static short kLow6Bits = 0x3F;
  constexpr static short kMaxMIDI = 0x7F;
  constexpr static short kMaxMIDIHalf = kMaxMIDI / 2;
  constexpr static short kMaxNRPN = 0x3FFF;
  constexpr static short kMaxNRPNHalf = kMaxNRPN / 2;
  constexpr static size_t kMaxControls = 0x4000;

public:
  ChannelModel();
  ~ChannelModel();
  double controllerToPlugin(short conttype, short controlnumber, short value) noexcept(ndebug);
  short pluginToController(short conttype, short controlnumber, double value) noexcept;
  void setCC(short controlnum, short min, short max, RSJ::CCmethod conttype) noexcept {
    setCCmin(controlnum, min);
    setCCmax(controlnum, max);
    setCCmethod(controlnum, conttype);
  };
  void setCCall(short controlnum, short min, short max, RSJ::CCmethod conttype) noexcept {
    if (isNRPN(controlnum))
      for (short a = kMaxMIDI + 1; a <= kMaxNRPN; a++)
        setCC(a, min, max, conttype);
    else
      for (short a = 0; a <= kMaxMIDI; a++)
        setCC(a, min, max, conttype);
  };
  short getCCmin(short controlnum) const noexcept {
    return ccLow_[controlnum];
  };
  short getCCmax(short controlnum) const noexcept {
    return ccHigh_[controlnum];
  };
  RSJ::CCmethod getCCmethod(short controlnum) const noexcept {
    return CCmethod_[controlnum];
  };
  void setCCmin(short controlnum, short value) noexcept {
    if (CCmethod_[controlnum] != RSJ::CCmethod::absolute)
      ccLow_[controlnum] = 0;
    else
      ccLow_[controlnum] = (value < 0 || value >= ccHigh_[controlnum]) ? 0 : value;
    currentV_[controlnum] = (ccHigh_[controlnum] - ccLow_[controlnum]) / 2;
  };
  void setCCmax(short controlnum, short value) noexcept {
    if (CCmethod_[controlnum] != RSJ::CCmethod::absolute) {
      ccHigh_[controlnum] = (value < 0) ? 1000 : value;
    }
    else {
      short max = (isNRPN(controlnum) ? kMaxNRPN : kMaxMIDI);
      ccHigh_[controlnum] = (value <= ccLow_[controlnum] || value > max) ? max : value;
    }
    currentV_[controlnum] = (ccHigh_[controlnum] - ccLow_[controlnum]) / 2;
  };
  void setCCmethod(short controlnum, RSJ::CCmethod value) noexcept {
    CCmethod_[controlnum] = value;
  }
  short getPWmin() const noexcept {
    return PitchWheelMin;
  };
  short getPWmax() const noexcept {
    return PitchWheelMax;
  };
  void setPWmin(short val) noexcept {
    if (val < 0 || val >= PitchWheelMax)
      PitchWheelMin = 0;
    else
      PitchWheelMin = val;
  };
  void setPWmax(short val) noexcept {
    if (val > kMaxNRPN || val <= PitchWheelMin)
      PitchWheelMax = kMaxNRPN;
    else
      PitchWheelMax = val;
  };
private:
  friend class cereal::access;
  template<class Archive>
  void serialize(Archive & archive, std::uint32_t const version) {
    archive(CCmethod_, ccHigh_, ccLow_, PitchWheelMax, PitchWheelMin); // serialize things by passing them to the archive
  }

  double offsetresult(short diff, size_t controlnumber) noexcept(ndebug);
  bool isNRPN(size_t controlnumber) {
    return controlnumber > kMaxMIDI;
  };

  short PitchWheelMax{kMaxNRPN};
  short PitchWheelMin{0};
  std::array<RSJ::CCmethod, kMaxControls> CCmethod_;
  std::array<short, kMaxControls> ccHigh_;
  std::array<short, kMaxControls> ccLow_;
  std::array<short, kMaxControls> currentV_;
};

class ControlsModel {
public:
  ControlsModel();
  ~ControlsModel();
  double controllerToPlugin(short conttype, size_t channel, short controlnumber, short value) noexcept(ndebug) {
    assert(channel <= 15 && channel >= 0);
    return AllControls[channel].controllerToPlugin(conttype, controlnumber, value);
  };
  short pluginToController(short conttype, size_t channel, short controlnumber, double value) noexcept(ndebug) {
    assert(channel <= 15 && channel >= 0);
    return AllControls[channel].pluginToController(conttype, controlnumber, value);
  };
  void setCC(size_t channel, short controlnum, short min, short max, RSJ::CCmethod conttype) noexcept(ndebug) {
    assert(channel <= 15 && channel >= 0);
    AllControls[channel].setCC(controlnum, min, max, conttype);
  }
  void setCCall(size_t channel, short controlnum, short min, short max, RSJ::CCmethod conttype) noexcept(ndebug) {
    assert(channel <= 15 && channel >= 0);
    AllControls[channel].setCCall(controlnum, min, max, conttype);
  };
  void setCCmethod(size_t channel, short controlnum, RSJ::CCmethod value) noexcept(ndebug) {
    assert(channel <= 15 && channel >= 0);
    AllControls[channel].setCCmethod(controlnum, value);
  };
  void setCCmin(size_t channel, short controlnum, short value) noexcept(ndebug) {
    assert(channel <= 15 && channel >= 0);
    AllControls[channel].setCCmin(controlnum, value);
  };
  void setCCmax(size_t channel, short controlnum, short value) noexcept(ndebug) {
    assert(channel <= 15 && channel >= 0);
    AllControls[channel].setCCmax(controlnum, value);
  };
  short getCCmin(size_t channel, short controlnum) noexcept(ndebug) {
    assert(channel <= 15 && channel >= 0);
    return AllControls[channel].getCCmin(controlnum);
  };
  short getCCmax(size_t channel, short controlnum) const noexcept(ndebug) {
    assert(channel <= 15 && channel >= 0);
    return AllControls[channel].getCCmax(controlnum);
  };
  RSJ::CCmethod getCCmethod(size_t channel, short controlnum) const noexcept(ndebug) {
    assert(channel <= 15 && channel >= 0);
    return AllControls[channel].getCCmethod(controlnum);
  };
  short getPWmin(size_t channel) const noexcept(ndebug) {
    assert(channel <= 15 && channel >= 0);
    return AllControls[channel].getPWmin();
  };
  short getPWmax(size_t channel) const noexcept(ndebug) {
    assert(channel <= 15 && channel >= 0);
    return AllControls[channel].getPWmax();
  };
  void setPWmin(size_t channel, short val) noexcept(ndebug) {
    assert(channel <= 15 && channel >= 0);
    AllControls[channel].setPWmin(val);
  };
  void setPWmax(size_t channel, short val) noexcept(ndebug) {
    assert(channel <= 15 && channel >= 0);
    AllControls[channel].setPWmax(val);
  };
private:
  friend class cereal::access;
  template<class Archive>
  void serialize(Archive & archive, std::uint32_t const version) {
    archive(AllControls); // serialize things by passing them to the archive
  }
  std::array<ChannelModel, 16> AllControls;
};


inline double ChannelModel::offsetresult(short diff, size_t controlN) noexcept(ndebug) {
  assert(ccHigh_[controlN] > 0);
  currentV_[controlN] += diff;
  if (currentV_[controlN] < 0)
    currentV_[controlN] = 0;
  else if (currentV_[controlN] > ccHigh_[controlN])
    currentV_[controlN] = ccHigh_[controlN];
  return static_cast<double>(currentV_[controlN]) / static_cast<double>(ccHigh_[controlN]);
}


inline short ChannelModel::pluginToController(short conttype, short controlnumber, double pluginV) noexcept {
  switch (conttype) {
    case RSJ::PWflag:
      return static_cast<short>(round(pluginV * (PitchWheelMax - PitchWheelMin))) + PitchWheelMin;
    case RSJ::CCflag:
      currentV_[controlnumber] = static_cast<short>(round(pluginV * (ccHigh_[controlnumber] - ccLow_[controlnumber])));
      return currentV_[controlnumber] + ccLow_[controlnumber];
    case RSJ::NoteOnFlag:
      return kMaxMIDI;
  }
  return 0;
}

CEREAL_CLASS_VERSION(ChannelModel, 1);
CEREAL_CLASS_VERSION(ControlsModel, 1);
