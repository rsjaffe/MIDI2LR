#pragma once
/*
  ==============================================================================

    SettingsManager.h

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
#ifndef SETTINGSMANAGER_H_INCLUDED
#define SETTINGSMANAGER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "LR_IPC_OUT.h"
#include "ProfileManager.h"

class SettingsManager final: public LRConnectionListener {
public:
  SettingsManager();
  virtual ~SettingsManager() {};
  void Init(std::weak_ptr<LR_IPC_OUT>&& lr_IPC_OUT,
    std::weak_ptr<ProfileManager>&& profile_manager);

  bool getPickupEnabled() const noexcept;
  void setPickupEnabled(bool enabled);

  String getProfileDirectory() const noexcept;
  void setProfileDirectory(const String& profile_directory);

  // LRConnectionListener interface
  virtual void connected() override;
  virtual void disconnected() override;

  int getAutoHideTime() const noexcept;
  void setAutoHideTime(int new_time);

  int getLastVersionFound() const noexcept;
  void setLastVersionFound(int version_number);

private:

  std::weak_ptr<LR_IPC_OUT> lr_ipc_out_;
  std::weak_ptr<ProfileManager> profile_manager_;
  std::unique_ptr<PropertiesFile> properties_file_;
};

#endif  // SETTINGSMANAGER_H_INCLUDED
