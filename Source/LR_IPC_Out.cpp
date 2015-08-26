/*
  ==============================================================================

    LR_IPC_OUT.cpp
    Created: 2 Aug 2015 12:27:47am
    Author:  Parth

  ==============================================================================
*/

#include "LR_IPC_OUT.h"
#include "CommandMap.h"
#include "LRCommands.h"

const int LR_IPC_OUT::LR_OUT_PORT = 58763;

LR_IPC_OUT& LR_IPC_OUT::getInstance()
{
    static LR_IPC_OUT instance;
    return instance;
}

LR_IPC_OUT::LR_IPC_OUT() : InterprocessConnection()
{
    MIDIProcessor::getInstance().addMIDICommandListener(this);
    startTimer(1000);
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

void LR_IPC_OUT::messageReceived(const MemoryBlock& msg)
{

}

void LR_IPC_OUT::sendCommand(String &command)
{
    getSocket()->write(command.getCharPointer(), command.length());
}

void LR_IPC_OUT::handleAsyncUpdate()
{
    if (!isConnected()) return;

    String command = _commandToSend + String::formatted(" %d\n", _valueToSend);
    sendCommand(command);
}

void LR_IPC_OUT::handleMidiCC(int midiChannel, int controller, int value)
{
    MIDI_Message cc(midiChannel, controller, true);

    if (CommandMap::getInstance().getCommandforMessage(cc) == 0) 
        return;

    _commandToSend = LRCommandList::LRStringList[(int)CommandMap::getInstance().getCommandforMessage(cc)];
    _valueToSend = value;
    handleAsyncUpdate();
}

void LR_IPC_OUT::handleMidiNote(int midiChannel, int note)
{
    MIDI_Message note_msg(midiChannel, note, false);

    if (CommandMap::getInstance().getCommandforMessage(note_msg) == 0)
        return;

    _commandToSend = LRCommandList::LRStringList[CommandMap::getInstance().getCommandforMessage(note_msg)];
    _valueToSend = 127;
    handleAsyncUpdate();
}