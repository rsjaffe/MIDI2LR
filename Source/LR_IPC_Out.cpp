/*
  ==============================================================================

    LR_IPC_OUT.cpp

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
#include "LR_IPC_OUT.h"
#include "CommandMap.h"
#include "LRCommands.h"

constexpr auto kLrOutPort = 58763;

LR_IPC_OUT::LR_IPC_OUT(): InterprocessConnection() {}

LR_IPC_OUT::~LR_IPC_OUT() {
  {
    std::lock_guard<decltype(timer_mutex_)> lock(timer_mutex_);
    timer_off_ = true;
    stopTimer();
    disconnect();
  }
  command_map_.reset();
}

void LR_IPC_OUT::Init(std::shared_ptr<CommandMap>& command_map,
  std::shared_ptr<MIDIProcessor>& midi_processor) {
    //copy the pointer
  command_map_ = command_map;

  if (midi_processor) {
    midi_processor->addMIDICommandListener(this);
  }

  //start the timer
  startTimer(1000);
}

void LR_IPC_OUT::addListener(LRConnectionListener *listener) {
  listeners_.addIfNotAlreadyThere(listener);
}

void LR_IPC_OUT::sendCommand(const String &command) {
  {
    std::lock_guard<decltype(command_mutex_)> lock(command_mutex_);
    command_ += command;
  }
  triggerAsyncUpdate();
}

void LR_IPC_OUT::handleMidiCC(int midi_channel, int controller, int value) {
  MIDI_Message message{midi_channel, controller, CC};

  if (command_map_) {
    if (!command_map_->messageExistsInMap(message) ||
      command_map_->getCommandforMessage(message) == "Unmapped" ||
      find(LRCommandList::NextPrevProfile.begin(),
      LRCommandList::NextPrevProfile.end(),
      command_map_->getCommandforMessage(message)) != LRCommandList::NextPrevProfile.end())
      return;

    auto command_to_send = command_map_->getCommandforMessage(message);
    double computed_value = value;
    computed_value /= (controller < 128) ? 127.0 : 16383.0;
    
    command_to_send += String::formatted(" %g\n", computed_value);
    {
      std::lock_guard<decltype(command_mutex_)> lock(command_mutex_);
      command_ += command_to_send;
    }
    triggerAsyncUpdate();
  }
}

void LR_IPC_OUT::handleMidiNote(int midi_channel, int note) {
  MIDI_Message message{midi_channel, note, NOTE};

  if (command_map_) {
    if (!command_map_->messageExistsInMap(message) ||
      command_map_->getCommandforMessage(message) == "Unmapped" ||
      find(LRCommandList::NextPrevProfile.begin(),
      LRCommandList::NextPrevProfile.end(),
      command_map_->getCommandforMessage(message)) != LRCommandList::NextPrevProfile.end())
      return;

    auto command_to_send = command_map_->getCommandforMessage(message);
    command_to_send += String(" 1\n");
    {
      std::lock_guard<decltype(command_mutex_)> lock(command_mutex_);
      command_ += command_to_send;
    }
    triggerAsyncUpdate();
  }
}

void LR_IPC_OUT::handlePitchWheel(int midi_channel, int value) {
  MIDI_Message message{midi_channel, midi_channel, PITCHBEND};

  if (command_map_) {
    if (!command_map_->messageExistsInMap(message) ||
      command_map_->getCommandforMessage(message) == "Unmapped" ||
      find(LRCommandList::NextPrevProfile.begin(),
      LRCommandList::NextPrevProfile.end(),
      command_map_->getCommandforMessage(message)) != LRCommandList::NextPrevProfile.end())
      return;

    auto command_to_send = command_map_->getCommandforMessage(message);
    double computed_value = value;
    computed_value /= 15300.0; // ToDo: make setter for this to push in the setting from the SettingsManager
    
    command_to_send += String::formatted(" %g\n", computed_value);
    {
      std::lock_guard<decltype(command_mutex_)> lock(command_mutex_);
      command_ += command_to_send;
    }
    triggerAsyncUpdate();
  }
}

void LR_IPC_OUT::connectionMade() {
  for (auto listener : listeners_)
    listener->connected();
}

void LR_IPC_OUT::connectionLost() {
  for (auto listener : listeners_)
    listener->disconnected();
}

void LR_IPC_OUT::messageReceived(const MemoryBlock& /*msg*/) {}

void LR_IPC_OUT::handleAsyncUpdate() {
  String command_copy;
  {
    std::lock_guard<decltype(command_mutex_)> lock(command_mutex_);
    command_copy = std::move(command_); //JUCE::String swaps in this case
  }
    //check if there is a connection
  if (isConnected()) {
    getSocket()->write(command_copy.getCharPointer(), command_copy.length());
  }
}

void LR_IPC_OUT::timerCallback() {
  std::lock_guard<decltype(timer_mutex_)> lock(timer_mutex_);
  if (!isConnected() && !timer_off_)
    connectToSocket("127.0.0.1", kLrOutPort, 100);
}