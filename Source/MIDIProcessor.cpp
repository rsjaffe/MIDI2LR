// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
  ==============================================================================

    MIDIProcessor.cpp

This file is part of MIDI2LR. Copyright 2015-2017 by Rory Jaffe.

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
#include "MIDIProcessor.h"
#include "MidiUtilities.h"

MIDIProcessor::MIDIProcessor() noexcept
{}

MIDIProcessor::~MIDIProcessor()
{}

void MIDIProcessor::Init()
{
    InitDevices_();
}

void MIDIProcessor::handleIncomingMidiMessage(juce::MidiInput * /*device*/,
    const juce::MidiMessage& message)
{
    const RSJ::MidiMessage mess{message};
    switch (mess.message_type_byte) {
    case RSJ::kCCFlag:
        if (nrpn_filter_.ProcessMidi(mess.channel, mess.number, mess.value)) { //true if nrpn piece
            auto nrpn = nrpn_filter_.GetNRPNifReady(mess.channel);
            if (nrpn.isValid) //send when finished
                for (const auto& cb : callbacks_)
                    cb(RSJ::MidiMessage{RSJ::kCCFlag, mess.channel, nrpn.control, nrpn.value});
        }
        else //regular message
            for (const auto& cb : callbacks_) {
                cb(mess);
            }
        break;
    case RSJ::kNoteOnFlag:
    case RSJ::kPWFlag:
        for (const auto& cb : callbacks_) {
            cb(mess);
        }
        break;
    default:
        ; //no action if other type of MIDI message
    }
}

void MIDIProcessor::RescanDevices()
{
    for (const auto& dev : devices_)
        dev->stop();
    devices_.clear();
    InitDevices_();
}

void MIDIProcessor::InitDevices_()
{
    for (auto idx = 0; idx < juce::MidiInput::getDevices().size(); ++idx) {
        const auto dev = juce::MidiInput::openDevice(idx, this);
        if (dev != nullptr) {
            devices_.emplace_back(dev);
            dev->start();
        }
    }
}