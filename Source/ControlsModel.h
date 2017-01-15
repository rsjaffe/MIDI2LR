#pragma once
/*
==============================================================================

ControlsModel.h

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
#include <chrono>
#include "../JuceLibraryCode/JuceHeader.h"
#include <cereal/access.hpp>
#include <cereal/types/array.hpp>
#include "MidiUtilities.h"
#include "Utilities/Utilities.h"

namespace RSJ {
  enum struct CCmethod: char {
    absolute, twoscomplement, binaryoffset, signmagnitude
  };
  inline auto now_ms() noexcept {
    return std::chrono::time_point_cast<std::chrono::milliseconds>
      (std::chrono::steady_clock::now()).time_since_epoch().count();
  }
  using timetype = decltype(now_ms());
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
  constexpr static RSJ::timetype kUpdateDelay = 250;
public:
  ChannelModel();
  ~ChannelModel();
  ChannelModel(const ChannelModel&) = delete; //can't copy atomics
  ChannelModel& operator= (const ChannelModel&) = delete; //can't assign atomics
  double controllerToPlugin(short controltype, short controlnumber, short value) noexcept(ndebug);
  RSJ::CCmethod getCCmethod(short controlnumber) const noexcept(ndebug);
  short getCCmax(short controlnumber) const noexcept(ndebug);
  short getCCmin(short controlnumber) const noexcept(ndebug);
  short getPWmax() const noexcept;
  short getPWmin() const noexcept;
  short pluginToController(short controltype, short controlnumber, double value) noexcept(ndebug);
  void setCC(short controlnumber, short min, short max, RSJ::CCmethod controltype) noexcept(ndebug);
  void setCCall(short controlnumber, short min, short max, RSJ::CCmethod controltype) noexcept(ndebug);
  void setCCmax(short controlnumber, short value) noexcept(ndebug);
  void setCCmethod(short controlnumber, RSJ::CCmethod value) noexcept(ndebug);
  void setCCmin(short controlnumber, short value) noexcept(ndebug);
  void setPWmax(short value) noexcept(ndebug);
  void setPWmin(short value) noexcept(ndebug);
private:
  friend class cereal::access;
  template<class Archive>
  void serialize(Archive & archive, std::uint32_t const version) {
    archive(CCmethod_, ccHigh_, ccLow_, PitchWheelMax, PitchWheelMin); // serialize things by passing them to the archive
  }
  bool isNRPN(size_t controlnumber) const noexcept(ndebug);
  double offsetresult(short diff, size_t controlnumber) noexcept(ndebug);
  short PitchWheelMax{kMaxNRPN};
  short PitchWheelMin{0};
  std::array<RSJ::CCmethod, kMaxControls> CCmethod_;
  std::array<short, kMaxControls> ccHigh_;
  std::array<short, kMaxControls> ccLow_;
  std::array<std::atomic<short>, kMaxControls> currentV_;
  mutable std::atomic<RSJ::timetype> lastUpdate_{0};
};

class ControlsModel {
public:
  ControlsModel();
  ~ControlsModel();
  double controllerToPlugin(short controltype, size_t channel, short controlnumber, short value) noexcept(ndebug) {
    assert(channel <= 15);
    return AllControls[channel].controllerToPlugin(controltype, controlnumber, value);
  };
  RSJ::CCmethod getCCmethod(size_t channel, short controlnumber) const noexcept(ndebug) {
    assert(channel <= 15);
    return AllControls[channel].getCCmethod(controlnumber);
  };
  short getCCmax(size_t channel, short controlnumber) const noexcept(ndebug) {
    assert(channel <= 15);
    return AllControls[channel].getCCmax(controlnumber);
  };
  short getCCmin(size_t channel, short controlnumber) noexcept(ndebug) {
    assert(channel <= 15);
    return AllControls[channel].getCCmin(controlnumber);
  };
  short getPWmax(size_t channel) const noexcept(ndebug) {
    assert(channel <= 15);
    return AllControls[channel].getPWmax();
  };
  short getPWmin(size_t channel) const noexcept(ndebug) {
    assert(channel <= 15);
    return AllControls[channel].getPWmin();
  };
  short pluginToController(short controltype, size_t channel, short controlnumber, double value) noexcept(ndebug) {
    assert(channel <= 15);
    return AllControls[channel].pluginToController(controltype, controlnumber, value);
  };
  void setCC(size_t channel, short controlnumber, short min, short max, RSJ::CCmethod controltype) noexcept {
    assert(channel <= 15);
    AllControls[channel].setCC(controlnumber, min, max, controltype);
  }
  void setCCall(size_t channel, short controlnumber, short min, short max, RSJ::CCmethod controltype) noexcept(ndebug) {
    assert(channel <= 15);
    AllControls[channel].setCCall(controlnumber, min, max, controltype);
  };
  void setCCmax(size_t channel, short controlnumber, short value) noexcept(ndebug) {
    assert(channel <= 15);
    AllControls[channel].setCCmax(controlnumber, value);
  };
  void setCCmethod(size_t channel, short controlnumber, RSJ::CCmethod value) noexcept(ndebug) {
    assert(channel <= 15);
    AllControls[channel].setCCmethod(controlnumber, value);
  };
  void setCCmin(size_t channel, short controlnumber, short value) noexcept(ndebug) {
    assert(channel <= 15);
    AllControls[channel].setCCmin(controlnumber, value);
  };
  void setPWmax(size_t channel, short value) noexcept(ndebug) {
    assert(channel <= 15);
    AllControls[channel].setPWmax(value);
  };
  void setPWmin(size_t channel, short value) noexcept(ndebug) {
    assert(channel <= 15);
    AllControls[channel].setPWmin(value);
  };
private:
  friend class cereal::access;
  template<class Archive>
  void serialize(Archive & archive, std::uint32_t const version) {
    archive(AllControls); // serialize things by passing them to the archive
  }
  std::array<ChannelModel, 16> AllControls;
};

inline RSJ::CCmethod ChannelModel::getCCmethod(short controlnumber) const noexcept(ndebug) {
  assert(controlnumber >= 0 && controlnumber <= kMaxNRPN);
  return CCmethod_[controlnumber];
}

inline short ChannelModel::getCCmax(short controlnumber) const noexcept(ndebug) {
  assert(controlnumber >= 0 && controlnumber <= kMaxNRPN);
  return ccHigh_[controlnumber];
}

inline short ChannelModel::getCCmin(short controlnumber) const noexcept(ndebug) {
  assert(controlnumber >= 0 && controlnumber <= kMaxNRPN);
  return ccLow_[controlnumber];
}

inline short ChannelModel::getPWmax() const noexcept {
  return PitchWheelMax;
}

inline short ChannelModel::getPWmin() const noexcept {
  return PitchWheelMin;
}

inline short ChannelModel::pluginToController(short controltype, short controlnumber, double pluginV) noexcept(ndebug) {
  assert(controlnumber >= 0 && controlnumber <= kMaxNRPN);
  assert(pluginV >= 0.0 && pluginV <= 1.0);
  switch (controltype) {
    case RSJ::PWflag:
      return static_cast<short>(round(pluginV * (PitchWheelMax - PitchWheelMin))) + PitchWheelMin;
    case RSJ::CCflag:
      {
        const size_t idx = static_cast<size_t>(controlnumber);
        short cv = static_cast<short>(round(pluginV * (ccHigh_[idx] - ccLow_[idx]))) + ccLow_[idx];
        if (CCmethod_[idx] != RSJ::CCmethod::absolute && RSJ::now_ms() -
          kUpdateDelay > lastUpdate_.load(std::memory_order_acquire)) {
          currentV_[idx].store(cv, std::memory_order_relaxed);
        }
        return cv;
      }
    case RSJ::NoteOnFlag:
      return kMaxMIDI;
  }
  return 0;
}

inline void ChannelModel::setCC(short controlnumber, short min, short max, RSJ::CCmethod controltype) noexcept {
  setCCmin(controlnumber, min);
  setCCmax(controlnumber, max);
  setCCmethod(controlnumber, controltype);
}

inline void ChannelModel::setCCall(short controlnumber, short min, short max, RSJ::CCmethod controltype) noexcept(ndebug) {
  assert(controlnumber >= 0 && controlnumber <= kMaxNRPN);
  if (isNRPN(controlnumber))
    for (short a = kMaxMIDI + 1; a <= kMaxNRPN; ++a)
      setCC(a, min, max, controltype);
  else
    for (short a = 0; a <= kMaxMIDI; ++a)
      setCC(a, min, max, controltype);
}

inline void ChannelModel::setCCmax(short controlnumber, short value) noexcept(ndebug) {
  assert(controlnumber >= 0 && controlnumber <= kMaxNRPN);
  assert(value <= kMaxNRPN);
  assert(value >= 0);
  if (CCmethod_[controlnumber] != RSJ::CCmethod::absolute) {
    ccHigh_[controlnumber] = (value < 0) ? 1000 : value;
  }
  else {
    short max = (isNRPN(controlnumber) ? kMaxNRPN : kMaxMIDI);
    ccHigh_[controlnumber] = (value <= ccLow_[controlnumber] || value > max) ? max : value;
  }
  currentV_[controlnumber] = (ccHigh_[controlnumber] - ccLow_[controlnumber]) / 2;
}

inline void ChannelModel::setCCmethod(short controlnumber, RSJ::CCmethod value) noexcept(ndebug) {
  assert(controlnumber >= 0 && controlnumber <= kMaxNRPN);
  CCmethod_[controlnumber] = value;
}

inline void ChannelModel::setCCmin(short controlnumber, short value) noexcept(ndebug) {
  assert(controlnumber >= 0 && controlnumber <= kMaxNRPN);
  assert(value <= kMaxNRPN);
  assert(value >= 0);
  if (CCmethod_[controlnumber] != RSJ::CCmethod::absolute)
    ccLow_[controlnumber] = 0;
  else
    ccLow_[controlnumber] = (value < 0 || value >= ccHigh_[controlnumber]) ? 0 : value;
  currentV_[controlnumber] = (ccHigh_[controlnumber] - ccLow_[controlnumber]) / 2;
}

inline void ChannelModel::setPWmax(short value) noexcept(ndebug) {
  assert(value <= kMaxNRPN);
  assert(value >= 0);
  if (value > kMaxNRPN || value <= PitchWheelMin)
    PitchWheelMax = kMaxNRPN;
  else
    PitchWheelMax = value;
}

inline void ChannelModel::setPWmin(short value) noexcept(ndebug) {
  assert(value <= kMaxNRPN);
  assert(value >= 0);
  if (value < 0 || value >= PitchWheelMax)
    PitchWheelMin = 0;
  else
    PitchWheelMin = value;
}

inline bool ChannelModel::isNRPN(size_t controlnumber) const noexcept(ndebug) {
  assert(controlnumber >= 0 && controlnumber <= kMaxNRPN);
  return controlnumber > kMaxMIDI;
}

inline double ChannelModel::offsetresult(short diff, size_t controlN) noexcept(ndebug) {
  assert(ccHigh_[controlN] > ccLow_[controlN]);
  assert(diff <= kMaxNRPN && diff >= -kMaxNRPN);
  assert(controlN >= 0 && controlN <= kMaxNRPN);
  lastUpdate_.store(RSJ::now_ms(), std::memory_order_release);
  short cv = currentV_[controlN].fetch_add(diff) + diff;
  if (cv < ccLow_[controlN]) {//fix currentV unless another thread has already altered it
    currentV_[controlN].compare_exchange_strong(cv, ccLow_[controlN]);
    return 0.0;
  }
  if (cv > ccHigh_[controlN]) {//fix currentV unless another thread has already altered it
    currentV_[controlN].compare_exchange_strong(cv, ccHigh_[controlN]);
    return 1.0;
  }
  return static_cast<double>(cv - ccLow_[controlN]) / static_cast<double>(ccHigh_[controlN] - ccLow_[controlN]);
}

CEREAL_CLASS_VERSION(ChannelModel, 1);
CEREAL_CLASS_VERSION(ControlsModel, 1);
