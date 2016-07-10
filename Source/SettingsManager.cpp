/*
  ==============================================================================

    SettingsManager.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

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
#include "ProfileManager.h"

//constexpr and auto don't work in XCode
const juce::String AutoHideSection{"autohide"};

SettingsManager::SettingsManager() {
  PropertiesFile::Options file_options;
  file_options.applicationName = "MIDI2LR";
  file_options.commonToAllUsers = false;
  file_options.filenameSuffix = "xml";
  file_options.osxLibrarySubFolder = "Application Support/MIDI2LR";
  file_options.storageFormat = PropertiesFile::storeAsXML;

  properties_file_ = std::make_unique<PropertiesFile>(file_options);
}

void SettingsManager::Init(std::shared_ptr<LR_IPC_OUT>& lr_ipc_out,
  std::shared_ptr<ProfileManager>& profile_manager) {
  lr_ipc_out_ = lr_ipc_out;

  if (lr_ipc_out_) {
      // add ourselves as a listener to LR_IPC_OUT so that we can send plugin
      // settings on connection
    lr_ipc_out_->addListener(this);
  }

  profile_manager_ = profile_manager;

  if (profile_manager_) {
      // set the profile directory
    File profile_directory{getProfileDirectory()};
    profile_manager->setProfileDirectory(profile_directory);
  }
}

bool SettingsManager::getPickupEnabled() const noexcept {
  return properties_file_->getBoolValue("pickup_enabled", true);
}

void SettingsManager::setPickupEnabled(bool enabled) {
  properties_file_->setValue("pickup_enabled", enabled);
  properties_file_->saveIfNeeded();

  auto command = String::formatted("Pickup %d\n", enabled);

  if (lr_ipc_out_) {
    lr_ipc_out_->sendCommand(command);
  }
}

String SettingsManager::getProfileDirectory() const noexcept {
  return properties_file_->getValue("profile_directory");
}

void SettingsManager::setProfileDirectory(const String& profile_directory_name) {
  properties_file_->setValue("profile_directory", profile_directory_name);
  properties_file_->saveIfNeeded();

  if (profile_manager_) {
    File profileDir{profile_directory_name};
    profile_manager_->setProfileDirectory(profileDir);
  }
}

bool SettingsManager::getContinuousEncoders() const noexcept {
  return properties_file_->getBoolValue("continuous_encoders_enabled", false);
}

void SettingsManager::setContinuousEncoders(bool enabled) {
  properties_file_->setValue("continuous_encoders_enabled", enabled);
  properties_file_->saveIfNeeded();
}

String SettingsManager::getPitchMaxValue() const noexcept {
  return properties_file_->getValue("pitch_max_value", "16383.0");
}

void SettingsManager::setPitchMaxValue(const String& pitch_max_value) {
  properties_file_->setValue("pitch_max_value", pitch_max_value);
  properties_file_->saveIfNeeded();
}

void SettingsManager::connected() {
  auto command = String::formatted("Pickup %d\n", getPickupEnabled());

  if (lr_ipc_out_) {
    lr_ipc_out_->sendCommand(command);
  }
}

void SettingsManager::disconnected() {}

int SettingsManager::getAutoHideTime() const noexcept {
  return properties_file_->getIntValue(AutoHideSection, 0);
}

void SettingsManager::setAutoHideTime(int new_time) {
  properties_file_->setValue(AutoHideSection, new_time);
  properties_file_->saveIfNeeded();
}

int SettingsManager::getLastVersionFound() const noexcept {
  return properties_file_->getIntValue("LastVersionFound", 0);
}

void SettingsManager::setLastVersionFound(int new_version) {
  properties_file_->setValue("LastVersionFound", new_version);
  properties_file_->saveIfNeeded();
}