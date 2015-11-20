/*
  ==============================================================================

	LR_IPC_In.cpp
	Created: 22 Aug 2015 9:44:03pm
	Author:  Parth

  ==============================================================================
*/

#include "LR_IPC_In.h"
#include "LRCommands.h"
#include "MIDISender.h"

const int LR_IPC_IN::LR_IN_PORT = 58764;

LR_IPC_IN& LR_IPC_IN::getInstance()
{
	static LR_IPC_IN instance;
	return instance;
}

LR_IPC_IN::LR_IPC_IN() : StreamingSocket(),
Thread("LR_IPC_IN")
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
    if (command == "ClearPresets") //can't do switch statement on String type
        LRCommandList::ClearPresets();
    else if (command == "AddPreset")
    {
        String presetInformation = line.fromFirstOccurrenceOf(" ", false, false);
        String presetID = presetInformation.upToFirstOccurrenceOf("\t", false, false);
        String presetLabel = presetInformation.fromFirstOccurrenceOf("\t", false, false);
        LRCommandList::AddPreset(presetID,presetLabel);
    }
    else
    {
        String valueString = line.replace(line.upToFirstOccurrenceOf(" ", true, true), "", true);
        auto value = valueString.getIntValue();

        // store updates in map
        parameterMap[command] = value;

        // send associated CC messages to MIDI OUT devices
        if (CommandMap::getInstance().commandHasAssociatedMessage(command))
        {
            const MIDI_Message& msg = CommandMap::getInstance().getMessageForCommand(command);
            MIDISender::getInstance().sendCC(msg.channel, msg.controller, value);
        }
    }
}

void LR_IPC_IN::refreshMIDIOutput()
{
	// send associated CC messages to MIDI OUT devices
	for (auto mapEntry : parameterMap)
	{
		if (CommandMap::getInstance().commandHasAssociatedMessage(mapEntry.first))
		{
			const MIDI_Message& msg = CommandMap::getInstance().getMessageForCommand(mapEntry.first);
			MIDISender::getInstance().sendCC(msg.channel, msg.controller, mapEntry.second);
		}
	}
}