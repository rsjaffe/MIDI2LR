/*
  ==============================================================================

	SettingsManager.cpp

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

/**********************************************************************************************//**
 * @def AUTOHIDE_SECTION
 *
 * @brief   A macro that defines autohide section.
 *
 * @date    3/22/2016
 **************************************************************************************************/

#define AUTOHIDE_SECTION "autohide"

/**********************************************************************************************//**
 * @fn  SettingsManager::SettingsManager()
 *
 * @brief   Default constructor.
 *
 * @date    3/22/2016
 **************************************************************************************************/

SettingsManager::SettingsManager() : m_lr_IPC_OUT(nullptr), m_profileManager(nullptr)
{
	PropertiesFile::Options opts;
	opts.applicationName = "MIDI2LR";
	opts.commonToAllUsers = false;
	opts.filenameSuffix = "xml";
	opts.osxLibrarySubFolder = "Application Support/MIDI2LR";
	opts.storageFormat = PropertiesFile::storeAsXML;

	_propertiesFile = new PropertiesFile(opts);

}

/**********************************************************************************************//**
 * @fn  void SettingsManager::setPickupEnabled(bool enabled)
 *
 * @brief   Sets pickup enabled.
 *
 * @date    3/22/2016
 *
 * @param   enabled true to enable, false to disable.
 **************************************************************************************************/

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

/**********************************************************************************************//**
 * @fn  bool SettingsManager::getPickupEnabled() const
 *
 * @brief   Gets pickup enabled.
 *
 * @date    3/22/2016
 *
 * @return  true if it succeeds, false if it fails.
 **************************************************************************************************/

bool SettingsManager::getPickupEnabled() const
{
	return _propertiesFile->getBoolValue("pickup_enabled", true);
}

/**********************************************************************************************//**
 * @fn  String SettingsManager::getProfileDirectory() const
 *
 * @brief   Gets profile directory.
 *
 * @date    3/22/2016
 *
 * @return  The profile directory.
 **************************************************************************************************/

String SettingsManager::getProfileDirectory() const
{
	return _propertiesFile->getValue("profile_directory");
}

/**********************************************************************************************//**
 * @fn  void SettingsManager::setProfileDirectory(const String& profileDirStr)
 *
 * @brief   Sets profile directory.
 *
 * @date    3/22/2016
 *
 * @param   profileDirStr   The profile dir string.
 **************************************************************************************************/

void SettingsManager::setProfileDirectory(const String& profileDirStr)
{
	_propertiesFile->setValue("profile_directory", profileDirStr);
	_propertiesFile->saveIfNeeded();

	if (m_profileManager)
	{
		File profileDir(profileDirStr);
		m_profileManager->setProfileDirectory(profileDir);
	}
}

/**********************************************************************************************//**
 * @fn  void SettingsManager::connected()
 *
 * @brief   Connected this object.
 *
 * @date    3/22/2016
 **************************************************************************************************/

void SettingsManager::connected()
{
	String command = String::formatted("Pickup %d\n", getPickupEnabled());

	if (m_lr_IPC_OUT)
	{
		m_lr_IPC_OUT->sendCommand(command);
	}
	
}

/**********************************************************************************************//**
 * @fn  void SettingsManager::disconnected()
 *
 * @brief   Disconnect from the ed.
 *
 * @date    3/22/2016
 **************************************************************************************************/

void SettingsManager::disconnected()
{

}

/**********************************************************************************************//**
 * @fn  int SettingsManager::getAutoHideTime() const
 *
 * @brief   Gets automatic hide time.
 *
 * @date    3/22/2016
 *
 * @return  The automatic hide time.
 **************************************************************************************************/

int SettingsManager::getAutoHideTime() const
{
	return _propertiesFile->getIntValue(AUTOHIDE_SECTION, 0);

}

/**********************************************************************************************//**
 * @fn  void SettingsManager::setAutoHideTime(int newTime)
 *
 * @brief   Sets automatic hide time.
 *
 * @date    3/22/2016
 *
 * @param   newTime The new time.
 **************************************************************************************************/

void SettingsManager::setAutoHideTime(int newTime)
{
	_propertiesFile->setValue(AUTOHIDE_SECTION, newTime);
	_propertiesFile->saveIfNeeded();

}

/**********************************************************************************************//**
 * @fn  int SettingsManager::getLastVersionFound() const
 *
 * @brief   Gets the last version found.
 *
 * @date    3/22/2016
 *
 * @return  The last version found.
 **************************************************************************************************/

int SettingsManager::getLastVersionFound() const
{
    return _propertiesFile->getIntValue("LastVersionFound", 0);
}

/**********************************************************************************************//**
 * @fn  void SettingsManager::setLastVersionFound(int newversion)
 *
 * @brief   Sets last version found.
 *
 * @date    3/22/2016
 *
 * @param   newversion  The newversion.
 **************************************************************************************************/

void SettingsManager::setLastVersionFound(int newversion)
{
    _propertiesFile->setValue("LastVersionFound", newversion);
    _propertiesFile->saveIfNeeded();
}

/**********************************************************************************************//**
 * @fn  void SettingsManager::Init(std::shared_ptr<LR_IPC_OUT>lr_IPC_OUT, std::shared_ptr<ProfileManager> profileManager)
 *
 * @brief   S.
 *
 * @date    3/22/2016
 *
 * @param   parameter1      If non-null, the lr ipc out.
 * @param   profileManager  If non-null, manager for profile.
 **************************************************************************************************/

void SettingsManager::Init(std::shared_ptr<LR_IPC_OUT>lr_IPC_OUT, std::shared_ptr<ProfileManager> profileManager)
{
	m_lr_IPC_OUT = lr_IPC_OUT;

	if (m_lr_IPC_OUT)
	{
		// add ourselves as a listener to LR_IPC_OUT so that we can send plugin settings on connection
		m_lr_IPC_OUT->addListener(this);
	}

	m_profileManager = profileManager;

	if (m_profileManager)
	{
		// set the profile directory
		File profileDir(getProfileDirectory());
		profileManager->setProfileDirectory(profileDir);
	}

}