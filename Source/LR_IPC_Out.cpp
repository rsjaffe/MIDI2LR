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
#ifdef _WIN32
#include "Windows.h"
#endif
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

void LR_IPC_OUT::handleKPCommand() 
{
    if (_valueToSend == 127)
    { // for debugging, send lower case 'L' for all key presses, make table later
        handleShortCutKeyDown(KeyPress::createFromDescription("l"));
        handleShortCutKeyUp(KeyPress::createFromDescription("l"));
    }
}

void LR_IPC_OUT::handleAsyncUpdate()
{
	//check if there is a connection
	if (isConnected())
	{
        if (_commandToSend.startsWith("KP"))
            handleKPCommand();
        else
        {
            String command(_commandToSend + String::formatted(" %d\n", _valueToSend));
            sendCommand(command);
        }
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

void LR_IPC_OUT::handleShortCutKeyDownUp(KeyPress key)
{
#ifdef _WIN32
    // input event.
    INPUT ip;
    ModifierKeys mk = key.getModifiers();
    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;    
    if (mk.isCtrlDown())
    {
        ip.ki.wVk = VK_CONTROL;
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (mk.isShiftDown())
    {
        ip.ki.wVk = VK_SHIFT;
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (mk.isAltDown())
    {
        ip.ki.wVk = VK_MENU;
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
    }
    ip.ki.wVk = key.getKeyCode();
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));
    // Release the key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
    if (mk.isCtrlDown())
    {
        ip.ki.wVk = VK_CONTROL;
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (mk.isShiftDown())
    {
        ip.ki.wVk = VK_SHIFT;
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (mk.isAltDown())
    {
        ip.ki.wVk = VK_MENU;
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));
    }
#endif
}

void LR_IPC_OUT::handleShortCutKeyDown(KeyPress key)
{
#ifdef _WIN32
    // input event.
    INPUT ip;
    ModifierKeys mk = key.getModifiers();
    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    if (mk.isCtrlDown())
    {
        ip.ki.wVk = VK_CONTROL;
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (mk.isShiftDown())
    {
        ip.ki.wVk = VK_SHIFT;
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (mk.isAltDown())
    {
        ip.ki.wVk = VK_MENU;
        ip.ki.dwFlags = 0;
        SendInput(1, &ip, sizeof(INPUT));
    }
    ip.ki.wVk = key.getKeyCode();
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));
#endif
}

void LR_IPC_OUT::handleShortCutKeyUp(KeyPress key)
{
#ifdef _WIN32
    // input event.
    INPUT ip;
    ModifierKeys mk = key.getModifiers();
    // Set up a generic keyboard event.
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    ip.ki.wVk = key.getKeyCode();
    // Release the key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
    if (mk.isCtrlDown())
    {
        ip.ki.wVk = VK_CONTROL;
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (mk.isShiftDown())
    {
        ip.ki.wVk = VK_SHIFT;
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));
    }
    if (mk.isAltDown())
    {
        ip.ki.wVk = VK_MENU;
        ip.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &ip, sizeof(INPUT));
    }
#endif
}