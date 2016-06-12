/*
  ==============================================================================

    MIDIProcessor.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

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

void MIDIProcessor::handleIncomingMidiMessage(MidiInput * /*device*/,
  const MidiMessage &message) {
  if (message.isController()) {
    for (auto listener : listeners_) {
      listener->handleMidiCC(message.getChannel(),
        message.getControllerNumber(), message.getControllerValue());
    }
  }
  else if (message.isNoteOn()) {
    for (auto listener : listeners_) {
      listener->handleMidiNote(message.getChannel(), message.getNoteNumber());
    }
  }
}

void MIDIProcessor::addMIDICommandListener(MIDICommandListener* listener) {
  listeners_.addIfNotAlreadyThere(listener);
}

void MIDIProcessor::rescanDevices() {
  for (auto dev : devices_)
    dev->stop();
  devices_.clear(true);

  InitDevices_();
}

void MIDIProcessor::InitDevices_() {
  for (auto idx = 0; idx < MidiInput::getDevices().size(); idx++) {
    if (devices_.set(idx, MidiInput::openDevice(idx, this))) {
      devices_[idx]->start();
      DBG(devices_[idx]->getName());
    }
  }
}
