/*
  ==============================================================================

	LR_IPC_OUT.cpp
	Created: 2 Aug 2015 12:27:47am
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
#include "LR_IPC_OUT.h"
#include "CommandMap.h"
#include "LRCommands.h"
#include "Tools.h"
#include "Windows.h"

// define the port used to 
#define LR_OUT_PORT 58763 

LR_IPC_OUT::LR_IPC_OUT() : InterprocessConnection()
{
		
}

void LR_IPC_OUT::shutdown()
{
	stopTimer();
	disconnect();
}

void LR_IPC_OUT::timerCallback()
{
	if (!isConnected())
		connectToSocket("127.0.0.1", LR_OUT_PORT, 100);
}

void LR_IPC_OUT::Init(CommandMap * mapCommand, MIDIProcessor *midiProcessor)
{
	//copy the pointer
	m_commandMap = mapCommand;

	if (midiProcessor)
	{
		midiProcessor->addMIDICommandListener(this);
	}


	//start the timer
	startTimer(1000);


}

void LR_IPC_OUT::addListener(LRConnectionListener *listener)
{
	_listeners.addIfNotAlreadyThere(listener);
}

void LR_IPC_OUT::connectionMade()
{
	for (auto listener : _listeners)
		listener->connected();
}

void LR_IPC_OUT::connectionLost()
{
	for (auto listener : _listeners)
		listener->disconnected();
}

void LR_IPC_OUT::messageReceived(const MemoryBlock& UNUSED_ARG(msg))
{

}

void LR_IPC_OUT::sendCommand(const String &command)
{
	//check if there is a connection
	if (isConnected())
	{
		getSocket()->write(command.getCharPointer(), command.length());
	}
}

void LR_IPC_OUT::handleAsyncUpdate()
{
	//check if there is a connection
	if (isConnected())
	{		
		String command(_commandToSend + String::formatted(" %d\n", _valueToSend));
		sendCommand(command);
	}
}

void LR_IPC_OUT::handleMidiCC(int midiChannel, int controller, int value)
{
	MIDI_Message cc(midiChannel, controller, true);

	if (m_commandMap)
	{
		if (!m_commandMap->messageExistsInMap(cc) ||
			m_commandMap->getCommandforMessage(cc) == "Unmapped" ||
			find(LRCommandList::NextPrevProfile.begin(),
				LRCommandList::NextPrevProfile.end(),
				m_commandMap->getCommandforMessage(cc)) != LRCommandList::NextPrevProfile.end())
			return;

		_commandToSend = m_commandMap->getCommandforMessage(cc);
		_valueToSend = value;
		
		// send a key command macro
		if ((_valueToSend == 127) && _commandToSend.startsWith("keys:"))
			handleKeyCommandMacro();
		else
			handleAsyncUpdate();
	}
}

void LR_IPC_OUT::handleMidiNote(int midiChannel, int note)
{
	MIDI_Message note_msg(midiChannel, note, false);

	if (m_commandMap)
	{

		if (!m_commandMap->messageExistsInMap(note_msg) ||
			m_commandMap->getCommandforMessage(note_msg) == "Unmapped" ||
			find(LRCommandList::NextPrevProfile.begin(),
				LRCommandList::NextPrevProfile.end(),
				m_commandMap->getCommandforMessage(note_msg)) != LRCommandList::NextPrevProfile.end())
			return;

		_commandToSend = m_commandMap->getCommandforMessage(note_msg);
		_valueToSend = 127;
		handleAsyncUpdate();
	}
}


void LR_IPC_OUT::handleKeyCommandMacro(void)
{
	int key, i, j;
	StringArray tokens;
	String strAllKeyCodes = _commandToSend.removeCharacters("keys:");
	// get all of the key commands
	tokens.addTokens(strAllKeyCodes, "+,", "\"");
	// get all of the characters that link the keys together, ie does the user want to press ctrl,alt,shift at same time, are ther multiple keys in a row to make up a macro
	// + signifies keys are held down
	// , indicates release this set of keys and then push more keys after these ones are released.  used to string multiple key strokes in sequence - ie creating a macro
	String strTerminate = strAllKeyCodes.retainCharacters("+,");

	int iKeyUP_start = 0, iKeyUP_end = -1;

	for (i = 0; i < tokens.size(); i++)
	{
		// press the key
		key = tokens[i].getHexValue32();
		handleShortcutKeyDown(key);

		if (i == tokens.size() - 1)
			iKeyUP_end = i + 1;
		else if (strTerminate[i] == ',')
			iKeyUP_end = i + 1;

		//release the keys in order, releasing all keys between each key sequence 
		for (j = iKeyUP_start; j < iKeyUP_end; j++)
		{
			key = tokens[j].getHexValue32();
			handleShortcutKeyUp(key);
			iKeyUP_start = j + 1;
		}
	}
}


void LR_IPC_OUT::handleShortcutKeyDown(int iKeyCode)
{

	// input event.
	INPUT ip;


	// Set up a generic keyboard event.
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0; // hardware scan code for key
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;
	ip.ki.wVk = iKeyCode;
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));
}

void LR_IPC_OUT::handleShortcutKeyUp(int iKeyCode)
{
	// input event.
	INPUT ip;

	// Set up a generic keyboard event.
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0; // hardware scan code for key
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;
	ip.ki.wVk = iKeyCode;
	//	ip.ki.dwFlags = 0; // 0 for key press
	// Release the key
	ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
	SendInput(1, &ip, sizeof(INPUT));
}