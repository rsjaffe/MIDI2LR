#pragma once
/*
  ==============================================================================

    MIDISender.h

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
#ifndef MIDISENDER_H_INCLUDED
#define MIDISENDER_H_INCLUDED
#include <memory>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"

class MIDISender {
public:
  MIDISender() noexcept;
  virtual ~MIDISender();
  void Init();

  // sends a CC message to all output devices
  void sendCC(int midi_channel, int controller, int value) const;
  // sends a PitchBend message to all output devices
  void sendPitchBend(int midi_channel, int value) const;

  // re-enumerates MIDI OUT devices
  void RescanDevices();

private:
  void InitDevices_();
  std::vector<std::unique_ptr<juce::MidiOutput>> output_devices_;
};

#endif  // MIDISENDER_H_INCLUDED
