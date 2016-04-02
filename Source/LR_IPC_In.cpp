/*
  ==============================================================================

	LR_IPC_In.cpp

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
#include "LR_IPC_In.h"


/** @brief   define the communication port. */
constexpr auto LrInPort = 58764;

/**********************************************************************************************//**
 * @fn  LR_IPC_IN::LR_IPC_IN()
 *
 * @brief   Default constructor.
 *
 *
 **************************************************************************************************/

LR_IPC_IN::LR_IPC_IN(): StreamingSocket{},
Thread{ "LR_IPC_IN" }, m_commandMap{ nullptr }, m_profileManager{ nullptr }, m_midiSender{ nullptr }
{
	
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_IN::shutdown()
 *
 * @brief   Shuts down this object and frees any resources it is using.
 *
 *
 **************************************************************************************************/

void LR_IPC_IN::shutdown()
{
	stopTimer();
	stopThread(1000);
	close();
    m_commandMap.reset();
    m_profileManager.reset();
    m_midiSender.reset();
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_IN::timerCallback()
 *
 * @brief   Callback, called when the timer.
 *
 *
 **************************************************************************************************/

void LR_IPC_IN::timerCallback()
{
	if (!isConnected())
	{
		if (connect("127.0.0.1", LrInPort, 100))
			startThread();
	}
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_IN::Init(std::shared_ptr<CommandMap>& mapCommand, std::shared_ptr<ProfileManager>& profileManager, std::shared_ptr<MIDISender>& midiSender) noexcept
 *
 * @brief   S.
 *
 *
 *
 * @param [in,out]  mapCommand      If non-null, the map command.
 * @param [in,out]  profileManager  If non-null, manager for profile.
 * @param [in,out]  midiSender      If non-null, the MIDI sender.
 **************************************************************************************************/

void LR_IPC_IN::Init(std::shared_ptr<CommandMap>& mapCommand, std::shared_ptr<ProfileManager>& profileManager,
    std::shared_ptr<MIDISender>& midiSender) noexcept
{
	m_commandMap = mapCommand;
	m_profileManager = profileManager;
	m_midiSender = midiSender;
	//start the timer
	startTimer(1000);
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_IN::run()
 *
 * @brief   Runs this object.
 *
 *
 **************************************************************************************************/

void LR_IPC_IN::run()
{
	while (!threadShouldExit())
	{
		char line[256] = { '\0' };
		auto sizeRead = 0;
		auto canReadLine = true;

		// parse input until we have a line, then process that line
		while (!String(line).endsWithChar('\n') && !threadShouldExit())
		{
			auto waitStatus = waitUntilReady(true, 0);
			if (waitStatus < 0)
			{
				canReadLine = false;
				break;
			}
			else if (waitStatus == 0)
			{
				wait(100);
				continue;
			}
			sizeRead += read(line + sizeRead, 1, false);
		}

		if (canReadLine)
		{
            String param{ line };
			processLine(param);
		}
	}
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_IN::processLine(const String& line)
 *
 * @brief   Process the line described by line.
 *
 *
 *
 * @param   line    The line.
 **************************************************************************************************/

void LR_IPC_IN::processLine(const String& line)
{	
	// process input into [parameter] [Value]
	line.trimEnd();
	const auto command = line.upToFirstOccurrenceOf(" ", false, false);
	const auto valueString = line.replace(line.upToFirstOccurrenceOf(" ", true, true), "", true);
	const auto value = valueString.getIntValue();

	if (m_commandMap)
	{

        if (command == String{ "SwitchProfile" })
		{
			if (m_profileManager)
			{
				m_profileManager->switchToProfile(valueString.trim());
			}
		}
        else if (command == String{ "SendKey" })
        {
            m_SendKeys.SendKeyDownUp(KeyPress::createFromDescription(valueString.trim()));
        }
		else
		{

			// store updates in map
			parameterMap[command] = value;

			// send associated CC messages to MIDI OUT devices
			if (m_commandMap->commandHasAssociatedMessage(command))
			{
				const auto& msg = m_commandMap->getMessageForCommand(command);


				if (m_midiSender)
				{
					m_midiSender->sendCC(msg.channel, msg.controller, value);
				}				
			}
		}
	}
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_IN::refreshMIDIOutput()
 *
 * @brief   Refresh MIDI output.
 *
 *
 **************************************************************************************************/

void LR_IPC_IN::refreshMIDIOutput()
{
	if (m_commandMap)
	{
		// send associated CC messages to MIDI OUT devices
		for (auto mapEntry : parameterMap)
		{
			if ((m_commandMap->commandHasAssociatedMessage(mapEntry.first)) && (m_midiSender))
			{
				const auto& msg = m_commandMap->getMessageForCommand(mapEntry.first);
				m_midiSender->sendCC(msg.channel, msg.controller, mapEntry.second);
			}
		}
	}
}