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

/**********************************************************************************************//**
 * @fn  ProfileManager::ProfileManager() noexcept
 *
 * @brief   Default constructor.
 *
 * @date    3/22/2016
 **************************************************************************************************/

ProfileManager::ProfileManager() noexcept : _currentProfileIdx(0), m_commandMap(nullptr), m_lr_IPC_OUT(nullptr)
{
	
   
}

/**********************************************************************************************//**
 * @fn  void ProfileManager::addListener(ProfileChangeListener *listener)
 *
 * @brief   Adds a listener.
 *
 * @date    3/22/2016
 *
 * @param [in,out]  listener    If non-null, the listener.
 **************************************************************************************************/

void ProfileManager::addListener(ProfileChangeListener *listener)
{
	_listeners.addIfNotAlreadyThere(listener);
}

/**********************************************************************************************//**
 * @fn  void ProfileManager::setProfileDirectory(const File& dir)
 *
 * @brief   Sets profile directory.
 *
 * @date    3/22/2016
 *
 * @param   dir The dir.
 **************************************************************************************************/

void ProfileManager::setProfileDirectory(const File& dir)
{
	_profileLocation = dir;

	Array<File> fileArray;
	dir.findChildFiles(fileArray, File::findFiles, false, "*.xml");

	_currentProfileIdx = 0;
	_profiles.clear();
	for (auto file : fileArray)
		_profiles.add(file.getFileName());

	if (_profiles.size() > 0)
		switchToProfile(_profiles[0]);
}

/**********************************************************************************************//**
 * @fn  const StringArray& ProfileManager::getMenuItems() const noexcept
 *
 * @brief   Gets menu items.
 *
 * @date    3/22/2016
 *
 * @return  The menu items.
 **************************************************************************************************/

const StringArray& ProfileManager::getMenuItems() const noexcept
{
	return _profiles;
}

/**********************************************************************************************//**
 * @fn  void ProfileManager::switchToProfile(int profileIdx)
 *
 * @brief   Switch to profile.
 *
 * @date    3/22/2016
 *
 * @param   profileIdx  Zero-based index of the profile.
 **************************************************************************************************/

void ProfileManager::switchToProfile(int profileIdx)
{
	if (profileIdx >= 0 && profileIdx < _profiles.size())
	{
		switchToProfile(_profiles[profileIdx]);
		_currentProfileIdx = profileIdx;
	}
}

/**********************************************************************************************//**
 * @fn  void ProfileManager::switchToProfile(const String& profile)
 *
 * @brief   Switch to profile.
 *
 * @date    3/22/2016
 *
 * @param   profile The profile.
 **************************************************************************************************/

void ProfileManager::switchToProfile(const String& profile)
{
	File profileFile = _profileLocation.getChildFile(profile);

	if (profileFile.exists())
	{
        std::unique_ptr<XmlElement> elem{ XmlDocument::parse(profileFile) };
		for (auto listener : _listeners)
			listener->profileChanged(elem.get(), profile);
        
		if (m_lr_IPC_OUT)
		{
			String command = String("ChangedToDirectory ") + File::addTrailingSeparator(_profileLocation.getFullPathName()) + String("\n");
			m_lr_IPC_OUT->sendCommand(command);
			command = String("ChangedToFile ") + profile + String("\n");
			m_lr_IPC_OUT->sendCommand(command);
		}
		        
	}
}

/**********************************************************************************************//**
 * @fn  void ProfileManager::switchToPreviousProfile()
 *
 * @brief   Switch to previous profile.
 *
 * @date    3/22/2016
 **************************************************************************************************/

void ProfileManager::switchToPreviousProfile()
{
	_currentProfileIdx--;
	if (_currentProfileIdx < 0) _currentProfileIdx = _profiles.size() - 1;

	switchToProfile(_currentProfileIdx);
}

/**********************************************************************************************//**
 * @fn  void ProfileManager::switchToNextProfile()
 *
 * @brief   Switch to next profile.
 *
 * @date    3/22/2016
 **************************************************************************************************/

void ProfileManager::switchToNextProfile()
{
	_currentProfileIdx++;
	if (_currentProfileIdx == _profiles.size()) _currentProfileIdx = 0;

	switchToProfile(_currentProfileIdx);
}

/**********************************************************************************************//**
 * @fn  void ProfileManager::handleMidiCC(int midiChannel, int controller, int value)
 *
 * @brief   Handles the MIDI Cc.
 *
 * @date    3/22/2016
 *
 * @param   midiChannel The MIDI channel.
 * @param   controller  The controller.
 * @param   value       The value.
 **************************************************************************************************/

void ProfileManager::handleMidiCC(int midiChannel, int controller, int value)
{
	const MIDI_Message cc(midiChannel, controller, true);

	if (m_commandMap)
	{
		// return if the value isn't 0 or 127, or the command isn't a valid profile-related command
		if ((value != 127) || !m_commandMap->messageExistsInMap(cc))
			return;

		if (m_commandMap->getCommandforMessage(cc) == "Previous Profile")
		{
			_switchState = SWITCH_STATE::PREV;
			triggerAsyncUpdate();
		}
		else if (m_commandMap->getCommandforMessage(cc) == "Next Profile")
		{
			_switchState = SWITCH_STATE::NEXT;
			triggerAsyncUpdate();
		}
	}
}

/**********************************************************************************************//**
 * @fn  void ProfileManager::handleMidiNote(int midiChannel, int note)
 *
 * @brief   Handles the MIDI note.
 *
 * @date    3/22/2016
 *
 * @param   midiChannel The MIDI channel.
 * @param   note        The note.
 **************************************************************************************************/

void ProfileManager::handleMidiNote(int midiChannel, int note)
{
	const MIDI_Message note_msg(midiChannel, note, false);

	if (m_commandMap)
	{

		// return if the command isn't a valid profile-related command
		if (!m_commandMap->messageExistsInMap(note_msg))
			return;

		if (m_commandMap->getCommandforMessage(note_msg) == "Previous Profile")
		{
			_switchState = SWITCH_STATE::PREV;
			triggerAsyncUpdate();
		}
		else if (m_commandMap->getCommandforMessage(note_msg) == "Next Profile")
		{
			_switchState = SWITCH_STATE::NEXT;
			triggerAsyncUpdate();
		}
	}
}

/**********************************************************************************************//**
 * @fn  void ProfileManager::handleAsyncUpdate()
 *
 * @brief   Handles the asynchronous update.
 *
 * @date    3/22/2016
 **************************************************************************************************/

void ProfileManager::handleAsyncUpdate()
{
	switch (_switchState)
	{
	case SWITCH_STATE::PREV:
		switchToPreviousProfile();
		_switchState = SWITCH_STATE::NONE;
		break;
	case SWITCH_STATE::NEXT:
		switchToNextProfile();
		_switchState = SWITCH_STATE::NONE;
		break;
	default:
		break;
	}
}

/**********************************************************************************************//**
 * @fn  void ProfileManager::connected()
 *
 * @brief   Connected this object.
 *
 * @date    3/22/2016
 **************************************************************************************************/

void ProfileManager::connected()
{
    String command = String("ChangedToDirectory ") + File::addTrailingSeparator(_profileLocation.getFullPathName()) + String("\n");
	if (m_lr_IPC_OUT)
	{
		m_lr_IPC_OUT->sendCommand(command);
	}
}

/**********************************************************************************************//**
 * @fn  void ProfileManager::disconnected()
 *
 * @brief   Disconnect from the ed.
 *
 * @date    3/22/2016
 **************************************************************************************************/

void ProfileManager::disconnected()
{

}

/**********************************************************************************************//**
 * @fn  void ProfileManager::Init(LR_IPC_OUT *out, CommandMap *commandMap, MIDIProcessor *midiProcessor)
 *
 * @brief   S.
 *
 * @date    3/22/2016
 *
 * @param [in,out]  out             If non-null, the out.
 * @param [in,out]  commandMap      If non-null, the command map.
 * @param [in,out]  midiProcessor   If non-null, the MIDI processor.
 **************************************************************************************************/

void ProfileManager::Init(std::shared_ptr<LR_IPC_OUT> out, std::shared_ptr<CommandMap> commandMap, std::shared_ptr<MIDIProcessor> midiProcessor)
{
	//copy the pointers
	m_commandMap = commandMap;
	m_lr_IPC_OUT = out;

	if (m_lr_IPC_OUT)
	{
		// add ourselves as a listener to LR_IPC_OUT so that we can send plugin settings on connection
		m_lr_IPC_OUT->addListener(this);
	}

	if (midiProcessor)
	{
		midiProcessor->addMIDICommandListener(this);
	}
}
