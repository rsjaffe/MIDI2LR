/*
  ==============================================================================

	ProfileManager.h
	Created: 29 Aug 2015 10:27:13pm
	Author:  Parth, Jaffe

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe, derived from code
by Parth.

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
#ifndef PROFILEMANAGER_H_INCLUDED
#define PROFILEMANAGER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "MIDIProcessor.h"
#include "LR_IPC_OUT.h"

class ProfileChangeListener
{
public:
	// called when the current profile is change
	virtual void profileChanged(XmlElement* elem, const String& filename) = 0;

	virtual ~ProfileChangeListener() {};
};

class ProfileManager : public MIDICommandListener,
	public AsyncUpdater, public LRConnectionListener
{
public:
	static ProfileManager& getInstance();
	void addListener(ProfileChangeListener *listener);

	// sets the default profile directory and scans its contents for profiles
	void setProfileDirectory(const File& dir);

	// returns an array of profile names
	const StringArray& getMenuItems() const;

	// switches to a profile defined by an index
	void switchToProfile(int profileIdx);

	// switches to a profile defined by a name
	void switchToProfile(const String& profile);

	// swithces to the next profile
	void switchToNextProfile();

	// switches to the previous profile
	void switchToPreviousProfile();

	// MIDICommandListener interface
	virtual void handleMidiCC(int midiChannel, int controller, int value) override;
	virtual void handleMidiNote(int midiChannel, int note) override;

	// AsyncUpdate interface
	virtual void handleAsyncUpdate() override;

    // LRConnectionListener interface
    virtual void connected() override;
    virtual void disconnected() override;

private:
	enum class SWITCH_STATE
	{
		NONE,
		PREV,
		NEXT,
	};

	ProfileManager();

	ProfileManager(ProfileManager const&) = delete;
	void operator=(ProfileManager const&) = delete;

	File _profileLocation;
	StringArray _profiles;
	Array<ProfileChangeListener *> _listeners;
	int _currentProfileIdx;
	SWITCH_STATE _switchState;
};


#endif  // PROFILEMANAGER_H_INCLUDED
