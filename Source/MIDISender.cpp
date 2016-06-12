/*
  ==============================================================================

    MIDISender.cpp

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
#include "MIDISender.h"

MIDISender::MIDISender() noexcept {}

MIDISender::~MIDISender() {}

void MIDISender::Init(void) {
  InitDevices_();
}

void MIDISender::sendCC(int midi_channel, int controller, int value) const {
  if (controller < 128) { // regular message
    for (auto dev : output_devices)
      dev->sendMessageNow(MidiMessage::controllerEvent(midi_channel, controller,
      value));
  }
  else { // NRPN
    const auto parameterLSB = static_cast<uint8_t>(controller & 0x7f);
    const auto parameterMSB = static_cast<uint8_t>(controller >> 7);
    const auto valueLSB = static_cast<uint8_t>(value & 0x7f);
    const auto valueMSB = static_cast<uint8_t>(value >> 7);
    for (auto dev : output_devices) {
      dev->sendMessageNow(MidiMessage::controllerEvent(midi_channel, 99, parameterMSB));
      dev->sendMessageNow(MidiMessage::controllerEvent(midi_channel, 98, parameterLSB));
      dev->sendMessageNow(MidiMessage::controllerEvent(midi_channel, 6, valueMSB));
      dev->sendMessageNow(MidiMessage::controllerEvent(midi_channel, 38, valueLSB));
    }
  }
}

void MIDISender::rescanDevices() {
  output_devices.clear(true);
  InitDevices_();
}

void MIDISender::InitDevices_() {
  for (auto idx = 0; idx < MidiOutput::getDevices().size(); idx++) {
    auto dev = MidiOutput::openDevice(idx);
    if (dev != nullptr)
      output_devices.set(idx, dev);
  }
}
