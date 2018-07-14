#pragma once
/*
==============================================================================

ControlsModel.h

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
#ifndef MIDI2LR_CONTROLSMODEL_H_INCLUDED
#define MIDI2LR_CONTROLSMODEL_H_INCLUDED

#include <array>
#include <atomic>
#include <vector>
#include <cereal/access.hpp>
#include <cereal/types/array.hpp> //required, ReSharper falsely marks as not needed
#include <cereal/types/vector.hpp>
#include <gsl/gsl>
#include "MidiUtilities.h"
#include "Misc.h"

namespace rsj {
   enum struct CCmethod : char { kAbsolute, kTwosComplement, kBinaryOffset, kSignMagnitude };

   struct SettingsStruct {
      short number; // not using size_t so serialized data won't vary if size_t varies
      short low;
      short high;
      rsj::CCmethod method;
      SettingsStruct(short n = 0, short l = 0, short h = 0x7F,
          rsj::CCmethod m = rsj::CCmethod::kAbsolute) noexcept
          : number{n}, low{l}, high{h}, method{m}
      {
      }

      template<class Archive,
          cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value> =
              cereal::traits::sfinae>
      void serialize(Archive& archive, uint32_t const version)
      {
         switch (version) {
         case 1:
            archive(number, high, low, method);
            break;
         default:
            rsj::LogAndAlertError("Wrong archive number for SettingsStruct");
         }
      }

      template<class Archive,
          cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value> =
              cereal::traits::sfinae>
      void serialize(Archive& archive, uint32_t const version)
      {
         switch (version) {
         case 1: {
            std::string methodstr{"undefined"};
            switch (method) {
            case CCmethod::kAbsolute:
               methodstr = "Absolute";
               break;
            case CCmethod::kBinaryOffset:
               methodstr = "BinaryOffset";
               break;
            case CCmethod::kSignMagnitude:
               methodstr = "SignMagnitute";
               break;
            case CCmethod::kTwosComplement:
               methodstr = "TwosComplement";
            default:
               break; // leave "undefined"
            }
            archive(cereal::make_nvp("CC", number), CEREAL_NVP(high), CEREAL_NVP(low),
                cereal::make_nvp("method", methodstr));
            switch (methodstr.front()) {
            case 'B':
               method = CCmethod::kBinaryOffset;
               break;
            case 'S':
               method = CCmethod::kSignMagnitude;
               break;
            case 'T':
               method = CCmethod::kTwosComplement;
               break;
            case 'A':
            default:
               method = CCmethod::kAbsolute;
               break;
            }
            break;
         }
         default:
            rsj::LogAndAlertError("Wrong archive number for SettingsStruct");
         }
      }
   };
} // namespace rsj

class ChannelModel {
   constexpr static short kBit14 = 0x2000;
   constexpr static short kBit7 = 0x40;
   constexpr static short kLow13Bits = 0x1FFF;
   constexpr static short kLow6Bits = 0x3F;
   constexpr static short kMaxMidi = 0x7F;
   constexpr static short kMaxMidiHalf = kMaxMidi / 2;
   constexpr static short kMaxNrpn = 0x3FFF;
   constexpr static short kMaxNrpnHalf = kMaxNrpn / 2;
   constexpr static size_t kMaxControls = 0x4000;
   constexpr static rsj::TimeType kUpdateDelay = 250;

 public:
   ChannelModel() noexcept;
   ~ChannelModel() = default;
   // Can write copy and move with special handling for atomics, but in lieu of that, delete
   ChannelModel(const ChannelModel&) = delete; // can't copy atomics
   ChannelModel& operator=(const ChannelModel&) = delete;
   ChannelModel(ChannelModel&&) = delete; // can't move atomics
   ChannelModel& operator=(ChannelModel&&) = delete;
   double ControllerToPlugin(short controltype, size_t controlnumber, short value);
   short MeasureChange(short controltype, size_t controlnumber, short value);
   short SetToCenter(short controltype, size_t controlnumber);
   [[nodiscard]] rsj::CCmethod GetCcMethod(size_t controlnumber) const
   {
      try {
         return cc_method_.at(controlnumber);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }
   [[nodiscard]] short GetCcMax(size_t controlnumber) const
   {
      try {
         return cc_high_.at(controlnumber);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }
   [[nodiscard]] short GetCcMin(size_t controlnumber) const
   {
      try {
         return cc_low_.at(controlnumber);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   [[nodiscard]] short GetPwMax() const noexcept
   {
      return pitch_wheel_max_;
   }
   [[nodiscard]] short GetPwMin() const noexcept
   {
      return pitch_wheel_min_;
   }
   short PluginToController(short controltype, size_t controlnumber, double value);
   void SetCc(size_t controlnumber, short min, short max, rsj::CCmethod controltype);
   void SetCcAll(size_t controlnumber, short min, short max, rsj::CCmethod controltype);
   void SetCcMax(size_t controlnumber, short value);
   void SetCcMethod(size_t controlnumber, rsj::CCmethod value)
   {
      try {
         cc_method_.at(controlnumber) = value;
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }
   void SetCcMin(size_t controlnumber, short value);
   void SetPwMax(short value) noexcept(kNdebug);
   void SetPwMin(short value) noexcept(kNdebug);

 private:
   [[nodiscard]] short CenterCc(size_t controlnumber) const noexcept
   {
      return (cc_high_.at(controlnumber) - cc_low_.at(controlnumber)) / 2
             + cc_low_.at(controlnumber)
             + (cc_high_.at(controlnumber) - cc_low_.at(controlnumber)) % 2;
   }
   [[nodiscard]] short CenterPw() const noexcept
   {
      return (pitch_wheel_max_ - pitch_wheel_min_) / 2 + pitch_wheel_min_
             + (pitch_wheel_max_ - pitch_wheel_min_) % 2;
   }
   friend class cereal::access;
   [[nodiscard]] bool IsNRPN_(size_t controlnumber) const noexcept(kNdebug)
   {
      Expects(controlnumber <= kMaxNrpn);
      return controlnumber > kMaxMidi;
   }
   double OffsetResult(short diff, size_t controlnumber);
   mutable rsj::RelaxTTasSpinLock current_v_mtx_;
   mutable std::vector<rsj::SettingsStruct> settings_to_save_{};
   short pitch_wheel_max_{kMaxNrpn};
   short pitch_wheel_min_{0};
   std::atomic<short> pitch_wheel_current_{0};
   std::array<rsj::CCmethod, kMaxControls> cc_method_{};
   std::array<short, kMaxControls> cc_high_{};
   std::array<short, kMaxControls> cc_low_{};
   std::array<short, kMaxControls> current_v_{};
   template<class Archive> void load(Archive& archive, uint32_t const version);
   template<class Archive> void save(Archive& archive, uint32_t const version) const;
   void ActiveToSaved() const;
   void CcDefaults();
   void SavedToActive();
};

class ControlsModel {
 public:
   ControlsModel() = default;
   ~ControlsModel() = default;
   // Can write copy and move with special handling for atomics, but in lieu of that, delete
   ControlsModel(const ControlsModel&) = delete; // can't copy atomics
   ControlsModel& operator=(const ControlsModel&) = delete;
   ControlsModel(ControlsModel&&) = delete; // can't move atomics
   ControlsModel& operator=(ControlsModel&&) = delete;
   double ControllerToPlugin(const rsj::MidiMessage& mm)
   {
      try {
         return all_controls_.at(mm.channel)
             .ControllerToPlugin(mm.message_type_byte, mm.number, mm.value);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   short MeasureChange(const rsj::MidiMessage& mm)
   {
      try {
         return all_controls_.at(mm.channel)
             .MeasureChange(mm.message_type_byte, mm.number, mm.value);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }
   short SetToCenter(const rsj::MidiMessage& mm)
   {
      try {
         return all_controls_.at(mm.channel).SetToCenter(mm.message_type_byte, mm.number);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   [[nodiscard]] rsj::CCmethod GetCcMethod(size_t channel, short controlnumber) const
   {
      try {
         return all_controls_.at(channel).GetCcMethod(controlnumber);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   [[nodiscard]] short GetCcMax(size_t channel, short controlnumber) const
   {
      try {
         return all_controls_.at(channel).GetCcMax(controlnumber);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   short GetCcMin(size_t channel, short controlnumber)
   {
      try {
         return all_controls_.at(channel).GetCcMin(controlnumber);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   [[nodiscard]] short GetPwMax(size_t channel) const
   {
      try {
         return all_controls_.at(channel).GetPwMax();
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   [[nodiscard]] short GetPwMin(size_t channel) const
   {
      try {
         return all_controls_.at(channel).GetPwMin();
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   short PluginToController(short controltype, size_t channel, short controlnumber, double value)
   {
      try {
         return all_controls_.at(channel).PluginToController(controltype, controlnumber, value);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   short MeasureChange(short controltype, size_t channel, short controlnumber, short value)
   {
      try {
         return all_controls_.at(channel).MeasureChange(controltype, controlnumber, value);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   void SetCc(size_t channel, short controlnumber, short min, short max, rsj::CCmethod controltype)
   {
      try {
         all_controls_.at(channel).SetCc(controlnumber, min, max, controltype);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }
   void SetCcAll(
       size_t channel, short controlnumber, short min, short max, rsj::CCmethod controltype)
   {
      try {
         all_controls_.at(channel).SetCcAll(controlnumber, min, max, controltype);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   void SetCcMax(size_t channel, short controlnumber, short value)
   {
      try {
         all_controls_.at(channel).SetCcMax(controlnumber, value);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   void SetCcMethod(size_t channel, short controlnumber, rsj::CCmethod value)
   {
      try {
         all_controls_.at(channel).SetCcMethod(controlnumber, value);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   void SetCcMin(size_t channel, short controlnumber, short value)
   {
      try {
         all_controls_.at(channel).SetCcMin(controlnumber, value);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   void SetPwMax(size_t channel, short value)
   {
      try {
         all_controls_.at(channel).SetPwMax(value);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   void SetPwMin(size_t channel, short value)
   {
      try {
         all_controls_.at(channel).SetPwMin(value);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

 private:
   friend class cereal::access;
   template<class Archive> void serialize(Archive& archive, uint32_t const version)
   {
      if (version == 1) // serialize things by passing them to the archive
         archive(all_controls_);
   }
   std::array<ChannelModel, 16> all_controls_;
};

template<class Archive> void ChannelModel::load(Archive& archive, uint32_t const version)
{
   try {
      switch (version) {
      case 1:
         archive(cc_method_, cc_high_, cc_low_, pitch_wheel_max_, pitch_wheel_min_);
         break;
      case 2:
         archive(settings_to_save_);
         SavedToActive();
         break;
      case 3:
         archive(settings_to_save_, cereal::make_nvp("PWmax", pitch_wheel_max_),
             cereal::make_nvp("PWmin", pitch_wheel_min_));
         SavedToActive();
         break;
      default:
         rsj::LogAndAlertError("Archive version not acceptable");
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

template<class Archive> void ChannelModel::save(Archive& archive, uint32_t const version) const
{
   try {
      switch (version) {
      case 1:
         archive(cc_method_, cc_high_, cc_low_, pitch_wheel_max_, pitch_wheel_min_);
         break;
      case 2:
         ActiveToSaved();
         archive(settings_to_save_);
         break;
      case 3:
         ActiveToSaved();
         archive(settings_to_save_, cereal::make_nvp("PWmax", pitch_wheel_max_),
             cereal::make_nvp("PWmin", pitch_wheel_min_));
         break;
      default:
         rsj::LogAndAlertError("Wrong archive version specified for save");
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}
#pragma warning(push)
#pragma warning(disable : 26440 26426 26444)
CEREAL_CLASS_VERSION(ChannelModel, 3)
CEREAL_CLASS_VERSION(ControlsModel, 1)
CEREAL_CLASS_VERSION(rsj::SettingsStruct, 1)
#pragma warning(pop)
#endif