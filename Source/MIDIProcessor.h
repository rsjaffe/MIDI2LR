#pragma once
/*
  ==============================================================================

    MIDIProcessor.h

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
#ifndef MIDIPROCESSOR_H_INCLUDED
#define MIDIPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

/**********************************************************************************************//**
* @class   MIDICommandListener
*
* @brief   MIDI Command Listener.
*
* @author  Rory Jaffe
* @date    3/20/2016
**************************************************************************************************/

class MIDICommandListener
{
public:
    virtual void handleMidiCC(int midiChannel, int controller, int value) = 0;
    virtual void handleMidiNote(int midiChannel, int note) = 0;

    virtual ~MIDICommandListener() {};
};

/**********************************************************************************************//**
* @class   MIDIProcessor
*
* @brief   MIDI Processor.
*
* @author  Rory Jaffe
* @date    3/20/2016
**************************************************************************************************/

class MIDIProcessor : public MidiInputCallback
{
public:
	MIDIProcessor();
	virtual ~MIDIProcessor();
    
    // overriden from MidiInputCallback
    void handleIncomingMidiMessage(MidiInput*, const MidiMessage&) override;

    void addMIDICommandListener(MIDICommandListener*);

    // re-enumerates MIDI IN devices
    void rescanDevices();
	void Init(void);
private:
   
    void initDevices();

    Array<MIDICommandListener *> _listeners;
    OwnedArray<MidiInput> _devices;
};


#endif  // MIDIPROCESSOR_H_INCLUDED
