/*
  ==============================================================================

    MIDIProcessor.cpp
    Created: 31 Jul 2015 11:56:19pm
    Author:  Parth, Jaffe

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe, derived from code
by Parth.

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
#include "MIDIProcessor.h"

MIDIProcessor& MIDIProcessor::getInstance()
{
    static MIDIProcessor instance;
    return instance;
}

MIDIProcessor::MIDIProcessor()
{
    initDevices();
}

void MIDIProcessor::initDevices()
{
    for(auto idx = 0; idx < MidiInput::getDevices().size(); idx++)
    {
        if(_devices.set(idx, MidiInput::openDevice(idx, this)))
        {
            _devices[idx]->start();
            DBG(_devices[idx]->getName());
        }
    }
}

void MIDIProcessor::rescanDevices()
{
    for (auto dev : _devices)
        dev->stop();
    _devices.clear(true);

    initDevices();
}

MIDIProcessor::~MIDIProcessor()
{
}

void MIDIProcessor::handleIncomingMidiMessage(MidiInput *device, const MidiMessage &msg)
{
    if(msg.isController())
        for(auto listener : _listeners)
            listener->handleMidiCC(msg.getChannel(), msg.getControllerNumber(), msg.getControllerValue());
    else if(msg.isNoteOn())
        for(auto listener : _listeners)
            listener->handleMidiNote(msg.getChannel(), msg.getNoteNumber());
}

void MIDIProcessor::addMIDICommandListener(MIDICommandListener* listener)
{
    _listeners.addIfNotAlreadyThere(listener);
}