/*
  ==============================================================================

    ProfileManager.cpp

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
#include "ProfileManager.h"
#include <string>
#include <utility>
#include "LRCommands.h"

ProfileManager::ProfileManager() noexcept {}

void ProfileManager::Init(std::weak_ptr<LR_IPC_OUT>&& out,
  std::shared_ptr<CommandMap>& commandMap,
  std::shared_ptr<MIDIProcessor>& midiProcessor) {
    //copy the pointers
  command_map_ = commandMap;
  lr_ipc_out_ = std::move(out);

  if (const auto ptr = lr_ipc_out_.lock()) {
      // add ourselves as a listener to LR_IPC_OUT so that we can send plugin
      // settings on connection
    ptr->addListener(this);
  }

  if (midiProcessor) {
    midiProcessor->addMIDICommandListener(this);
  }
}

void ProfileManager::addListener(ProfileChangeListener *listener) {
  for (const auto& current_listener : listeners_)
    if (current_listener == listener)
      return; //don't add duplicates
  listeners_.push_back(listener);
}

void ProfileManager::setProfileDirectory(const juce::File& directory) {
  profile_location_ = directory;

  juce::Array<juce::File> file_array;
  directory.findChildFiles(file_array, juce::File::findFiles, false, "*.xml");

  current_profile_index_ = 0;
  profiles_.clear();
  for (const auto file : file_array)
    profiles_.emplace_back(file.getFileName());

  if (profiles_.size() > 0)
    switchToProfile(profiles_[0]);
}

const std::vector<juce::String>& ProfileManager::getMenuItems() const noexcept {
  return profiles_;
}

void ProfileManager::switchToProfile(int profile_index) {
  if (profile_index >= 0 && profile_index < static_cast<int>(profiles_.size())) {
    switchToProfile(profiles_[static_cast<size_t>(profile_index)]);
    current_profile_index_ = profile_index;
  }
}

void ProfileManager::switchToProfile(const juce::String& profile) {
  const auto profile_file = profile_location_.getChildFile(profile);

  if (profile_file.exists()) {
    std::unique_ptr<juce::XmlElement> xml_element{juce::XmlDocument::parse(profile_file)};
    for (const auto& listener : listeners_)
      listener->profileChanged(xml_element.get(), profile);

    if (const auto ptr = lr_ipc_out_.lock()) {
      std::string command = "ChangedToDirectory " +
        juce::File::addTrailingSeparator(profile_location_.getFullPathName()).toStdString() +
        '\n';
      ptr->sendCommand(command);
      command = "ChangedToFile " + profile.toStdString() + '\n';
      ptr->sendCommand(command);
    }
  }
}

void ProfileManager::switchToNextProfile() {
  current_profile_index_++;
  if (current_profile_index_ == static_cast<int>(profiles_.size()))
    current_profile_index_ = 0;

  switchToProfile(current_profile_index_);
}

void ProfileManager::switchToPreviousProfile() {
  current_profile_index_--;
  if (current_profile_index_ < 0) current_profile_index_ = static_cast<int>(profiles_.size()) - 1;

  switchToProfile(current_profile_index_);
}

void ProfileManager::handleMidiCC(int midi_channel, int controller, int value) {
  const MIDI_Message_ID cc{midi_channel, controller, true};

  if (command_map_) {
      // return if the value isn't 127, or the command isn't a valid
      // profile-related command
    if ((value != 127) || !command_map_->messageExistsInMap(cc))
      return;

    if (command_map_->getCommandforMessage(cc) == "Previous Profile") {
      switch_state_ = SWITCH_STATE::PREV;
      triggerAsyncUpdate();
    }
    else if (command_map_->getCommandforMessage(cc) == "Next Profile") {
      switch_state_ = SWITCH_STATE::NEXT;
      triggerAsyncUpdate();
    }
  }
}

void ProfileManager::handleMidiNote(int midi_channel, int note) {
  const MIDI_Message_ID note_msg{midi_channel, note, false};

  if (command_map_) {
      // return if the command isn't a valid profile-related command
    if (!command_map_->messageExistsInMap(note_msg))
      return;

    if (command_map_->getCommandforMessage(note_msg) == "Previous Profile") {
      switch_state_ = SWITCH_STATE::PREV;
      triggerAsyncUpdate();
    }
    else if (command_map_->getCommandforMessage(note_msg) == "Next Profile") {
      switch_state_ = SWITCH_STATE::NEXT;
      triggerAsyncUpdate();
    }
  }
}

void ProfileManager::connected() {
  const std::string command = "ChangedToDirectory " +
    juce::File::addTrailingSeparator(profile_location_.getFullPathName()).toStdString() +
    '\n';
  if (const auto ptr = lr_ipc_out_.lock()) {
    ptr->sendCommand(command);
  }
}

void ProfileManager::disconnected() {}

void ProfileManager::handleAsyncUpdate() {
  switch (switch_state_) {
    case SWITCH_STATE::PREV:
      switchToPreviousProfile();
      switch_state_ = SWITCH_STATE::NONE;
      break;
    case SWITCH_STATE::NEXT:
      switchToNextProfile();
      switch_state_ = SWITCH_STATE::NONE;
      break;
    default:
      break;
  }
}