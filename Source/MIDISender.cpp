/*
  ==============================================================================

    MIDISender.cpp
    Created: 22 Aug 2015 9:44:21pm
    Author:  Parth

  ==============================================================================
*/

#include "MIDISender.h"
#include "SettingsManager.h"

MIDISender::MIDISender()
{
    initDevices();
    startTimer(SettingsManager::getInstance().getMIDISendDelay());
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

void MIDISender::timerCallback()
{
    if (!_ccMap.empty())
    {
        sendCC(_ccMap.begin()->first.channel, _ccMap.begin()->first.controller, _ccMap.begin()->second);
        _ccMap.erase(_ccMap.begin()->first);
    }
}

void MIDISender::queueCCForSending(int midi_channel, int controller, int value)
{
    MIDI_Message msg(midi_channel, controller, true);
    _ccMap[msg] = value;
}

void MIDISender::sendCC(int midi_channel, int controller, int value)
{
    for (auto dev : _outputDevices)
        dev->sendMessageNow(MidiMessage::controllerEvent(midi_channel, controller, value));
}