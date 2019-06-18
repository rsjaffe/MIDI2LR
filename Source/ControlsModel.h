#ifndef MIDI2LR_CONTROLSMODEL_H_INCLUDED
#define MIDI2LR_CONTROLSMODEL_H_INCLUDED
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
#include <array>
#include <atomic>
#include <exception>
#include <vector>

#include <cereal/access.hpp>
// ReSharper disable once CppUnusedIncludeDirective
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <gsl/gsl>
#include "Concurrency.h"
#include "MidiUtilities.h"
#include "Misc.h"

namespace rsj {
   enum struct CCmethod : char { kAbsolute, kTwosComplement, kBinaryOffset, kSignMagnitude };

   struct SettingsStruct {
      short control_number; // not using size_t so serialized data won't vary if size_t varies
      short low;
      short high;
      rsj::CCmethod method;
      // ReSharper disable once CppNonExplicitConvertingConstructor
      SettingsStruct(short n = 0, short l = 0, short h = 0x7F,
          rsj::CCmethod m = rsj::CCmethod::kAbsolute) noexcept
          : control_number{n}, low{l}, high{h}, method{m}
      {
      }

      template<class Archive,
          cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value> =
              cereal::traits::sfinae>
      void serialize(Archive& archive, uint32_t const version)
      {
         switch (version) {
         case 1:
            archive(control_number, high, low, method);
            break;
         default:
            rsj::LogAndAlertError("Wrong archive version for SettingsStruct. Version is "
                                  + juce::String(version) + '.');
         }
      }

      template<class Archive,
          cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value> =
              cereal::traits::sfinae>
      void serialize(Archive& archive, uint32_t const version)
      {
         try {
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
               archive(cereal::make_nvp("CC", control_number), CEREAL_NVP(high), CEREAL_NVP(low),
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
               rsj::LogAndAlertError("Wrong archive version for SettingsStruct. Version is "
                                     + juce::String(version) + '.');
            }
         }
         catch (const std::exception& e) {
            rsj::ExceptionResponse(typeid(this).name(), __func__, e);
            throw;
         }
      }
   };
} // namespace rsj

class ChannelModel {
   static constexpr short kBit14 = 0x2000;
   static constexpr short kBit7 = 0x40;
   static constexpr short kLow13Bits = 0x1FFF;
   static constexpr short kLow6Bits = 0x3F;
   static constexpr short kMaxMidi = 0x7F;
   static constexpr short kMaxMidiHalf = kMaxMidi / 2;
   static constexpr short kMaxNrpn = 0x3FFF;
   static constexpr short kMaxNrpnHalf = kMaxNrpn / 2;
   static constexpr size_t kMaxControls = 0x4000;

 public:
   ChannelModel();
   ~ChannelModel() = default;
   // Can write copy and move with special handling for atomics, but in lieu of that, delete
   ChannelModel(const ChannelModel&) = delete; // can't copy atomics
   ChannelModel& operator=(const ChannelModel&) = delete;
   ChannelModel(ChannelModel&&) = delete; // can't move atomics
   ChannelModel& operator=(ChannelModel&&) = delete;
   double ControllerToPlugin(rsj::MessageType controltype, size_t controlnumber, short value);
   short MeasureChange(rsj::MessageType controltype, size_t controlnumber, short value);
   short SetToCenter(rsj::MessageType controltype, size_t controlnumber);
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
   short PluginToController(rsj::MessageType controltype, size_t controlnumber, double value);
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
   void SetPwMax(short value) noexcept;
   void SetPwMin(short value) noexcept;

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
   // ReSharper disable once CppMemberFunctionMayBeStatic
   [[nodiscard]] bool IsNRPN_(size_t controlnumber) const noexcept(kNdebug)
   {
      Expects(controlnumber <= kMaxNrpn);
      return controlnumber > kMaxMidi;
   }
   double OffsetResult(short diff, size_t controlnumber);
   mutable rsj::SpinLock current_v_mtx_;
   mutable std::vector<rsj::SettingsStruct> settings_to_save_{};
   short pitch_wheel_max_{kMaxNrpn};
   short pitch_wheel_min_{0};
   std::atomic<short> pitch_wheel_current_{0};
   std::array<rsj::CCmethod, kMaxControls> cc_method_{};
   std::array<short, kMaxControls> cc_high_{};
   std::array<short, kMaxControls> cc_low_{};
   std::array<short, kMaxControls> current_v_{};
   // ReSharper disable CppConstParameterInDeclaration
   template<class Archive> void load(Archive& archive, uint32_t const version);
   template<class Archive> void save(Archive& archive, uint32_t const version) const;
   // ReSharper restore CppConstParameterInDeclaration
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
             .ControllerToPlugin(mm.message_type_byte, mm.control_number, mm.value);
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
             .MeasureChange(mm.message_type_byte, mm.control_number, mm.value);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }
   short SetToCenter(const rsj::MidiMessage& mm)
   {
      try {
         return all_controls_.at(mm.channel).SetToCenter(mm.message_type_byte, mm.control_number);
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

   [[nodiscard]] rsj::CCmethod GetCcMethod(rsj::MidiMessageId msg_id) const
   {
      try { // MidiMessageId channel is 1-based
         return all_controls_.at(static_cast<size_t>(msg_id.channel) - 1)
             .GetCcMethod(msg_id.control_number);
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

   short PluginToController(rsj::MidiMessageId msg_id, double value)
   {
      try { // msg_id is one-based
         return all_controls_.at(static_cast<size_t>(msg_id.channel) - 1)
             .PluginToController(msg_id.msg_id_type, msg_id.control_number, value);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   short MeasureChange(
       rsj::MessageType controltype, size_t channel, short controlnumber, short value)
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
         rsj::LogAndAlertError(
             "Wrong archive version for ChannelModel. Version is " + juce::String(version) + '.');
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
         rsj::LogAndAlertError(
             "Wrong archive version specified for saving ChannelModel. Version is "
             + juce::String(version) + '.');
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}
#pragma warning(push)
#pragma warning(disable : 26426 26440 26444)
CEREAL_CLASS_VERSION(ChannelModel, 3)
CEREAL_CLASS_VERSION(ControlsModel, 1)
CEREAL_CLASS_VERSION(rsj::SettingsStruct, 1)
#pragma warning(pop)
#endif