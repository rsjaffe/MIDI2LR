/*
  ==============================================================================

	SettingsManager.h
	Created: 24 Aug 2015 7:11:29pm
	Author:  Parth, Jaffe

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
#ifndef SETTINGSMANAGER_H_INCLUDED
#define SETTINGSMANAGER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "LR_IPC_OUT.h"
#include "ProfileManager.h"

class SettingsManager : public LRConnectionListener
{
public:
	SettingsManager();
	virtual ~SettingsManager();

	bool getPickupEnabled() const;
	void setPickupEnabled(bool enabled);

	void setProfileDirectory(const String& profileDirectory);
	String getProfileDirectory() const;

	// LRConnectionListener interface
	virtual void connected() override;
	virtual void disconnected() override;

	int getAutoHideTime() const;
	void setAutoHideTime(int newTime);
	void Init(LR_IPC_OUT *lr_IPC_OUT);
private:

	LR_IPC_OUT *m_lr_IPC_OUT;
	ScopedPointer<PropertiesFile> _propertiesFile;
};



#endif  // SETTINGSMANAGER_H_INCLUDED
