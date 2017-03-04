#pragma once
/*
  ==============================================================================

    LR_IPC_OUT.h

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
#ifndef LR_IPC_OUT_H_INCLUDED
#define LR_IPC_OUT_H_INCLUDED

#include <mutex>
#include <string>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "MIDIProcessor.h"
#include "MidiUtilities.h"
#include "Utilities/Utilities.h"

class CommandMap;
class ControlsModel;

class LRConnectionListener {
public:
    // sent when a connection to the LR plugin is made
  virtual void connected() = 0;

  // sent if disconnected from the LR plugin
  virtual void disconnected() = 0;

  virtual ~LRConnectionListener() {};
///< .
};

class LR_IPC_OUT final:
  private juce::DeletedAtShutdown,
  private juce::InterprocessConnection,
  private juce::AsyncUpdater,
  private juce::Timer {
public:
  LR_IPC_OUT(ControlsModel* c_model, CommandMap const * const mapCommand);
  virtual ~LR_IPC_OUT();
  void Init(std::shared_ptr<MIDIProcessor>&  midiProcessor);

  void addListener(LRConnectionListener *listener);

  // sends a command to the plugin
  void sendCommand(const std::string& command);

  // MIDICommandListener interface
  virtual void handleMIDI(RSJ::Message);

private:
  // IPC interface
  virtual void connectionMade() override;
  virtual void connectionLost() override;
  virtual void messageReceived(const juce::MemoryBlock& msg) override;
  // AsyncUpdater interface
  virtual void handleAsyncUpdate() override;
  // Timer callback
  virtual void timerCallback() override;

  ControlsModel* const controls_model_;

  std::vector<LRConnectionListener *> listeners_;
  bool timer_off_{false};
  mutable RSJ::spinlock command_mutex_; //fast spinlock for brief use
  mutable std::mutex timer_mutex_; //fix race during shutdown
  CommandMap const * const command_map_;
  std::string command_;
};

#endif  // LR_IPC_OUT_H_INCLUDED
