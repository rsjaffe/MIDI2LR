/*
  ==============================================================================

    MIDISender.cpp
    Created: 22 Aug 2015 9:44:21pm
    Author:  Parth

  ==============================================================================
*/

#include "MIDISender.h"

MIDISender::MIDISender()
{
    initDevices();
}

void MIDISender::initDevices()
{
    for (auto idx = 0; idx < MidiOutput::getDevices().size(); idx++)
    {
        auto dev = MidiOutput::openDevice(idx);
        if (dev != nullptr)
            _outputDevices.set(idx, dev);
    }
}

void MIDISender::rescanDevices()
{
    _outputDevices.clear(true);
    initDevices();
}

MIDISender& MIDISender::getInstance()
{
    static MIDISender instance;
    return instance;
}

MIDISender::~MIDISender()
{

}

void MIDISender::sendCC(int midi_channel, int controller, int value)
{
    for (auto dev : _outputDevices)
        if (controller > 0x7F || value > 0x7F) //requires NRPN message
        {
            dev->sendMessageNow(MidiMessage::controllerEvent(midi_channel, 99, controller >> 7)); //MSB Address
            dev->sendMessageNow(MidiMessage::controllerEvent(midi_channel, 98, controller & 0x7F)); //LSB Address
            dev->sendMessageNow(MidiMessage::controllerEvent(midi_channel, 6, value >> 7)); //MSB Data Byte
            dev->sendMessageNow(MidiMessage::controllerEvent(midi_channel, 38, value & 0x7F)); //LSB Data Byte
        }
        else
            dev->sendMessageNow(MidiMessage::controllerEvent(midi_channel, controller, value));
}