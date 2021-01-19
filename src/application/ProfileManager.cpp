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
#include "ProfileManager.h"

#include <exception>
#include <string>

#include <fmt/format.h>
#include <gsl/gsl>

#include "ControlsModel.h"
#include "LR_IPC_Out.h"
#include "MIDIReceiver.h"
#include "MidiUtilities.h"
#include "Misc.h"
#include "Profile.h"

ProfileManager::ProfileManager(
    ControlsModel& c_model, const Profile& profile, LrIpcOut& out, MidiReceiver& midi_receiver)
    : current_profile_ {profile}, controls_model_ {c_model}, lr_ipc_out_ {out}
{
   /* add ourselves as a listener to LR_IPC_OUT so that we can send plugin settings on connection */
   midi_receiver.AddCallback(this, &ProfileManager::MidiCmdCallback);
   lr_ipc_out_.AddCallback(this, &ProfileManager::ConnectionCallback);
}

void ProfileManager::SetProfileDirectory(const juce::File& directory)
{
   try {
      profile_location_ = directory;
      auto file_array {directory.findChildFiles(juce::File::findFiles, false, "*.xml")};
      file_array.sort();
      profiles_.clear();
      for (const auto& file : file_array) profiles_.emplace_back(file.getFileName());
      if (!profiles_.empty())
         SwitchToProfile(profiles_.front());
      current_profile_index_ = 0;
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

const std::vector<juce::String>& ProfileManager::GetMenuItems() const noexcept { return profiles_; }

void ProfileManager::SwitchToProfile(int profile_index)
{
   try {
      if (profile_index >= 0 && rsj::cmp_less(profile_index, profiles_.size())) {
         SwitchToProfile(profiles_.at(gsl::narrow_cast<size_t>(profile_index)));
         current_profile_index_ = profile_index;
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void ProfileManager::SwitchToProfile(const juce::String& profile)
{
   try {
      const auto profile_file {profile_location_.getChildFile(profile)};
      if (profile_file.exists()) {
         if (const auto parsed {juce::parseXML(profile_file)}) {
            for (const auto& cb : callbacks_) cb(parsed.get(), profile);
            lr_ipc_out_.SendCommand(fmt::format(FMT_STRING("ChangedToDirectory {}\n"),
                juce::File::addTrailingSeparator(profile_location_.getFullPathName())
                    .toStdString()));
            lr_ipc_out_.SendCommand(
                fmt::format(FMT_STRING("ChangedToFile {}\n"), profile.toStdString()));
         }
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void ProfileManager::SwitchToNextProfile()
{
   try {
      if (rsj::cmp_equal(++current_profile_index_, profiles_.size()))
         current_profile_index_ = 0;
      SwitchToProfile(current_profile_index_);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void ProfileManager::SwitchToPreviousProfile()
{
   try {
      if (--current_profile_index_ < 0)
         current_profile_index_ = gsl::narrow_cast<int>(profiles_.size()) - 1;
      SwitchToProfile(current_profile_index_);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void ProfileManager::MapCommand(const rsj::MidiMessageId& msg)
{
   try {
      const auto cmd {current_profile_.GetCommandForMessage(msg)};
      if (cmd == "PrevPro") {
         switch_state_ = SwitchState::kPrev;
         triggerAsyncUpdate();
      }
      else if (cmd == "NextPro") {
         switch_state_ = SwitchState::kNext;
         triggerAsyncUpdate();
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void ProfileManager::MidiCmdCallback(const rsj::MidiMessage& mm)
{
   try {
      const rsj::MidiMessageId cc {mm};
      /* return if the value isn't high enough (notes may be < 1), or the command isn't a valid
       * profile-related command */
      if (controls_model_.ControllerToPlugin(mm, false) < 0.4
          || !current_profile_.MessageExistsInMap(cc))
         return;
      MapCommand(cc);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ProfileManager::ConnectionCallback(const bool connected, const bool blocked)
{
   try {
      if (connected && !blocked) {
         lr_ipc_out_.SendCommand(fmt::format(FMT_STRING("ChangedToDirectory {}\n"),
             juce::File::addTrailingSeparator(profile_location_.getFullPathName()).toStdString()));
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void ProfileManager::handleAsyncUpdate()
{
   try {
      switch (switch_state_) {
      case SwitchState::kPrev:
         SwitchToPreviousProfile();
         switch_state_ = SwitchState::kNone;
         break;
      case SwitchState::kNext:
         SwitchToNextProfile();
         switch_state_ = SwitchState::kNone;
         break;
      case SwitchState::kNone:
         break;
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}