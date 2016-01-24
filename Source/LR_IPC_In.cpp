/*
  ==============================================================================

	LR_IPC_In.cpp
	Created: 22 Aug 2015 9:44:03pm
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
#include "LR_IPC_In.h"
#include "MIDISender.h"
#include "ProfileManager.h"
#include "Logger.h"

//define the communication port
#define LR_IN_PORT 58764

LR_IPC_IN::LR_IPC_IN() : StreamingSocket(),
Thread("LR_IPC_IN"), m_commandMap(NULL)
{
	startTimer(1000);
	MIDISender::getInstance(); // enumerate MIDI output devices
}

void LR_IPC_IN::shutdown()
{
	stopTimer();
	stopThread(1000);
	close();
}

void LR_IPC_IN::timerCallback()
{
	if (!isConnected())
	{
		if (connect("127.0.0.1", LR_IN_PORT, 100))
			startThread();
	}
}

void LR_IPC_IN::Init(CommandMap * mapCommand)
{
	m_commandMap = mapCommand;
}

void LR_IPC_IN::run()
{
	while (!threadShouldExit())
	{
		char line[256] = { '\0' };
		int sizeRead = 0;
		bool canReadLine = true;

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
			String param(line);
			processLine(param);
		}
	}
}

void LR_IPC_IN::processLine(const String& line)
{	
	// process input into [parameter] [Value]
	line.trimEnd();
	String command = line.upToFirstOccurrenceOf(" ", false, false);
	String valueString = line.replace(line.upToFirstOccurrenceOf(" ", true, true), "", true);
	auto value = valueString.getIntValue();

	if (m_commandMap)
	{

		if (command == String("SwitchProfile"))
		{
			ProfileManager::getInstance().switchToProfile(valueString.trim());
		}
		else
		{

			// store updates in map
			parameterMap[command] = value;

			// send associated CC messages to MIDI OUT devices
			if (m_commandMap->commandHasAssociatedMessage(command))
			{
				const MIDI_Message& msg = m_commandMap->getMessageForCommand(command);
				MIDISender::getInstance().sendCC(msg.channel, msg.controller, value);
			}
		}
	}
}

void LR_IPC_IN::refreshMIDIOutput()
{
	if (m_commandMap)
	{
		// send associated CC messages to MIDI OUT devices
		for (auto mapEntry : parameterMap)
		{
			if (m_commandMap->commandHasAssociatedMessage(mapEntry.first))
			{
				const MIDI_Message& msg = m_commandMap->getMessageForCommand(mapEntry.first);
				MIDISender::getInstance().sendCC(msg.channel, msg.controller, mapEntry.second);
			}
		}
	}
}