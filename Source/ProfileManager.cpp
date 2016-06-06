/*
  ==============================================================================

    ProfileManager.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/
#include "ProfileManager.h"
#include "LRCommands.h"

ProfileManager::ProfileManager() noexcept {}

void ProfileManager::addListener(ProfileChangeListener *listener) {
  listeners_.addIfNotAlreadyThere(listener);
}

void ProfileManager::setProfileDirectory(const File& directory) {
  profile_location_ = directory;

  Array<File> file_array;
  directory.findChildFiles(file_array, File::findFiles, false, "*.xml");

  current_profile_index_ = 0;
  profiles_.clear();
  for (auto file : file_array)
    profiles_.add(file.getFileName());

  if (profiles_.size() > 0)
    switchToProfile(profiles_[0]);
}

const StringArray& ProfileManager::getMenuItems() const noexcept {
  return profiles_;
}

void ProfileManager::switchToProfile(int profile_index) {
  if (profile_index >= 0 && profile_index < profiles_.size()) {
    switchToProfile(profiles_[profile_index]);
    current_profile_index_ = profile_index;
  }
}

void ProfileManager::switchToProfile(const String& profile) {
  auto profile_file = profile_location_.getChildFile(profile);

  if (profile_file.exists()) {
    std::unique_ptr<XmlElement> xml_element{XmlDocument::parse(profile_file)};
    for (auto listener : listeners_)
      listener->profileChanged(xml_element.get(), profile);

    if (lr_ipc_out_) {
      auto command = String{"ChangedToDirectory "} +
        File::addTrailingSeparator(profile_location_.getFullPathName()) + 
        String{"\n"};
      lr_ipc_out_->sendCommand(command);
      command = String("ChangedToFile ") + profile + String("\n");
      lr_ipc_out_->sendCommand(command);
    }
  }
}

void ProfileManager::switchToPreviousProfile() {
  current_profile_index_--;
  if (current_profile_index_ < 0) current_profile_index_ = profiles_.size() - 1;

  switchToProfile(current_profile_index_);
}

void ProfileManager::switchToNextProfile() {
  current_profile_index_++;
  if (current_profile_index_ == profiles_.size()) current_profile_index_ = 0;

  switchToProfile(current_profile_index_);
}

void ProfileManager::handleMidiCC(int midi_channel, int controller, int value) {
  const MIDI_Message cc{midi_channel, controller, true};

  if (command_map_) {
      // return if the value isn't 0 or 127, or the command isn't a valid profile-related command
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
  const MIDI_Message note_msg{midi_channel, note, false};

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

void ProfileManager::connected() {
  auto command = String{"ChangedToDirectory "} +
    File::addTrailingSeparator(profile_location_.getFullPathName()) + 
    String{"\n"};
  if (lr_ipc_out_) {
    lr_ipc_out_->sendCommand(command);
  }
}

void ProfileManager::disconnected() {}

void ProfileManager::Init(std::shared_ptr<LR_IPC_OUT> out, 
  std::shared_ptr<CommandMap> commandMap, 
  std::shared_ptr<MIDIProcessor> midiProcessor) {
    //copy the pointers
  command_map_ = commandMap;
  lr_ipc_out_ = out;

  if (lr_ipc_out_) {
      // add ourselves as a listener to LR_IPC_OUT so that we can send plugin settings on connection
    lr_ipc_out_->addListener(this);
  }

  if (midiProcessor) {
    midiProcessor->addMIDICommandListener(this);
  }
}