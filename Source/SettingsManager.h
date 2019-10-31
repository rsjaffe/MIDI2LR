#ifndef MIDI2LR_SETTINGSMANAGER_H_INCLUDED
#define MIDI2LR_SETTINGSMANAGER_H_INCLUDED
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
#include <memory>

#include <JuceLibraryCode/JuceHeader.h>
#include "LR_IPC_Out.h"
#include "ProfileManager.h"

class SettingsManager final {
 public:
   SettingsManager(ProfileManager& profile_manager, LrIpcOut& lr_ipc_out);
   ~SettingsManager() = default;
   SettingsManager(const SettingsManager& other) = delete;
   SettingsManager(SettingsManager&& other) = delete;
   SettingsManager& operator=(const SettingsManager& other) = delete;
   SettingsManager& operator=(SettingsManager&& other) = delete;
   [[nodiscard]] int GetAutoHideTime() const noexcept
   {
      return properties_file_->getIntValue("autohide", 0);
   }
   [[nodiscard]] int GetLastVersionFound() const noexcept
   {
      return properties_file_->getIntValue("LastVersionFound", 0);
   }
   [[nodiscard]] bool GetPickupEnabled() const noexcept
   {
      return properties_file_->getBoolValue("pickup_enabled", true);
   }
   [[nodiscard]] juce::String GetProfileDirectory() const noexcept
   {
      return properties_file_->getValue("profile_directory");
   }
   void SetAutoHideTime(int new_time)
   {
      properties_file_->setValue("autohide", new_time);
   }
   void SetLastVersionFound(int version_number)
   {
      properties_file_->setValue("LastVersionFound", version_number);
   }
   void SetPickupEnabled(bool enabled)
   {
      properties_file_->setValue("pickup_enabled", enabled);
      lr_ipc_out_.SendCommand(std::string("Pickup ") + (enabled ? '1' : '0') + '\n');
   }
   void SetProfileDirectory(const juce::String& profile_directory)
   {
      properties_file_->setValue("profile_directory", profile_directory);
      profile_manager_.SetProfileDirectory(profile_directory);
   }

 private:
   void ConnectionCallback(bool, bool);

   LrIpcOut& lr_ipc_out_;
   ProfileManager& profile_manager_;
   std::unique_ptr<juce::PropertiesFile> properties_file_;
};

#endif // SETTINGSMANAGER_H_INCLUDED
