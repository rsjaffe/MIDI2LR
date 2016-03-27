/*
  ==============================================================================

    MIDISender.cpp

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
#include "MIDISender.h"

/**********************************************************************************************//**
 * @fn  MIDISender::MIDISender() noexcept
 *
 * @brief   Default constructor.
 *
 * @date    3/22/2016
 **************************************************************************************************/

MIDISender::MIDISender() noexcept
{
   
}

/**********************************************************************************************//**
 * @fn  void MIDISender::initDevices()
 *
 * @brief   Init devices.
 *
 *
 * @date    3/22/2016
 **************************************************************************************************/

void MIDISender::initDevices()
{
    for (auto idx = 0; idx < MidiOutput::getDevices().size(); idx++)
    {
        auto dev = MidiOutput::openDevice(idx);
        if (dev != nullptr)
            _outputDevices.set(idx, dev);
    }
}

/**********************************************************************************************//**
 * @fn  void MIDISender::rescanDevices()
 *
 * @brief   Rescan devices.
 *
 *
 * @date    3/22/2016
 **************************************************************************************************/

void MIDISender::rescanDevices()
{
    _outputDevices.clear(true);
    initDevices();
}

/**********************************************************************************************//**
 * @fn  MIDISender::~MIDISender()
 *
 * @brief   Destructor.
 *
 *
 * @date    3/22/2016
 **************************************************************************************************/

MIDISender::~MIDISender()
{

}

/**********************************************************************************************//**
 * @fn  void MIDISender::sendCC(int midi_channel, int controller, int value) const
 *
 * @brief   Sends a MIDI control change.
 *
 * @date    3/22/2016
 *
 * @param   midi_channel    The MIDI channel.
 * @param   controller      The controller.
 * @param   value           The value.
 **************************************************************************************************/

void MIDISender::sendCC(int midi_channel, int controller, int value) const
{
    for (auto dev : _outputDevices)
        dev->sendMessageNow(MidiMessage::controllerEvent(midi_channel, controller, value));
}

/**********************************************************************************************//**
 * @fn  void MIDISender::Init(void)
 *
 * @brief   S this object.
 *
 *
 * @date    3/22/2016
 **************************************************************************************************/

void MIDISender::Init(void)
{
	initDevices();
}