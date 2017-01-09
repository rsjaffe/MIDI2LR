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

MIDIProcessor::MIDIProcessor() noexcept {}

MIDIProcessor::~MIDIProcessor() {}

void MIDIProcessor::Init(void) {
  InitDevices_();
}

void MIDIProcessor::handleIncomingMidiMessage(juce::MidiInput * /*device*/,
  const juce::MidiMessage& message) {
  RSJ::Message mess(RSJ::ParseMidi(message));
  switch (mess.MessageType) {
    case RSJ::CCflag:
      if (nrpn_filter_.ProcessMidi(mess.Channel, mess.Number, mess.Value)) { //true if nrpn piece
        if (nrpn_filter_.IsReady(mess.Channel)) { //send when finished
          for (const auto listener : listeners_)
            listener->handleMIDI(RSJ::Message{RSJ::CCflag, mess.Channel,
              nrpn_filter_.GetControl(mess.Channel),
              nrpn_filter_.GetValue(mess.Channel)});
          nrpn_filter_.Clear(mess.Channel);
        }
      }
      else {//regular message
        for (const auto listener : listeners_)
          listener->handleMIDI(mess);
      }
      break;
    case RSJ::NoteOnFlag:
    case RSJ::PWflag:
      for (const auto listener : listeners_) {
        listener->handleMIDI(mess);
      }
      break;
  }
}

void MIDIProcessor::addMIDICommandListener(MIDICommandListener* listener) {
  for (const auto current_listener : listeners_)
    if (current_listener == listener)
      return; //don't add duplicates
  listeners_.push_back(listener);
}

void MIDIProcessor::RescanDevices() {
  for (const auto& dev : devices_)
    dev->stop();
  devices_.clear();

  InitDevices_();
}

void MIDIProcessor::InitDevices_() {
  for (auto idx = 0; idx < juce::MidiInput::getDevices().size(); ++idx) {
    const auto dev = juce::MidiInput::openDevice(idx, this);
    if (dev != nullptr) {
      devices_.emplace_back(dev);
      dev->start();
    }
  }
}