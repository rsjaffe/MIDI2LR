// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
  ==============================================================================

    MIDISender.cpp

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/
#include "MIDISender.h"
#include <gsl/gsl>

void MidiSender::Init()
{
    InitDevices();
}

void MidiSender::SendCc(int midi_channel, int controller, int value) const
{
    if (controller < 128) { // regular message
        for (const auto& dev : output_devices_)
            dev->sendMessageNow(juce::MidiMessage::controllerEvent(midi_channel, controller, value));
    }
    else { // NRPN
        const auto parameter_lsb = controller & 0x7f;
        const auto parameter_msb = (controller >> 7) & 0x7F;
        const auto value_lsb = value & 0x7f;
        const auto value_msb = (value >> 7) & 0x7F;
        for (const auto& dev : output_devices_) {
            dev->sendMessageNow(juce::MidiMessage::controllerEvent(midi_channel, 99, parameter_msb));
            dev->sendMessageNow(juce::MidiMessage::controllerEvent(midi_channel, 98, parameter_lsb));
            dev->sendMessageNow(juce::MidiMessage::controllerEvent(midi_channel, 6, value_msb));
            dev->sendMessageNow(juce::MidiMessage::controllerEvent(midi_channel, 38, value_lsb));
        }
    }
}

void MidiSender::SendNoteOn(int midi_channel, int controller, int value) const
{
    for (const auto& dev : output_devices_)
        dev->sendMessageNow(MidiMessage::noteOn(midi_channel, controller,
            gsl::narrow_cast<juce::uint8>(value)));
}

void MidiSender::SendPitchWheel(int midi_channel, int value) const
{
    for (const auto& dev : output_devices_)
        dev->sendMessageNow(MidiMessage::pitchWheel(midi_channel, value));
}

void MidiSender::RescanDevices()
{
    output_devices_.clear();
    InitDevices();
}

void MidiSender::InitDevices()
{
    for (auto idx = 0; idx < juce::MidiOutput::getDevices().size(); ++idx) {
        auto dev = juce::MidiOutput::openDevice(idx);
        if (dev)
            output_devices_.emplace_back(dev);
    }
}