#pragma once
/*
  ==============================================================================

	SettingsManager.h

This file is part of MIDI2LR. Copyright 2015-2017 by Rory Jaffe.

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

#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"
class LR_IPC_OUT;
class ProfileManager;

class SettingsManager final {
public:
	explicit SettingsManager(ProfileManager* profile_manager);
	virtual ~SettingsManager() {};
	void Init(std::weak_ptr<LR_IPC_OUT>&& lr_IPC_OUT);
	bool getPickupEnabled() const noexcept;
	void setPickupEnabled(bool enabled);
	juce::String getProfileDirectory() const noexcept;
	void setProfileDirectory(const juce::String& profile_directory);
	void ConnectionCallback(bool);
	int getAutoHideTime() const noexcept;
	void setAutoHideTime(int new_time);
	int getLastVersionFound() const noexcept;
	void setLastVersionFound(int version_number);

private:
	ProfileManager* const profile_manager_;
	std::unique_ptr<juce::PropertiesFile> properties_file_;
	std::weak_ptr<LR_IPC_OUT> lr_ipc_out_;
};

#endif  // SETTINGSMANAGER_H_INCLUDED
