/*
  ==============================================================================

	SettingsManager.h
	Created: 24 Aug 2015 7:11:29pm
	Author:  Parth

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
	static SettingsManager& getInstance();

	bool getPickupEnabled() const;
	void setPickupEnabled(bool enabled);

	void setProfileDirectory(const String& profileDirectory);
	String getProfileDirectory() const;

	// LRConnectionListener interface
	virtual void connected() override;
	virtual void disconnected() override;

private:
	SettingsManager();

	SettingsManager(SettingsManager const&) = delete;
	void operator=(SettingsManager const&) = delete;

	ScopedPointer<PropertiesFile> _propertiesFile;
};



#endif  // SETTINGSMANAGER_H_INCLUDED
