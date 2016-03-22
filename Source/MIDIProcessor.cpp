/*
  ==============================================================================

    MIDIProcessor.cpp
    Created: 31 Jul 2015 11:56:19pm
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
#include "MIDIProcessor.h"
#include "Tools.h"

/**********************************************************************************************//**
 * @fn  MIDIProcessor::MIDIProcessor()
 *
 * @brief   Default constructor.
 *
 *
 * @date    3/21/2016
 **************************************************************************************************/

MIDIProcessor::MIDIProcessor()
{
    
}

/**********************************************************************************************//**
 * @fn  MIDIProcessor::~MIDIProcessor()
 *
 * @brief   Destructor.
 *
 *
 * @date    3/21/2016
 **************************************************************************************************/

MIDIProcessor::~MIDIProcessor()
{

}

/**********************************************************************************************//**
 * @fn  void MIDIProcessor::Init(void)
 *
 * @brief   S this object.
 *
 *
 * @date    3/21/2016
 **************************************************************************************************/

void MIDIProcessor::Init(void)
{
	initDevices();
}

/**********************************************************************************************//**
 * @fn  void MIDIProcessor::initDevices()
 *
 * @brief   Init devices.
 *
 *
 * @date    3/21/2016
 **************************************************************************************************/

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

/**********************************************************************************************//**
 * @fn  void MIDIProcessor::rescanDevices()
 *
 * @brief   Rescan devices.
 *
 *
 * @date    3/21/2016
 **************************************************************************************************/

void MIDIProcessor::rescanDevices()
{
    for (auto dev : _devices)
        dev->stop();
    _devices.clear(true);

    initDevices();
}

/**********************************************************************************************//**
 * @fn  void MIDIProcessor::handleIncomingMidiMessage(MidiInput * UNUSED_ARG(device), const MidiMessage &msg)
 *
 * @brief   Handles the incoming MIDI message.
 *
 *
 * @date    3/21/2016
 *
 * @param [in,out]  UNUSED_ARG(device)  If non-null, the unused argument (device)
 * @param   msg                         The message.
 **************************************************************************************************/

void MIDIProcessor::handleIncomingMidiMessage(MidiInput * UNUSED_ARG(device), const MidiMessage &msg)
{
	if (msg.isController())
	{
		for (auto listener : _listeners)
		{
			listener->handleMidiCC(msg.getChannel(), msg.getControllerNumber(), msg.getControllerValue());
		}
	}
	else if (msg.isNoteOn())
	{
		for (auto listener : _listeners)
		{
			listener->handleMidiNote(msg.getChannel(), msg.getNoteNumber());
		}
	}
}

/**********************************************************************************************//**
 * @fn  void MIDIProcessor::addMIDICommandListener(MIDICommandListener* listener)
 *
 * @brief   Adds a MIDI command listener.
 *
 *
 * @date    3/21/2016
 *
 * @param [in,out]  listener    If non-null, the listener.
 **************************************************************************************************/

void MIDIProcessor::addMIDICommandListener(MIDICommandListener* listener)
{
    _listeners.addIfNotAlreadyThere(listener);
}