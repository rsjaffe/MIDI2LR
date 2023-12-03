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
#include <stdexcept>

double ChannelModel::OffsetResult(const int diff, const int controlnumber, const bool wrap)
{
   try {
      const auto high_limit {cc_high_.at(controlnumber)};
      Expects(diff <= high_limit && diff >= -high_limit);
#ifdef __cpp_lib_atomic_ref
      const std::atomic_ref cv {current_v_.at(controlnumber)};
#else
      auto& cv {current_v_.at(controlnumber)};
#endif
      auto old_v {cv.load(std::memory_order_acquire)};
      int new_v {};
      if (wrap) {
         do {
            new_v = old_v + diff;
            if (new_v > high_limit) { new_v -= high_limit; }
            else if (new_v < 0) {
               new_v += high_limit;
            }
            else { /* no action needed */
            }
         } while (!cv.compare_exchange_weak(old_v, new_v, std::memory_order_release,
             std::memory_order_acquire));
      }
      else [[likely]] {
         do {
            new_v = std::clamp(old_v + diff, 0, high_limit);
         } while (!cv.compare_exchange_weak(old_v, new_v, std::memory_order_release,
             std::memory_order_acquire));
      }
      return static_cast<double>(new_v) / static_cast<double>(high_limit);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

double ChannelModel::ControllerToPlugin(const rsj::MessageType controltype, const int controlnumber,
    const int value, const bool wrap)
{
   try {
      Expects(controltype == rsj::MessageType::kCc
                      && cc_method_.at(controlnumber) == rsj::CCmethod::kAbsolute
                  ? cc_low_.at(controlnumber) < cc_high_.at(controlnumber)
                  : 1);
      Expects(controltype == rsj::MessageType::kPw ? pitch_wheel_max_ > pitch_wheel_min_ : 1);
      Expects(controltype == rsj::MessageType::kPw
                  ? value >= pitch_wheel_min_ && value <= pitch_wheel_max_
                  : 1);
      /* note that the value is not msb,lsb, but rather the calculated value. Since lsb is only 7
       * bits, high bits are shifted one right when placed into int. */
      switch (controltype) {
      case rsj::MessageType::kPw:
         pitch_wheel_current_.store(value, std::memory_order_release);
#pragma warning(suppress : 26451) /* int subtraction won't overflow 4 bytes here */
         return static_cast<double>(value - pitch_wheel_min_)
                / static_cast<double>(pitch_wheel_max_ - pitch_wheel_min_);
      case rsj::MessageType::kCc:
         switch (cc_method_.at(controlnumber)) {
         case rsj::CCmethod::kAbsolute:
#ifdef __cpp_lib_atomic_ref
            std::atomic_ref(current_v_.at(controlnumber)).store(value, std::memory_order_release);
#else
            current_v_.at(controlnumber).store(value, std::memory_order_release);
#endif
#pragma warning(suppress : 26451) /* int subtraction won't overflow 4 bytes here */
            return static_cast<double>(value - cc_low_.at(controlnumber))
                   / static_cast<double>(cc_high_.at(controlnumber) - cc_low_.at(controlnumber));
         case rsj::CCmethod::kBinaryOffset:
            if (IsNrpn(controlnumber)) { return OffsetResult(value - kBit14, controlnumber, wrap); }
            return OffsetResult(value - kBit7, controlnumber, wrap);
         case rsj::CCmethod::kSignMagnitude:
            if (IsNrpn(controlnumber)) {
               return OffsetResult(value & kBit14 ? -(value & kLow13Bits) : value, controlnumber,
                   wrap);
            }
            return OffsetResult(value & kBit7 ? -(value & kLow6Bits) : value, controlnumber, wrap);
         case rsj::CCmethod::kTwosComplement:
            /* SEE:https://en.wikipedia.org/wiki/Signed_number_representations#Two.27s_complement
             * flip twos comp and subtract--independent of processor architecture */
            if (IsNrpn(controlnumber)) {
               return OffsetResult(value & kBit14 ? -((value ^ kMaxNrpn) + 1) : value,
                   controlnumber, wrap);
            }
            return OffsetResult(value & kBit7 ? -((value ^ kMaxMidi) + 1) : value, controlnumber,
                wrap);
         }
      case rsj::MessageType::kNoteOn:
         return static_cast<double>(value)
                / static_cast<double>((IsNrpn(controlnumber) ? kMaxNrpn : kMaxMidi));
      case rsj::MessageType::kNoteOff:
         return 0.0;
      case rsj::MessageType::kChanPressure:
      case rsj::MessageType::kKeyPressure:
      case rsj::MessageType::kPgmChange:
      case rsj::MessageType::kSystem:
         throw std::invalid_argument(fmt::format(
             FMT_STRING("ChannelModel::ControllerToPlugin unexpected control type. Controltype {}, "
                        "controlnumber {}, value {}, wrap {}."),
             controltype, controlnumber, value, wrap));
      }
      throw std::domain_error(fmt::format(FMT_STRING("Undefined control type in "
                                                     "ChannelModel::PluginToController. "
                                                     "Control type {}."),
          controltype));
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
      case rsj::MessageType::kPw:
         retval = CenterPw();
         pitch_wheel_current_.store(retval, std::memory_order_release);
         break;
      case rsj::MessageType::kCc:
         if (cc_method_.at(controlnumber) == rsj::CCmethod::kAbsolute) {
            retval = CenterCc(controlnumber);
#ifdef __cpp_lib_atomic_ref
            std::atomic_ref(current_v_.at(controlnumber)).store(retval, std::memory_order_release);
#else
            current_v_.at(controlnumber).store(retval, std::memory_order_release);
#endif
         }
         break;
      case rsj::MessageType::kChanPressure:
      case rsj::MessageType::kKeyPressure:
      case rsj::MessageType::kNoteOff:
      case rsj::MessageType::kNoteOn:
      case rsj::MessageType::kPgmChange:
      case rsj::MessageType::kSystem:
         break;
      }
      return retval;
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

int ChannelModel::MeasureChange(const rsj::MessageType controltype, const int controlnumber,
    const int value)
{
   try {
      Expects(controltype == rsj::MessageType::kCc
                      && cc_method_.at(controlnumber) == rsj::CCmethod::kAbsolute
                  ? cc_low_.at(controlnumber) < cc_high_.at(controlnumber)
                  : 1);
      Expects(controltype == rsj::MessageType::kPw ? pitch_wheel_max_ > pitch_wheel_min_ : 1);
      Expects(controltype == rsj::MessageType::kPw
                  ? value >= pitch_wheel_min_ && value <= pitch_wheel_max_
                  : 1);
      /* note that the value is not msb,lsb, but rather the calculated value. Since lsb is only 7
       * bits, high bits are shifted one right when placed into int. */
      switch (controltype) {
      case rsj::MessageType::kPw:
         return value - pitch_wheel_current_.exchange(value, std::memory_order_acq_rel);
      case rsj::MessageType::kCc:
         switch (cc_method_.at(controlnumber)) {
         case rsj::CCmethod::kAbsolute:
#ifdef __cpp_lib_atomic_ref
            return value
                   - std::atomic_ref(current_v_.at(controlnumber))
                         .exchange(value, std::memory_order_acq_rel);
#else
            return value - current_v_.at(controlnumber).exchange(value, std::memory_order_acq_rel);
#endif
         case rsj::CCmethod::kBinaryOffset:
            if (IsNrpn(controlnumber)) { return value - kBit14; }
            return value - kBit7;
         case rsj::CCmethod::kSignMagnitude:
            if (IsNrpn(controlnumber)) { return value & kBit14 ? -(value & kLow13Bits) : value; }
            return value & kBit7 ? -(value & kLow6Bits) : value;
         case rsj::CCmethod::kTwosComplement:
            /* SEE:https://en.wikipedia.org/wiki/Signed_number_representations#Two.27s_complement
             * flip twos comp and subtract--independent of processor architecture */
            if (IsNrpn(controlnumber)) {
               return value & kBit14 ? -((value ^ kMaxNrpn) + 1) : value;
            }
            return value & kBit7 ? -((value ^ kMaxMidi) + 1) : value;
         }
      case rsj::MessageType::kNoteOff:
      case rsj::MessageType::kNoteOn:
         return int {0};
      case rsj::MessageType::kChanPressure:
      case rsj::MessageType::kKeyPressure:
      case rsj::MessageType::kPgmChange:
      case rsj::MessageType::kSystem:
         throw std::invalid_argument(
             fmt::format(FMT_STRING("ChannelModel::MeasureChange unexpected control type. "
                                    "Controltype {}, controlnumber {}, value {}."),
                 controltype, controlnumber, value));
      }
      throw std::domain_error(fmt::format(FMT_STRING("Undefined control type in "
                                                     "ChannelModel::PluginToController. "
                                                     "Control type {}."),
          controltype));
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

#pragma warning(push)
#pragma warning(disable : 26451) /* see TODO below */

int ChannelModel::PluginToController(const rsj::MessageType controltype, const int controlnumber,
    const double value)
{
   try {
      /* value effectively clamped to 0-1 by clamp calls below */
      switch (controltype) {
      case rsj::MessageType::kPw:
         {
            /* TODO(C26451): int subtraction: can it overflow? */
            const auto newv {
                std::clamp(gsl::narrow<int>(std::lrint(
                               value * static_cast<double>(pitch_wheel_max_ - pitch_wheel_min_)))
                               + pitch_wheel_min_,
                    pitch_wheel_min_, pitch_wheel_max_)};
            pitch_wheel_current_.store(newv, std::memory_order_release);
            return newv;
         }
      case rsj::MessageType::kCc:
         {
            /* TODO(C26451): int subtraction: can it overflow? */
            const auto clow {cc_low_.at(controlnumber)};
            const auto chigh {cc_high_.at(controlnumber)};
#ifdef _WIN32
            const auto newv {
                std::clamp(_cvt_dtoi_fast(value * static_cast<double>(chigh - clow) + 0.5) + clow,
                    clow, chigh)};
#else
            const auto newv {std::clamp(
                gsl::narrow_cast<int>(value * static_cast<double>(chigh - clow) + 0.5) + clow, clow,
                chigh)};
#endif
#ifdef __cpp_lib_atomic_ref
            std::atomic_ref(current_v_.at(controlnumber)).store(newv, std::memory_order_release);
#else
            current_v_.at(controlnumber).store(newv, std::memory_order_release);
#endif
            return newv;
         }
      case rsj::MessageType::kNoteOn:
         return kMaxMidi;
      case rsj::MessageType::kChanPressure:
      case rsj::MessageType::kKeyPressure:
      case rsj::MessageType::kNoteOff:
      case rsj::MessageType::kPgmChange:
      case rsj::MessageType::kSystem:
         throw std::invalid_argument(fmt::format(FMT_STRING("Unexpected control type in "
                                                            "ChannelModel::PluginToController. "
                                                            "Control type {}."),
             controltype));
      }
      throw std::domain_error(fmt::format(FMT_STRING("Undefined control type in "
                                                     "ChannelModel::PluginToController. "
                                                     "Control type {}."),
          controltype));
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

#pragma warning(pop)

void ChannelModel::SetCc(const int controlnumber, const int min, const int max,
    const rsj::CCmethod controltype)
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

void ChannelModel::SetCcAll(const int controlnumber, const int min, const int max,
    const rsj::CCmethod controltype)
{
   try {
      if (IsNrpn(controlnumber)) {
         for (auto a {kMaxMidi + 1}; a <= kMaxNrpn; ++a) { SetCc(a, min, max, controltype); }
      }
      else {
         for (auto a {0}; a <= kMaxMidi; ++a) { SetCc(a, min, max, controltype); }
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void ChannelModel::SetCcMax(const int controlnumber, const int value)
{
   try {
      Expects(value <= kMaxNrpn && value >= 0);
      if (cc_method_.at(controlnumber) != rsj::CCmethod::kAbsolute) {
         cc_high_.at(controlnumber) = value < 0 ? 1000 : value;
      }
      else {
         const auto max {IsNrpn(controlnumber) ? kMaxNrpn : kMaxMidi};
         cc_high_.at(controlnumber) =
             value <= cc_low_.at(controlnumber) || value > max ? max : value;
      }
#ifdef __cpp_lib_atomic_ref
      std::atomic_ref(current_v_.at(controlnumber))
          .store(CenterCc(controlnumber), std::memory_order_release);
#else
      current_v_.at(controlnumber).store(CenterCc(controlnumber), std::memory_order_release);
#endif
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void ChannelModel::SetCcMin(const int controlnumber, const int value)
{
   try {
      if (cc_method_.at(controlnumber) != rsj::CCmethod::kAbsolute) {
         cc_low_.at(controlnumber) = 0;
      }
      else {
         cc_low_.at(controlnumber) = value < 0 || value >= cc_high_.at(controlnumber) ? 0 : value;
      }
#ifdef __cpp_lib_atomic_ref
      std::atomic_ref(current_v_.at(controlnumber))
          .store(CenterCc(controlnumber), std::memory_order_release);
#else
      current_v_.at(controlnumber).store(CenterCc(controlnumber), std::memory_order_release);
#endif
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
      for (auto i {0}; i <= kMaxMidi; ++i) {
         if (cc_method_.at(i) != rsj::CCmethod::kAbsolute || cc_high_.at(i) != kMaxMidi
             || cc_low_.at(i) != 0) {
            settings_to_save_.emplace_back(i, cc_low_.at(i), cc_high_.at(i), cc_method_.at(i));
         }
      }
      for (auto i {kMaxMidi + 1}; i <= kMaxNrpn; ++i) {
         if (cc_method_.at(i) != rsj::CCmethod::kAbsolute || cc_high_.at(i) != kMaxNrpn
             || cc_low_.at(i) != 0) {
            settings_to_save_.emplace_back(i, cc_low_.at(i), cc_high_.at(i), cc_method_.at(i));
         }
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void ChannelModel::CcDefaults()
{
   try {
      cc_low_.fill(0);
      cc_high_.fill(kMaxNrpn);
      cc_method_.fill(rsj::CCmethod::kAbsolute);
#ifdef __cpp_lib_atomic_ref
      current_v_.fill(kMaxNrpnHalf);
      std::fill_n(cc_high_.begin(), kMaxMidi + 1, kMaxMidi);
      std::fill_n(current_v_.begin(), kMaxMidi + 1, kMaxMidiHalf);
#else
      for (auto&& a : current_v_) { a.store(kMaxNrpnHalf, std::memory_order_relaxed); }
      std::fill_n(cc_high_.begin(), kMaxMidi + 1, kMaxMidi);
      for (size_t a {0}; a <= kMaxMidi; ++a) {
         current_v_.at(a).store(kMaxMidiHalf, std::memory_order_relaxed);
      }
#endif
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
      for (const auto& set : settings_to_save_) {
         SetCc(set.control_number, set.low, set.high, set.method);
      }
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