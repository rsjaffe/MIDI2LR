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
#include <atomic>
#include <cassert>
#include "../JuceLibraryCode/JuceHeader.h"
#include <cereal/access.hpp>
#include <cereal/types/array.hpp>
#include "MidiUtilities.h"
#include "Utilities/Utilities.h"

namespace RSJ {
  enum struct CCmethod: char {
    absolute, twoscomplement, binaryoffset, signmagnitude
  };
}

class ChannelModel {
private:
 /* using timetype = decltype(std::chrono::time_point_cast
    <std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch().count());*/

  constexpr static short kBit14 = 0x2000;
  constexpr static short kBit7 = 0x40;
  constexpr static short kLow13Bits = 0x1FFF;
  constexpr static short kLow6Bits = 0x3F;
  constexpr static short kMaxMIDI = 0x7F;
  constexpr static short kMaxMIDIHalf = kMaxMIDI / 2;
  constexpr static short kMaxNRPN = 0x3FFF;
  constexpr static short kMaxNRPNHalf = kMaxNRPN / 2;
  constexpr static size_t kMaxControls = 0x4000;
//  constexpr static timetype kUpdateDelay = 50;
public:
  ChannelModel();
  ~ChannelModel();
  double controllerToPlugin(short conttype, short controlnumber, short value) noexcept(ndebug);
  RSJ::CCmethod getCCmethod(short controlnum) const noexcept(ndebug);
  short getCCmax(short controlnum) const noexcept(ndebug);
  short getCCmin(short controlnum) const noexcept(ndebug);
  short getPWmax() const noexcept;
  short getPWmin() const noexcept;
  short pluginToController(short conttype, short controlnumber, double value) noexcept(ndebug);
  void setCC(short controlnum, short min, short max, RSJ::CCmethod conttype) noexcept(ndebug);
  void setCCall(short controlnum, short min, short max, RSJ::CCmethod conttype) noexcept(ndebug);
  void setCCmax(short controlnum, short value) noexcept(ndebug);
  void setCCmethod(short controlnum, RSJ::CCmethod value) noexcept(ndebug);
  void setCCmin(short controlnum, short value) noexcept(ndebug);
  void setPWmax(short val) noexcept;
  void setPWmin(short val) noexcept;
private:
  friend class cereal::access;
  template<class Archive>
  void serialize(Archive & archive, std::uint32_t const version) {
    archive(CCmethod_, ccHigh_, ccLow_, PitchWheelMax, PitchWheelMin); // serialize things by passing them to the archive
  }
 /* timetype nowms() const noexcept {
    return std::chrono::time_point_cast<std::chrono::milliseconds>
      (std::chrono::steady_clock::now()).time_since_epoch().count();
  }*/
  bool isNRPN(size_t controlnumber);
  double offsetresult(short diff, size_t controlnumber) noexcept(ndebug);
  short PitchWheelMax{kMaxNRPN};
  short PitchWheelMin{0};
  std::array<RSJ::CCmethod, kMaxControls> CCmethod_;
  std::array<short, kMaxControls> ccHigh_;
  std::array<short, kMaxControls> ccLow_;
  std::array<short, kMaxControls> currentV_;
  mutable RSJ::spinlock mtx_{};
//  std::atomic<timetype> lastUpdate_{0};
};

class ControlsModel {
public:
  ControlsModel();
  ~ControlsModel();
  double controllerToPlugin(short conttype, size_t channel, short controlnumber, short value) noexcept(ndebug) {
    assert(channel <= 15);
    return AllControls[channel].controllerToPlugin(conttype, controlnumber, value);
  };
  RSJ::CCmethod getCCmethod(size_t channel, short controlnum) const noexcept(ndebug) {
    assert(channel <= 15);
    return AllControls[channel].getCCmethod(controlnum);
  };
  short getCCmax(size_t channel, short controlnum) const noexcept(ndebug) {
    assert(channel <= 15);
    return AllControls[channel].getCCmax(controlnum);
  };
  short getCCmin(size_t channel, short controlnum) noexcept(ndebug) {
    assert(channel <= 15);
    return AllControls[channel].getCCmin(controlnum);
  };
  short getPWmax(size_t channel) const noexcept(ndebug) {
    assert(channel <= 15);
    return AllControls[channel].getPWmax();
  };
  short getPWmin(size_t channel) const noexcept(ndebug) {
    assert(channel <= 15);
    return AllControls[channel].getPWmin();
  };
  short pluginToController(short conttype, size_t channel, short controlnumber, double value) noexcept(ndebug) {
    assert(channel <= 15);
    return AllControls[channel].pluginToController(conttype, controlnumber, value);
  };
  void setCC(size_t channel, short controlnum, short min, short max, RSJ::CCmethod conttype) noexcept(ndebug) {
    assert(channel <= 15);
    AllControls[channel].setCC(controlnum, min, max, conttype);
  }
  void setCCall(size_t channel, short controlnum, short min, short max, RSJ::CCmethod conttype) noexcept(ndebug) {
    assert(channel <= 15);
    AllControls[channel].setCCall(controlnum, min, max, conttype);
  };
  void setCCmax(size_t channel, short controlnum, short value) noexcept(ndebug) {
    assert(channel <= 15);
    AllControls[channel].setCCmax(controlnum, value);
  };
  void setCCmethod(size_t channel, short controlnum, RSJ::CCmethod value) noexcept(ndebug) {
    assert(channel <= 15);
    AllControls[channel].setCCmethod(controlnum, value);
  };
  void setCCmin(size_t channel, short controlnum, short value) noexcept(ndebug) {
    assert(channel <= 15);
    AllControls[channel].setCCmin(controlnum, value);
  };
  void setPWmax(size_t channel, short val) noexcept(ndebug) {
    assert(channel <= 15);
    AllControls[channel].setPWmax(val);
  };
  void setPWmin(size_t channel, short val) noexcept(ndebug) {
    assert(channel <= 15);
    AllControls[channel].setPWmin(val);
  };
private:
  friend class cereal::access;
  template<class Archive>
  void serialize(Archive & archive, std::uint32_t const version) {
    archive(AllControls); // serialize things by passing them to the archive
  }
  std::array<ChannelModel, 16> AllControls;
};

inline RSJ::CCmethod ChannelModel::getCCmethod(short controlnum) const noexcept(ndebug) {
  assert(controlnum >= 0 && controlnum <= kMaxNRPN);
  return CCmethod_[controlnum];
}

inline short ChannelModel::getCCmax(short controlnum) const noexcept(ndebug) {
  assert(controlnum >= 0 && controlnum <= kMaxNRPN);
  return ccHigh_[controlnum];
}

inline short ChannelModel::getCCmin(short controlnum) const noexcept(ndebug) {
  assert(controlnum >= 0 && controlnum <= kMaxNRPN);
  return ccLow_[controlnum];
}

inline short ChannelModel::getPWmax() const noexcept {
  return PitchWheelMax;
}

inline short ChannelModel::getPWmin() const noexcept {
  return PitchWheelMin;
}

inline short ChannelModel::pluginToController(short conttype, short controlnumber, double pluginV) noexcept(ndebug) {
  assert(controlnumber >= 0 && controlnumber <= kMaxNRPN);
  assert(pluginV >= 0.0 && pluginV <= 1.0);
  switch (conttype) {
    case RSJ::PWflag:
      return static_cast<short>(round(pluginV * (PitchWheelMax - PitchWheelMin))) + PitchWheelMin;
    case RSJ::CCflag:
      {
        short cv{0};
        {
          std::lock_guard<decltype(mtx_)> lock(mtx_);
          cv = static_cast<short>(round(pluginV * (ccHigh_[static_cast<size_t>(controlnumber)]
            - ccLow_[static_cast<size_t>(controlnumber)])));
          if (CCmethod_[static_cast<size_t>(controlnumber)] != RSJ::CCmethod::absolute /*&& nowms() - kUpdateDelay > lastUpdate_.load()*/)
            currentV_[static_cast<size_t>(controlnumber)] = cv;
        }
        return cv + ccLow_[static_cast<size_t>(controlnumber)];
      }
    case RSJ::NoteOnFlag:
      return kMaxMIDI;
  }
  return 0;
}

inline void ChannelModel::setCC(short controlnum, short min, short max, RSJ::CCmethod conttype) noexcept(ndebug) {
  setCCmin(controlnum, min);
  setCCmax(controlnum, max);
  setCCmethod(controlnum, conttype);
}

inline void ChannelModel::setCCall(short controlnum, short min, short max, RSJ::CCmethod conttype) noexcept(ndebug) {
  assert(controlnum >= 0 && controlnum <= kMaxNRPN);
  if (isNRPN(controlnum))
    for (short a = kMaxMIDI + 1; a <= kMaxNRPN; ++a)
      setCC(a, min, max, conttype);
  else
    for (short a = 0; a <= kMaxMIDI; ++a)
      setCC(a, min, max, conttype);
}

inline void ChannelModel::setCCmax(short controlnum, short value) noexcept(ndebug) {
  assert(controlnum >= 0 && controlnum <= kMaxNRPN);
  if (CCmethod_[controlnum] != RSJ::CCmethod::absolute) {
    ccHigh_[controlnum] = (value < 0) ? 1000 : value;
  }
  else {
    short max = (isNRPN(controlnum) ? kMaxNRPN : kMaxMIDI);
    ccHigh_[controlnum] = (value <= ccLow_[controlnum] || value > max) ? max : value;
  }
  currentV_[controlnum] = (ccHigh_[controlnum] - ccLow_[controlnum]) / 2;
}

inline void ChannelModel::setCCmethod(short controlnum, RSJ::CCmethod value) noexcept(ndebug) {
  assert(controlnum >= 0 && controlnum <= kMaxNRPN);
  CCmethod_[controlnum] = value;
}

inline void ChannelModel::setCCmin(short controlnum, short value) noexcept(ndebug) {
  assert(controlnum >= 0 && controlnum <= kMaxNRPN);
  if (CCmethod_[controlnum] != RSJ::CCmethod::absolute)
    ccLow_[controlnum] = 0;
  else
    ccLow_[controlnum] = (value < 0 || value >= ccHigh_[controlnum]) ? 0 : value;
  currentV_[controlnum] = (ccHigh_[controlnum] - ccLow_[controlnum]) / 2;
}

inline void ChannelModel::setPWmax(short val) noexcept {
  if (val > kMaxNRPN || val <= PitchWheelMin)
    PitchWheelMax = kMaxNRPN;
  else
    PitchWheelMax = val;
}

inline void ChannelModel::setPWmin(short val) noexcept {
  if (val < 0 || val >= PitchWheelMax)
    PitchWheelMin = 0;
  else
    PitchWheelMin = val;
}

inline bool ChannelModel::isNRPN(size_t controlnumber) {
  return controlnumber > kMaxMIDI;
}

inline double ChannelModel::offsetresult(short diff, size_t controlN) noexcept(ndebug) {
  assert(ccHigh_[controlN] > 0);
//  lastUpdate_.store(nowms());
  std::lock_guard<decltype(mtx_)> lock(mtx_);
  currentV_[controlN] += diff;
  if (currentV_[controlN] < 0)
    currentV_[controlN] = 0;
  else if (currentV_[controlN] > ccHigh_[controlN])
    currentV_[controlN] = ccHigh_[controlN];
  return static_cast<double>(currentV_[controlN]) / static_cast<double>(ccHigh_[controlN]);
}

CEREAL_CLASS_VERSION(ChannelModel, 1);
CEREAL_CLASS_VERSION(ControlsModel, 1);
