#pragma once
/*
  ==============================================================================

	LR_IPC_OUT.h
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
#ifndef LR_IPC_OUT_H_INCLUDED
#define LR_IPC_OUT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "MIDIProcessor.h"
#include "CommandMap.h"

class LRConnectionListener
{
public:
	// sent when a connection to the LR plugin is made
	virtual void connected() = 0;

	// sent if disconnected from the LR plugin
	virtual void disconnected() = 0;

	virtual ~LRConnectionListener() {};
};

class LR_IPC_OUT : public InterprocessConnection,
	public MIDICommandListener,
	public AsyncUpdater,
	public Timer
{
public:
	LR_IPC_OUT();
	virtual ~LR_IPC_OUT() {};
	// closes the socket
	void shutdown();

	void addListener(LRConnectionListener *listener);

	// sends a command to the plugin
	void sendCommand(const String& command);

	// IPC interface
	virtual void connectionMade() override;
	virtual void connectionLost() override;
	virtual void messageReceived(const MemoryBlock& msg) override;

	// MIDICommandListener interface
	virtual void handleMidiCC(int midiChannel, int controller, int value) override;
	virtual void handleMidiNote(int midiChannel, int note) override;



	// AsyncUpdater interface
	virtual void handleAsyncUpdate() override;

	// Timer callback
	virtual void timerCallback() override;
	void Init(CommandMap *mapCommand, MIDIProcessor *midiProcessor);
private:
    const static unordered_map<String, KeyPress> KPMappings;
	CommandMap *m_commandMap;
	Array<LRConnectionListener *> _listeners;
	int _valueToSend;
	String _commandToSend;
    // Send key commands
    void handleKPCommand(void);
    void handleShortCutKeyDownUp(KeyPress key);
};


#endif  // LR_IPC_OUT_H_INCLUDED
