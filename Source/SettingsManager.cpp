/*
  ==============================================================================

	SettingsManager.cpp
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
#include "SettingsManager.h"
#include "ProfileManager.h"

#define AUTOHIDE_SECTION "autohide"



SettingsManager::SettingsManager() : m_lr_IPC_OUT(NULL)
{
	PropertiesFile::Options opts;
	opts.applicationName = "MIDI2LR";
	opts.commonToAllUsers = false;
	opts.filenameSuffix = "xml";
	opts.osxLibrarySubFolder = "Application Support/MIDI2LR";
	opts.storageFormat = PropertiesFile::storeAsXML;

	_propertiesFile = new PropertiesFile(opts);

}

void SettingsManager::setPickupEnabled(bool enabled)
{
	_propertiesFile->setValue("pickup_enabled", enabled);
	_propertiesFile->saveIfNeeded();

	String command = String::formatted("Pickup %d\n", enabled);

	if (m_lr_IPC_OUT)
	{
		m_lr_IPC_OUT->sendCommand(command);
	}
	
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

	if (m_lr_IPC_OUT)
	{
		m_lr_IPC_OUT->sendCommand(command);
	}
	
}

void SettingsManager::disconnected()
{

}

int SettingsManager::getAutoHideTime() const
{
	return _propertiesFile->getIntValue(AUTOHIDE_SECTION, 0);

}
void SettingsManager::setAutoHideTime(int newTime)
{
	_propertiesFile->setValue(AUTOHIDE_SECTION, newTime);
	_propertiesFile->saveIfNeeded();

}

void SettingsManager::Init(LR_IPC_OUT *lr_IPC_OUT)
{
	m_lr_IPC_OUT = lr_IPC_OUT;

	if (m_lr_IPC_OUT)
	{
		// add ourselves as a listener to LR_IPC_OUT so that we can send plugin settings on connection
		m_lr_IPC_OUT->addListener(this);
	}

	// set the profile directory
	File profileDir(getProfileDirectory());
	ProfileManager::getInstance().setProfileDirectory(profileDir);

}