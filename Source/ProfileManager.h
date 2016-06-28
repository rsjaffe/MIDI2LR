#pragma once
/*
  ==============================================================================

    ProfileManager.h

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
#ifndef PROFILEMANAGER_H_INCLUDED
#define PROFILEMANAGER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "CommandMap.h"
#include "LR_IPC_OUT.h"
#include "MIDIProcessor.h"

class ProfileChangeListener {
public:
    // called when the current profile is changed
  virtual void profileChanged(XmlElement* elem, const String& file_name) = 0;

  virtual ~ProfileChangeListener() {};
};

class ProfileManager final: public MIDICommandListener,
  private AsyncUpdater, public LRConnectionListener {
public:
  ProfileManager() noexcept;
  virtual ~ProfileManager() {};
  void Init(std::shared_ptr<LR_IPC_OUT> out,
    std::shared_ptr<CommandMap> command_map,
    std::shared_ptr<MIDIProcessor> midi_processor);

  void addListener(ProfileChangeListener *listener);

  // sets the default profile directory and scans its contents for profiles
  void setProfileDirectory(const File& dir);

  // returns an array of profile names
  const StringArray& getMenuItems() const noexcept;

  // switches to a profile defined by an index
  void switchToProfile(int profileIdx);

  // switches to a profile defined by a name
  void switchToProfile(const String& profile);

  // switches to the next profile
  void switchToNextProfile();

  // switches to the previous profile
  void switchToPreviousProfile();

  // MIDICommandListener interface
  virtual void handleMidiCC(int midi_channel, int controller, int value) override;
  virtual void handleMidiNote(int midi_channel, int note) override;

  // LRConnectionListener interface
  virtual void connected() override;
  virtual void disconnected() override;

private:
  // AsyncUpdate interface
  virtual void handleAsyncUpdate() override;
  enum class SWITCH_STATE {
    NONE,
    PREV,
    NEXT,
  };

  ProfileManager(ProfileManager const&) = delete;
  void operator=(ProfileManager const&) = delete;

  std::vector<ProfileChangeListener *> listeners_;
  File profile_location_;
  int current_profile_index_{0};
  std::shared_ptr<CommandMap> command_map_{nullptr};
  std::shared_ptr<LR_IPC_OUT> lr_ipc_out_{nullptr};
  StringArray profiles_;
  SWITCH_STATE switch_state_;
};

#endif  // PROFILEMANAGER_H_INCLUDED
