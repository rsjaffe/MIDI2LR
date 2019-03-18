/*
  ==============================================================================

    SettingsManager.cpp

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
#include "SettingsManager.h"

#include <string>
#include <utility>

#include "DebugInfo.h"
#include "LR_IPC_Out.h"
#include "ProfileManager.h"
using namespace std::literals::string_literals;
namespace {
   constexpr auto kAutoHideSection{"autohide"};
}

SettingsManager::SettingsManager(ProfileManager& profile_manager) try : profile_manager_ {
   profile_manager
}
{
   juce::PropertiesFile::Options file_options;
   file_options.applicationName = "MIDI2LR";
   file_options.commonToAllUsers = false;
   file_options.filenameSuffix = "xml";
   file_options.osxLibrarySubFolder = "Application Support/MIDI2LR";
   file_options.storageFormat = juce::PropertiesFile::storeAsXML;
   properties_file_ = std::make_unique<juce::PropertiesFile>(file_options);
}
catch (const std::exception& e)
{
   rsj::ExceptionResponse(__func__, __func__, e);
   throw;
}

void SettingsManager::Init(std::weak_ptr<LrIpcOut>&& lr_ipc_out)
{
   try {
      lr_ipc_out_ = std::move(lr_ipc_out);
      if (const auto ptr = lr_ipc_out_.lock())
         // add ourselves as a listener to LR_IPC_OUT so that we can send plugin
         // settings on connection
         ptr->AddCallback(this, &SettingsManager::ConnectionCallback);
      // set the profile directory
      profile_manager_.SetProfileDirectory(GetProfileDirectory());
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(__func__, __func__, e);
      throw;
   }
}

bool SettingsManager::GetPickupEnabled() const noexcept
{
   return properties_file_->getBoolValue("pickup_enabled", true);
}

void SettingsManager::SetPickupEnabled(bool enabled)
{
   try {
      properties_file_->setValue("pickup_enabled", enabled);
      properties_file_->saveIfNeeded();
      if (const auto ptr = lr_ipc_out_.lock())
         ptr->SendCommand("Pickup "s + (enabled ? '1' : '0') + '\n');
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(__func__, __func__, e);
      throw;
   }
}
juce::String SettingsManager::GetProfileDirectory() const noexcept
{
   return properties_file_->getValue("profile_directory");
}

void SettingsManager::SetProfileDirectory(const juce::String& profile_directory)
{
   try {
      properties_file_->setValue("profile_directory", profile_directory);
      if (!properties_file_->saveIfNeeded())
         rsj::Log("SettingsManager::SetProfileDirectory saveIfNeeded failed. Directory "
                  + profile_directory);
      profile_manager_.SetProfileDirectory(profile_directory);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(__func__, __func__, e);
      throw;
   }
}

void SettingsManager::ConnectionCallback(bool connected, bool blocked)
{
   try {
      if (connected && !blocked)
         if (const auto ptr = lr_ipc_out_.lock()) {
            DebugInfo db{GetProfileDirectory()};
            ptr->SendCommand("Pickup "s + (GetPickupEnabled() ? '1' : '0') + '\n');
            rsj::Log(GetPickupEnabled() ? "Pickup is enabled" : "Pickup is disabled");
            // rest of info about app is logged by DebugInfo
            ptr->SendCommand("AppInfoClear 1\n"s);
            for (const auto& info : db.GetInfo()) {
               ptr->SendCommand("AppInfo "s + info + '\n');
            }
            ptr->SendCommand("AppInfoDone 1\n"s);
         }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(__func__, __func__, e);
      throw;
   }
}

int SettingsManager::GetAutoHideTime() const noexcept
{
   return properties_file_->getIntValue(kAutoHideSection, 0);
}

void SettingsManager::SetAutoHideTime(int new_time)
{
   try {
      properties_file_->setValue(kAutoHideSection, new_time);
      properties_file_->saveIfNeeded();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(__func__, __func__, e);
      throw;
   }
}

int SettingsManager::GetLastVersionFound() const noexcept
{
   return properties_file_->getIntValue("LastVersionFound", 0);
}

void SettingsManager::SetLastVersionFound(int version_number)
{
   try {
      properties_file_->setValue("LastVersionFound", version_number);
      if (!properties_file_->saveIfNeeded())
         rsj::Log("SettingsManager::SetLastVersionFound saveIfNeeded failed. Directory "
                  + GetProfileDirectory());
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(__func__, __func__, e);
      throw;
   }
}