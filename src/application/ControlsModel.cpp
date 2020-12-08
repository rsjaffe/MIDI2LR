/*
 * This file is part of MIDI2LR. Copyright (C) 2015 by Rory Jaffe.
 *
 * MIDI2LR is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with MIDI2LR.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include "ControlsModel.h"

#include <algorithm>
#include <cmath>
#include <exception>

#include "MidiUtilities.h"
#include "Misc.h"

double ChannelModel::OffsetResult(const int diff, const int controlnumber)
{
   try {
      Expects(cc_high_.at(controlnumber) > 0); /* CCLow will always be 0 for offset controls */
      Expects(diff <= kMaxNrpn && diff >= -kMaxNrpn);
      Expects(controlnumber <= kMaxNrpn && controlnumber >= 0);
      const auto high_limit {cc_high_.at(controlnumber)};
      auto cached_v {current_v_.at(controlnumber).load(std::memory_order_acquire)};
      const auto new_v {std::clamp(cached_v + diff, 0, high_limit)};
      if (current_v_.at(controlnumber)
              .compare_exchange_strong(
                  cached_v, new_v, std::memory_order_release, std::memory_order_acquire))
         return static_cast<double>(new_v) / static_cast<double>(high_limit);
      /* someone else got to change the value first, use theirs to be consistent � cached_v updated
       * by exchange */
      return static_cast<double>(cached_v) / static_cast<double>(high_limit);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

double ChannelModel::ControllerToPlugin(
    const rsj::MessageType controltype, const int controlnumber, const int value)
{
   try {
      Expects(controltype == rsj::MessageType::Cc
                      && cc_method_.at(controlnumber) == rsj::CCmethod::kAbsolute
                  ? cc_low_.at(controlnumber) < cc_high_.at(controlnumber)
                  : 1);
      Expects(controltype == rsj::MessageType::Pw ? pitch_wheel_max_ > pitch_wheel_min_ : 1);
      Expects(controltype == rsj::MessageType::Pw
                  ? value >= pitch_wheel_min_ && value <= pitch_wheel_max_
                  : 1);
      /* note that the value is not msb,lsb, but rather the calculated value. Since lsb is only 7
       * bits, high bits are shifted one right when placed into int. */
      switch (controltype) {
      case rsj::MessageType::Pw:
         pitch_wheel_current_.store(value, std::memory_order_release);
#pragma warning(suppress : 26451) /* int subtraction won't overflow 4 bytes here */
         return static_cast<double>(value - pitch_wheel_min_)
                / static_cast<double>(pitch_wheel_max_ - pitch_wheel_min_);
      case rsj::MessageType::Cc:
         switch (cc_method_.at(controlnumber)) {
         case rsj::CCmethod::kAbsolute:
            current_v_.at(controlnumber).store(value, std::memory_order_release);
#pragma warning(suppress : 26451) /* int subtraction won't overflow 4 bytes here */
            return static_cast<double>(value - cc_low_.at(controlnumber))
                   / static_cast<double>(cc_high_.at(controlnumber) - cc_low_.at(controlnumber));
         case rsj::CCmethod::kBinaryOffset:
            if (IsNRPN_(controlnumber))
               return OffsetResult(value - kBit14, controlnumber);
            return OffsetResult(value - kBit7, controlnumber);
         case rsj::CCmethod::kSignMagnitude:
            if (IsNRPN_(controlnumber))
               return OffsetResult(value & kBit14 ? -(value & kLow13Bits) : value, controlnumber);
            return OffsetResult(value & kBit7 ? -(value & kLow6Bits) : value, controlnumber);
         case rsj::CCmethod::kTwosComplement:
            /* SEE:https://en.wikipedia.org/wiki/Signed_number_representations#Two.27s_complement
             * flip twos comp and subtract--independent of processor architecture */
            if (IsNRPN_(controlnumber))
               return OffsetResult(
                   value & kBit14 ? -((value ^ kMaxNrpn) + 1) : value, controlnumber);
            return OffsetResult(value & kBit7 ? -((value ^ kMaxMidi) + 1) : value, controlnumber);
         default:
            Ensures(!"Should be unreachable code in ControllerToPlugin--unknown CCmethod");
            // ReSharper disable once CppUnreachableCode
            return 0.0;
         }
      case rsj::MessageType::NoteOn:
         return static_cast<double>(value)
                / static_cast<double>((IsNRPN_(controlnumber) ? kMaxNrpn : kMaxMidi));
      case rsj::MessageType::NoteOff:
         return 0.0;
      default:
         Ensures(!"Should be unreachable code in ControllerToPlugin--unknown control type");
         // ReSharper disable once CppUnreachableCode
         return 0.0;
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

/* Note: rounding up on set to center (adding remainder of %2) to center the control's LED when
 * centered */
int ChannelModel::SetToCenter(const rsj::MessageType controltype, const int controlnumber)
{
   try {
      auto retval {0};
      switch (controltype) {
      case rsj::MessageType::Pw:
         retval = CenterPw();
         pitch_wheel_current_.store(retval, std::memory_order_release);
         break;
      case rsj::MessageType::Cc:
         if (cc_method_.at(controlnumber) == rsj::CCmethod::kAbsolute) {
            retval = CenterCc(controlnumber);
            current_v_.at(controlnumber).store(retval, std::memory_order_release);
         }
         break;
      default:
         break;
      }
      return retval;
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

int ChannelModel::MeasureChange(
    const rsj::MessageType controltype, const int controlnumber, const int value)
{
   try {
      Expects(controltype == rsj::MessageType::Cc
                      && cc_method_.at(controlnumber) == rsj::CCmethod::kAbsolute
                  ? cc_low_.at(controlnumber) < cc_high_.at(controlnumber)
                  : 1);
      Expects(controltype == rsj::MessageType::Pw ? pitch_wheel_max_ > pitch_wheel_min_ : 1);
      Expects(controltype == rsj::MessageType::Pw
                  ? value >= pitch_wheel_min_ && value <= pitch_wheel_max_
                  : 1);
      /* note that the value is not msb,lsb, but rather the calculated value. Since lsb is only 7
       * bits, high bits are shifted one right when placed into int. */
      switch (controltype) {
      case rsj::MessageType::Pw: {
         return value - pitch_wheel_current_.exchange(value, std::memory_order_acq_rel);
      }
      case rsj::MessageType::Cc:
         switch (cc_method_.at(controlnumber)) {
         case rsj::CCmethod::kAbsolute:
            return value - current_v_.at(controlnumber).exchange(value, std::memory_order_acq_rel);
         case rsj::CCmethod::kBinaryOffset:
            if (IsNRPN_(controlnumber))
               return value - kBit14;
            return value - kBit7;
         case rsj::CCmethod::kSignMagnitude:
            if (IsNRPN_(controlnumber))
               return value & kBit14 ? -(value & kLow13Bits) : value;
            return value & kBit7 ? -(value & kLow6Bits) : value;
         case rsj::CCmethod::kTwosComplement:
            /* SEE:https://en.wikipedia.org/wiki/Signed_number_representations#Two.27s_complement
             * flip twos comp and subtract--independent of processor architecture */
            if (IsNRPN_(controlnumber))
               return value & kBit14 ? -((value ^ kMaxNrpn) + 1) : value;
            return value & kBit7 ? -((value ^ kMaxMidi) + 1) : value;
         default:
            Ensures(!"Should be unreachable code in ControllerToPlugin--unknown CCmethod");
            // ReSharper disable once CppUnreachableCode
            return int {0};
         }
      case rsj::MessageType::NoteOn:
      case rsj::MessageType::NoteOff:
         return int {0};
      default:
         Ensures(!"Should be unreachable code in ControllerToPlugin--unknown control type");
         // ReSharper disable once CppUnreachableCode
         return int {0};
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

#pragma warning(push)
#pragma warning(disable : 26451) /* see TODO below */
int ChannelModel::PluginToController(
    const rsj::MessageType controltype, const int controlnumber, const double value)
{
   try {
      Expects(controlnumber <= kMaxNrpn && controlnumber >= 0);
      /* value effectively clamped to 0-1 by clamp calls below */
      switch (controltype) {
      case rsj::MessageType::Pw: {
         /* TODO(C26451): int subtraction: can it overflow? */
         const auto newv {
             std::clamp(gsl::narrow<int>(std::lrint(value * (pitch_wheel_max_ - pitch_wheel_min_)))
                            + pitch_wheel_min_,
                 pitch_wheel_min_, pitch_wheel_max_)};
         pitch_wheel_current_.store(newv, std::memory_order_release);
         return newv;
      }
      case rsj::MessageType::Cc: {
         /* TODO(C26451): int subtraction: can it overflow? */
         const auto clow {cc_low_.at(controlnumber)};
         const auto chigh {cc_high_.at(controlnumber)};
         const auto newv {
             std::clamp(gsl::narrow<int>(std::lrint(value * (chigh - clow))) + clow, clow, chigh)};
         current_v_.at(controlnumber).store(newv, std::memory_order_release);
         return newv;
      }
      case rsj::MessageType::NoteOn:
         return kMaxMidi;
      default:
         Ensures(!"Unexpected control type");
      }
      // ReSharper disable once CppUnreachableCode
      return 0;
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}
#pragma warning(pop)

void ChannelModel::SetCc(
    const int controlnumber, const int min, const int max, const rsj::CCmethod controltype)
{
   try {
      /* CcMethod has to be set before others or ranges won't be correct */
      SetCcMethod(controlnumber, controltype);
      SetCcMin(controlnumber, min);
      SetCcMax(controlnumber, max);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void ChannelModel::SetCcAll(
    const int controlnumber, const int min, const int max, const rsj::CCmethod controltype)
{
   try {
      if (IsNRPN_(controlnumber))
         for (auto a {kMaxMidi + 1}; a <= kMaxNrpn; ++a) SetCc(a, min, max, controltype);
      else
         for (auto a {0}; a <= kMaxMidi; ++a) SetCc(a, min, max, controltype);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void ChannelModel::SetCcMax(const int controlnumber, const int value)
{
   try {
      Expects(controlnumber <= kMaxNrpn && controlnumber >= 0);
      Expects(value <= kMaxNrpn && value >= 0);
      if (cc_method_.at(controlnumber) != rsj::CCmethod::kAbsolute)
         cc_high_.at(controlnumber) = value < 0 ? 1000 : value;
      else {
         const auto max {IsNRPN_(controlnumber) ? kMaxNrpn : kMaxMidi};
         cc_high_.at(controlnumber) =
             value <= cc_low_.at(controlnumber) || value > max ? max : value;
      }
      current_v_.at(controlnumber).store(CenterCc(controlnumber), std::memory_order_release);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void ChannelModel::SetCcMin(const int controlnumber, const int value)
{
   try {
      Expects(controlnumber <= kMaxNrpn && controlnumber >= 0);
      if (cc_method_.at(controlnumber) != rsj::CCmethod::kAbsolute)
         cc_low_.at(controlnumber) = 0;
      else
         cc_low_.at(controlnumber) = value < 0 || value >= cc_high_.at(controlnumber) ? 0 : value;
      current_v_.at(controlnumber).store(CenterCc(controlnumber), std::memory_order_release);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void ChannelModel::SetPwMax(const int value) noexcept
{
   pitch_wheel_max_ = value > kMaxNrpn || value <= pitch_wheel_min_ ? kMaxNrpn : value;
   pitch_wheel_current_.store(CenterPw(), std::memory_order_release);
}

void ChannelModel::SetPwMin(const int value) noexcept
{
   pitch_wheel_min_ = value < 0 || value >= pitch_wheel_max_ ? 0 : value;
   pitch_wheel_current_.store(CenterPw(), std::memory_order_release);
}

void ChannelModel::ActiveToSaved() const
{
   try {
      settings_to_save_.clear();
      for (auto i {0}; i <= kMaxMidi; ++i)
         if (cc_method_.at(i) != rsj::CCmethod::kAbsolute || cc_high_.at(i) != kMaxMidi
             || cc_low_.at(i) != 0)
            settings_to_save_.emplace_back(i, cc_low_.at(i), cc_high_.at(i), cc_method_.at(i));
      for (auto i {kMaxMidi + 1}; i <= kMaxNrpn; ++i)
         if (cc_method_.at(i) != rsj::CCmethod::kAbsolute || cc_high_.at(i) != kMaxNrpn
             || cc_low_.at(i) != 0)
            settings_to_save_.emplace_back(i, cc_low_.at(i), cc_high_.at(i), cc_method_.at(i));
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void ChannelModel::CcDefaults()
{
   try {
      /* atomics relaxed as this does not occur concurrently with other actions */
      cc_low_.fill(0);
      /* XCode throws linker error when use ChannelModel::kMaxNRPN here */
      cc_high_.fill(0x3FFF);
      cc_method_.fill(rsj::CCmethod::kAbsolute);
      for (auto&& a : current_v_) a.store(kMaxNrpnHalf, std::memory_order_relaxed);
      for (size_t a {0}; a <= kMaxMidi; ++a) {
         cc_high_.at(a) = kMaxMidi;
         current_v_.at(a).store(kMaxMidiHalf, std::memory_order_relaxed);
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void ChannelModel::SavedToActive()
{
   try {
      CcDefaults();
      for (const auto& set : settings_to_save_)
         SetCc(set.control_number, set.low, set.high, set.method);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

#pragma warning(push)
#pragma warning(disable : 26455)
ChannelModel::ChannelModel()
{
   try {
      CcDefaults();
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}
#pragma warning(pop)