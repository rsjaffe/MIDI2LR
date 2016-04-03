/*
  ==============================================================================

    LR_IPC_OUT.cpp

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

constexpr auto LrOutPort = 58763;

/**********************************************************************************************//**
 * @fn  LR_IPC_OUT::LR_IPC_OUT()
 *
 * @brief   Default constructor.
 *
 *
 *
 **************************************************************************************************/

LR_IPC_OUT::LR_IPC_OUT(): InterprocessConnection()
{


}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::shutdown()
 *
 * @brief   Shuts down this object and frees any resources it is using.
 *
 *
 *
 **************************************************************************************************/

void LR_IPC_OUT::shutdown()
{
    stopTimer(),
        disconnect();
    m_commandMap.reset();
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::timerCallback()
 *
 * @brief   Callback, called when the timer.
 *
 *
 *
 **************************************************************************************************/

void LR_IPC_OUT::timerCallback()
{
    if (!isConnected())
        connectToSocket("127.0.0.1", LrOutPort, 100);
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::Init(std::shared_ptr<CommandMap>& mapCommand, std::shared_ptr<MIDIProcessor>& midiProcessor)
 *
 * @brief   S.
 *
 *
 *
 * @param [in,out]  mapCommand      If non-null, the map command.
 * @param [in,out]  midiProcessor   If non-null, the MIDI processor.
 **************************************************************************************************/

void LR_IPC_OUT::Init(std::shared_ptr<CommandMap>& mapCommand, std::shared_ptr<MIDIProcessor>& midiProcessor)
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

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::addListener(LRConnectionListener *listener)
 *
 * @brief   Adds a listener.
 *
 *
 *
 *
 * @param [in,out]  listener    If non-null, the listener.
 **************************************************************************************************/

void LR_IPC_OUT::addListener(LRConnectionListener *listener)
{
    _listeners.addIfNotAlreadyThere(listener);
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::connectionMade()
 *
 * @brief   Connection made.
 *
 *
 *
 **************************************************************************************************/

void LR_IPC_OUT::connectionMade()
{
    for (auto listener : _listeners)
        listener->connected();
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::connectionLost()
 *
 * @brief   Connection lost.
 *
 *
 *
 **************************************************************************************************/

void LR_IPC_OUT::connectionLost()
{
    for (auto listener : _listeners)
        listener->disconnected();
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::messageReceived(const MemoryBlock& )
 *
 * @brief   Message received.
 *
 *
 *
 * @param   msg  The message.
 **************************************************************************************************/

void LR_IPC_OUT::messageReceived(const MemoryBlock& /*msg*/)
{

}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::sendCommand(const String &command) const
 *
 * @brief   Sends a command.
 *
 *
 *
 * @param   command The command.
 **************************************************************************************************/

void LR_IPC_OUT::sendCommand(const String &command) const
{
    //check if there is a connection
    if (isConnected())
    {
        getSocket()->write(command.getCharPointer(), command.length());
    }
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::handleAsyncUpdate()
 *
 * @brief   Handles the asynchronous update.
 *
 *
 *
 **************************************************************************************************/

void LR_IPC_OUT::handleAsyncUpdate()
{
    //check if there is a connection
    if (isConnected())
    {
        auto command(_commandToSend + String::formatted(" %d\n", _valueToSend));
        sendCommand(command);
    }
}

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::handleMidiCC(int midiChannel, int controller, int value)
 *
 * @brief   Handles MIDI CC messages.
 *
 *
 *
 *
 * @param   midiChannel The MIDI channel.
 * @param   controller  The controller.
 * @param   value       The value.
 **************************************************************************************************/

void LR_IPC_OUT::handleMidiCC(int midiChannel, int controller, int value)
{
    MIDI_Message cc{ midiChannel, controller, true };

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

/**********************************************************************************************//**
 * @fn  void LR_IPC_OUT::handleMidiNote(int midiChannel, int note)
 *
 * @brief   Handles MIDI note messages.
 *
 *
 *
 *
 * @param   midiChannel The MIDI channel.
 * @param   note        The note.
 **************************************************************************************************/

void LR_IPC_OUT::handleMidiNote(int midiChannel, int note)
{
    MIDI_Message note_msg{ midiChannel, note, false };

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
