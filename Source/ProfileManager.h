#ifndef MIDI2LR_PROFILEMANAGER_H_INCLUDED
#define MIDI2LR_PROFILEMANAGER_H_INCLUDED
/*
  ==============================================================================

    ProfileManager.h

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
#include <exception>
#include <functional>
#include <memory>
#include <vector>

#include <JuceLibraryCode/JuceHeader.h>
#include "Misc.h"

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
   ProfileManager(ControlsModel& c_model, Profile& profile, std::weak_ptr<LrIpcOut>&& out) noexcept;
   ~ProfileManager() = default;
   ProfileManager(const ProfileManager& other) = delete;
   ProfileManager(ProfileManager&& other) = delete;
   ProfileManager& operator=(const ProfileManager& other) = delete;
   ProfileManager& operator=(ProfileManager&& other) = delete;
   void Start(MidiReceiver& midi_receiver);
   template<class T>
   void AddCallback(T* const object, void (T::*const mf)(juce::XmlElement*, const juce::String&))
   {
      try {
         using namespace std::placeholders;
         if (object && mf) // only store non-empty functions
            callbacks_.emplace_back(std::bind(mf, object, _1, _2));
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }
   const juce::String& GetProfileDirectory() const noexcept
   {
      return profile_location_.getFullPathName();
   }
   // sets the default profile directory and scans its contents for profiles
   void SetProfileDirectory(const juce::File& directory);
   // switches to a profile defined by an index
   void SwitchToProfile(int profile_index);
   // switches to a profile defined by a name
   void SwitchToProfile(const juce::String& profile);

 private:
   // returns an array of profile names
   [[nodiscard]] const std::vector<juce::String>& GetMenuItems() const noexcept;
   // switches to the next profile
   void SwitchToNextProfile();
   // switches to the previous profile
   void SwitchToPreviousProfile();
   // AsyncUpdate interface
   void handleAsyncUpdate() override;

   void MidiCmdCallback(rsj::MidiMessage);
   void MapCommand(const rsj::MidiMessageId& msg);

   enum class SwitchState {
      kNone,
      kPrev,
      kNext,
   };

   Profile& current_profile_;
   ControlsModel& controls_model_;
   int current_profile_index_{0};
   juce::File profile_location_;
   std::vector<juce::String> profiles_;
   std::vector<std::function<void(juce::XmlElement*, const juce::String&)>> callbacks_;
   std::weak_ptr<LrIpcOut> lr_ipc_out_;
   SwitchState switch_state_{SwitchState::kNone};
   void ConnectionCallback(bool, bool);
};

#endif // PROFILEMANAGER_H_INCLUDED
