#ifndef MIDI2LR_CONTROLSMODEL_H_INCLUDED
#define MIDI2LR_CONTROLSMODEL_H_INCLUDED
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
//-V813_MINSIZE=13 /*warn if passing structure by value > 12 bytes (3*sizeof(int)) */
#include <array>
#include <atomic>
#include <exception>
#include <vector>

#include <cereal/access.hpp>
#include <cereal/types/array.hpp> //ReSharper false alarm
#include <cereal/types/vector.hpp>
#include <gsl/gsl>

#include "MidiUtilities.h"
#include "Misc.h"

namespace rsj {
   enum struct CCmethod : char { kAbsolute, kTwosComplement, kBinaryOffset, kSignMagnitude };

   struct SettingsStruct {
      int control_number {};
      int low {};
      int high {};
      rsj::CCmethod method {};

      // ReSharper disable once CppNonExplicitConvertingConstructor
      SettingsStruct(int n = 0, int l = 0, int h = 0x7F,
          rsj::CCmethod m = rsj::CCmethod::kAbsolute) noexcept
          : control_number {n}, low {l}, high {h}, method {m}
      {
      }

      template<class Archive,
          cereal::traits::DisableIf<cereal::traits::is_text_archive<Archive>::value> =
              cereal::traits::sfinae>
      void serialize(Archive& archive, const uint32_t version)
      {
         switch (version) {
         case 1:
            archive(control_number, high, low, method);
            break;
         default:
            {
               constexpr auto msg {
                   "The file, 'settings.xml', is marked as a version not supported by the current "
                   "version of MIDI2LR SettingsStruct, and won't be loaded. File version: {}."};
               rsj::LogAndAlertError(fmt::format(juce::translate(msg).toStdString(), version),
                   fmt::format(msg, version), std::source_location::current());
            }
            break;
         }
      }

      template<class Archive,
          cereal::traits::EnableIf<cereal::traits::is_text_archive<Archive>::value> =
              cereal::traits::sfinae>
      void serialize(Archive& archive, const uint32_t version)
      {
         try {
            switch (version) {
            case 1:
               {
                  std::string methodstr {"undefined"};
                  switch (method) {
                  case CCmethod::kAbsolute:
                     methodstr = "Absolute";
                     break;
                  case CCmethod::kBinaryOffset:
                     methodstr = "BinaryOffset";
                     break;
                  case CCmethod::kSignMagnitude:
                     methodstr = "SignMagnitude";
                     break;
                  case CCmethod::kTwosComplement:
                     methodstr = "TwosComplement";
                     break;
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
               {
                  constexpr auto msg {"The file, 'settings.xml', is marked as a version not "
                                      "supported by the current version of MIDI2LR SettingsStruct, "
                                      "and won't be loaded. File version: {}."};
                  rsj::LogAndAlertError(fmt::format(juce::translate(msg).toStdString(), version),
                      fmt::format(msg, version), std::source_location::current());
               }
               break;
            }
         }
         catch (const std::exception& e) {
            rsj::ExceptionResponse(e, std::source_location::current());
            throw;
         }
      }
   };
} // namespace rsj

class ChannelModel {
   static constexpr int kBit14 {0x2000};
   static constexpr int kBit7 {0x40};
   static constexpr int kLow13Bits {0x1FFF};
   static constexpr int kLow6Bits {0x3F};
   static constexpr int kMaxMidi {0x7F};
   static constexpr int kMaxMidiHalf {kMaxMidi / 2};
   static constexpr int kMaxNrpn {0x3FFF};
   static constexpr int kMaxNrpnHalf {kMaxNrpn / 2};
   static constexpr size_t kMaxControls {0x4000};

 public:
   ChannelModel() noexcept { CcDefaults(); }

   double ControllerToPlugin(rsj::MessageType controltype, int controlnumber, int value, bool wrap);
   int MeasureChange(rsj::MessageType controltype, int controlnumber, int value);
   int SetToCenter(rsj::MessageType controltype, int controlnumber);

   [[nodiscard]] rsj::CCmethod GetCcMethod(int controlnumber) const
   {
      return cc_method_.at(controlnumber);
   }

   [[nodiscard]] int GetCcMax(int controlnumber) const { return cc_high_.at(controlnumber); }

   [[nodiscard]] int GetCcMin(int controlnumber) const { return cc_low_.at(controlnumber); }

   [[nodiscard]] int GetPwMax() const noexcept { return pitch_wheel_max_; }

   [[nodiscard]] int GetPwMin() const noexcept { return pitch_wheel_min_; }

   int PluginToController(rsj::MessageType controltype, int controlnumber, double value);
   void SetCc(int controlnumber, int min, int max, rsj::CCmethod controltype);
   void SetCcAll(int controlnumber, int min, int max, rsj::CCmethod controltype);
   void SetCcMax(int controlnumber, int value);

   void SetCcMethod(int controlnumber, rsj::CCmethod value)
   {
      cc_method_.at(controlnumber) = value;
   }

   void SetCcMin(int controlnumber, int value);
   void SetPwMax(int value) noexcept;
   void SetPwMin(int value) noexcept;

 private:
   friend class cereal::access;

   [[nodiscard]] int CenterCc(int controlnumber) const noexcept
   {
      return (cc_high_.at(controlnumber) - cc_low_.at(controlnumber)) / 2
             + cc_low_.at(controlnumber)
             + (cc_high_.at(controlnumber) - cc_low_.at(controlnumber)) % 2;
   }

   [[nodiscard]] int CenterPw() const noexcept
   {
      return (pitch_wheel_max_ - pitch_wheel_min_) / 2 + pitch_wheel_min_
             + (pitch_wheel_max_ - pitch_wheel_min_) % 2;
   }

   // ReSharper disable once CppMemberFunctionMayBeStatic
   [[nodiscard]] bool IsNrpn(int controlnumber) const noexcept
   {
      Expects(controlnumber <= kMaxNrpn && controlnumber >= 0);
      return controlnumber > kMaxMidi;
   }

   double OffsetResult(int diff, int controlnumber, bool wrap);
   void ActiveToSaved() const;
   void CcDefaults() noexcept;
   void SavedToActive();
   void SaveSettings(int start, int end, int maxVal) const;
   // ReSharper disable CppConstParameterInDeclaration
   template<class Archive> void load(Archive& archive, const uint32_t version);
   template<class Archive> void save(Archive& archive, const uint32_t version) const;
   // ReSharper restore CppConstParameterInDeclaration

   mutable std::vector<rsj::SettingsStruct> settings_to_save_ {};
   int pitch_wheel_max_ {kMaxNrpn};
   int pitch_wheel_min_ {0};
   std::atomic<int> pitch_wheel_current_ {kMaxNrpnHalf};
   std::array<rsj::CCmethod, kMaxControls> cc_method_ {};
   std::array<int, kMaxControls> cc_high_ {};
   std::array<int, kMaxControls> cc_low_ {};
#ifdef __cpp_lib_atomic_ref
   std::array<int, kMaxControls> current_v_ {};
#else
   std::array<std::atomic<int>, kMaxControls> current_v_ {};
#endif
};

class ControlsModel {
 public:
   double ControllerToPlugin(rsj::MidiMessage mm, bool wrap)
   {
      return all_controls_.at(mm.channel)
          .ControllerToPlugin(mm.message_type_byte, mm.control_number, mm.value, wrap);
   }

   int MeasureChange(rsj::MidiMessage mm)
   {
      return all_controls_.at(mm.channel)
          .MeasureChange(mm.message_type_byte, mm.control_number, mm.value);
   }

   int SetToCenter(rsj::MidiMessageId mm)
   {
      /* MidiMessageId channel is 1-based */
      return all_controls_.at(gsl::narrow_cast<size_t>(mm.channel) - 1)
          .SetToCenter(mm.msg_id_type, mm.control_number);
   }

   [[nodiscard]] rsj::CCmethod GetCcMethod(int channel, int controlnumber) const
   {
      return all_controls_.at(channel).GetCcMethod(controlnumber);
   }

   [[nodiscard]] rsj::CCmethod GetCcMethod(rsj::MidiMessageId msg_id) const
   {
      /* MidiMessageId channel is 1-based */
      return all_controls_.at(gsl::narrow_cast<size_t>(msg_id.channel) - 1)
          .GetCcMethod(msg_id.control_number);
   }

   [[nodiscard]] int GetCcMax(int channel, int controlnumber) const
   {
      return all_controls_.at(channel).GetCcMax(controlnumber);
   }

   int GetCcMin(int channel, int controlnumber) const
   {
      return all_controls_.at(channel).GetCcMin(controlnumber);
   }

   [[nodiscard]] int GetPwMax(int channel) const { return all_controls_.at(channel).GetPwMax(); }

   [[nodiscard]] int GetPwMin(int channel) const { return all_controls_.at(channel).GetPwMin(); }

   int PluginToController(rsj::MidiMessageId msg_id, double value)
   {
      /* msg_id is one-based */
      return all_controls_.at(gsl::narrow_cast<size_t>(msg_id.channel) - 1)
          .PluginToController(msg_id.msg_id_type, msg_id.control_number, value);
   }

   int MeasureChange(rsj::MessageType controltype, int channel, int controlnumber, int value)
   {
      return all_controls_.at(channel).MeasureChange(controltype, controlnumber, value);
   }

   void SetCc(int channel, int controlnumber, int min, int max, rsj::CCmethod controltype)
   {
      all_controls_.at(channel).SetCc(controlnumber, min, max, controltype);
   }

   void SetCcAll(int channel, int controlnumber, int min, int max, rsj::CCmethod controltype)
   {
      all_controls_.at(channel).SetCcAll(controlnumber, min, max, controltype);
   }

   void SetCcMax(int channel, int controlnumber, int value)
   {
      all_controls_.at(channel).SetCcMax(controlnumber, value);
   }

   void SetCcMethod(int channel, int controlnumber, rsj::CCmethod value)
   {
      all_controls_.at(channel).SetCcMethod(controlnumber, value);
   }

   void SetCcMin(int channel, int controlnumber, int value)
   {
      all_controls_.at(channel).SetCcMin(controlnumber, value);
   }

   void SetPwMax(int channel, int value) { all_controls_.at(channel).SetPwMax(value); }

   void SetPwMin(int channel, int value) { all_controls_.at(channel).SetPwMin(value); }

 private:
   friend class cereal::access;

   template<class Archive> void serialize(Archive& archive, const uint32_t version)
   {
      try {
         if (version == 1) { archive(all_controls_); }
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(e, std::source_location::current());
         throw;
      }
   }

   std::array<ChannelModel, 16> all_controls_;
};

template<class Archive> void ChannelModel::load(Archive& archive, const uint32_t version)
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
         {
            constexpr auto msg {
                "The file, 'settings.xml', is marked as a version not supported by the current "
                "version of MIDI2LR ChannelModel, and won't be loaded. File version: {}."};
            rsj::LogAndAlertError(fmt::format(juce::translate(msg).toStdString(), version),
                fmt::format(msg, version), std::source_location::current());
         }
         break;
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

template<class Archive> void ChannelModel::save(Archive& archive, const uint32_t version) const
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
         {
            constexpr auto msg {
                "The file, 'settings.xml', is marked as a version not supported by the current "
                "version of MIDI2LR ChannelModel, and won't be loaded. File version: {}."};
            rsj::LogAndAlertError(fmt::format(juce::translate(msg).toStdString(), version),
                fmt::format(msg, version), std::source_location::current());
         }
         break;
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

#pragma warning(push)
#pragma warning(disable : 26426 26440 26444)
CEREAL_CLASS_VERSION(ChannelModel, 3)        //-V837
CEREAL_CLASS_VERSION(ControlsModel, 1)       //-V837
CEREAL_CLASS_VERSION(rsj::SettingsStruct, 1) //-V837
#pragma warning(pop)
#endif
