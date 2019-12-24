#ifndef MIDI2LR_PROFILEMANAGER_H_INCLUDED
#define MIDI2LR_PROFILEMANAGER_H_INCLUDED
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
#include <functional>
#include <vector>

#include <JuceLibraryCode/JuceHeader.h>
#ifndef _MSC_VER
#define _In_
#endif

class ControlsModel;
class LrIpcOut;
class MidiReceiver;
class Profile;
namespace rsj {
   struct MidiMessage;
   struct MidiMessageId;
} // namespace rsj

class ProfileManager final : juce::AsyncUpdater {
 public:
   ProfileManager(
       ControlsModel& c_model, const Profile& profile, LrIpcOut& out, MidiReceiver& midi_receiver);
   ~ProfileManager() = default;
   ProfileManager(const ProfileManager& other) = delete;
   ProfileManager(ProfileManager&& other) = delete;
   ProfileManager& operator=(const ProfileManager& other) = delete;
   ProfileManager& operator=(ProfileManager&& other) = delete;
   template<class T>
   void AddCallback(
       _In_ T* const object, _In_ void (T::*const mf)(juce::XmlElement*, const juce::String&))
   {
      using namespace std::placeholders;
      if (object && mf)
         /* only store non-empty functions */
         callbacks_.emplace_back(std::bind(mf, object, _1, _2));
   }
   [[nodiscard]] const juce::String& GetProfileDirectory() const noexcept
   {
      return profile_location_.getFullPathName();
   }
   void SetProfileDirectory(const juce::File& directory);
   void SwitchToProfile(int profile_index);
   void SwitchToProfile(const juce::String& profile);

 private:
   [[nodiscard]] const std::vector<juce::String>& GetMenuItems() const noexcept;
   void ConnectionCallback(bool, bool);
   void handleAsyncUpdate() override;
   void MapCommand(rsj::MidiMessageId msg);
   void MidiCmdCallback(const rsj::MidiMessage&);
   void SwitchToNextProfile();
   void SwitchToPreviousProfile();

   enum class SwitchState {
      kNone,
      kPrev,
      kNext,
   };

   const Profile& current_profile_;
   ControlsModel& controls_model_;
   int current_profile_index_ {0};
   juce::File profile_location_;
   LrIpcOut& lr_ipc_out_;
   std::vector<juce::String> profiles_;
   std::vector<std::function<void(juce::XmlElement*, const juce::String&)>> callbacks_;
   SwitchState switch_state_ {SwitchState::kNone};
};

#endif
