/*
  ==============================================================================

	SettingsManager.cpp
	Created: 24 Aug 2015 7:11:29pm
	Author:  Parth

  ==============================================================================
*/

#include "SettingsManager.h"
#include "ProfileManager.h"

SettingsManager::SettingsManager()
{
	PropertiesFile::Options opts;
	opts.applicationName = "MIDI2LR";
	opts.commonToAllUsers = false;
	opts.filenameSuffix = "xml";
	opts.osxLibrarySubFolder = "Application Support/MIDI2LR";
	opts.storageFormat = PropertiesFile::storeAsXML;

	_propertiesFile = new PropertiesFile(opts);

	// add ourselves as a listener to LR_IPC_OUT so that we can send plugin settings on connection
	LR_IPC_OUT::getInstance().addListener(this);

	// set the profile directory
	File profileDir(getProfileDirectory());
	ProfileManager::getInstance().setProfileDirectory(profileDir);
}

SettingsManager& SettingsManager::getInstance()
{
	static SettingsManager instance;
	return instance;
}

void SettingsManager::setPickupEnabled(bool enabled)
{
	_propertiesFile->setValue("pickup_enabled", enabled);
	_propertiesFile->saveIfNeeded();

	String command = String::formatted("Pickup %d\n", enabled);
	LR_IPC_OUT::getInstance().sendCommand(command);
}

bool SettingsManager::getPickupEnabled() const
{
	return _propertiesFile->getBoolValue("pickup_enabled", true);
}

String SettingsManager::getProfileDirectory() const
{
	return _propertiesFile->getValue("profile_directory");
}

void SettingsManager::setProfileDirectory(const String& profileDirStr)
{
	_propertiesFile->setValue("profile_directory", profileDirStr);
	_propertiesFile->saveIfNeeded();

	File profileDir(profileDirStr);
	ProfileManager::getInstance().setProfileDirectory(profileDir);
}

void SettingsManager::connected()
{
	String command = String::formatted("Pickup %d\n", getPickupEnabled());
	LR_IPC_OUT::getInstance().sendCommand(command);
}

void SettingsManager::disconnected()
{

}