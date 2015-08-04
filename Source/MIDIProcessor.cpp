/*
  ==============================================================================

    MIDIProcessor.cpp
    Created: 31 Jul 2015 11:56:19pm
    Author:  Parth

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
    else if(msg.isNoteOnOrOff())
        for(auto listener : _listeners)
            listener->handleMidiNote(msg.getChannel(), msg.getNoteNumber());
}

void MIDIProcessor::addMIDICommandListener(MIDICommandListener* listener)
{
    _listeners.addIfNotAlreadyThere(listener);
}