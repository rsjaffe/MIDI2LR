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
  if (message.isController()) {
    const auto channel =
      static_cast<unsigned short int>(message.getChannel()); // 1-based
    const auto control =
      static_cast<unsigned short int>(message.getControllerNumber());
    const auto value =
      static_cast<unsigned short int>(message.getControllerValue());
    if (nrpn_filter_.ProcessMidi(channel, control, value)) { //true if nrpn piece
      if (nrpn_filter_.IsReady(channel)) { //send when finished
        for (const auto listener : listeners_)
          listener->handleMidiCC(channel, nrpn_filter_.GetControl(channel),
          nrpn_filter_.GetValue(channel));
        nrpn_filter_.Clear(channel);
      }
    }
    else //regular message
      for (const auto listener : listeners_)
        listener->handleMidiCC(channel, control, value);
  }
  else if (message.isNoteOn()) {
    for (const auto listener : listeners_) {
		  listener->handleMidiNote(message.getChannel(), message.getNoteNumber());
	  }
  }
  else if (message.isPitchWheel()) {
    const auto value =
      static_cast<unsigned short int>(message.getPitchWheelValue());
	  for (auto listener : listeners_) {
		  listener->handlePitchWheel(message.getChannel(), value);
	  }
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