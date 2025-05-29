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

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

class ControlsModel;
class LrIpcOut;
class MidiReceiver;
class Profile;

#ifndef _MSC_VER
#define _In_ //-V3547
#endif

namespace rsj {
   struct MidiMessage;
   struct MidiMessageId;
} // namespace rsj

class ProfileManager final : juce::AsyncUpdater {
 public:
   ProfileManager(ControlsModel& c_model, const Profile& profile, LrIpcOut& out,
       MidiReceiver& midi_receiver);
   ~ProfileManager() = default; // NOLINT(modernize-use-override)
   ProfileManager(const ProfileManager& other) = delete;
   ProfileManager(ProfileManager&& other) = delete;
   ProfileManager& operator=(const ProfileManager& other) = delete;
   ProfileManager& operator=(ProfileManager&& other) = delete;

   template<class T>
   void AddCallback(_In_ T* const object,
       _In_ void (T::* const mf)(juce::XmlElement*, const juce::String&))
   {
      if (object && mf) { callbacks_.emplace_back(std::bind_front(mf, object)); }
   }

   [[nodiscard]] const juce::String& GetProfileDirectory() const noexcept
   {
      return profile_location_.getFullPathName();
   }

   void SetProfileDirectory(const juce::File& directory);
   void SwitchToProfile(int profile_index);
   void SwitchToProfile(const juce::String& profile);

 private:
   void ConnectionCallback(bool, bool);
   void handleAsyncUpdate() override;
   void MapCommand(rsj::MidiMessageId msg);
   void MidiCmdCallback(rsj::MidiMessage mm);
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
#ifdef __cpp_lib_copyable_function
   std::vector<std::copyable_function<void(juce::XmlElement*, const juce::String&)>> callbacks_;
#else
   std::vector<std::function<void(juce::XmlElement*, const juce::String&)>> callbacks_;
#endif
   SwitchState switch_state_ {SwitchState::kNone};
};

#endif
