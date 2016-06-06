/*
  ==============================================================================

    LR_IPC_OUT.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

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

void LR_IPC_OUT::shutdown() {
  stopTimer(),
    disconnect();
  command_map_.reset();
}

void LR_IPC_OUT::timerCallback() {
  if (!isConnected())
    connectToSocket("127.0.0.1", kLrOutPort, 100);
}

void LR_IPC_OUT::Init(std::shared_ptr<CommandMap>& map_command, 
  std::shared_ptr<MIDIProcessor>& midi_processor) {
    //copy the pointer
  command_map_ = map_command;

  if (midi_processor) {
    midi_processor->addMIDICommandListener(this);
  }

  //start the timer
  startTimer(1000);
}

void LR_IPC_OUT::addListener(LRConnectionListener *listener) {
  listeners_.addIfNotAlreadyThere(listener);
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

void LR_IPC_OUT::sendCommand(const String &command) const {
    //check if there is a connection
  if (isConnected()) {
    getSocket()->write(command.getCharPointer(), command.length());
  }
}

void LR_IPC_OUT::handleAsyncUpdate() {
    //check if there is a connection
  if (isConnected()) {
    auto command(command_to_send_ + String::formatted(" %d\n", value_to_send_));
    sendCommand(command);
  }
}

void LR_IPC_OUT::handleMidiCC(int midi_channel, int controller, int value) {
  MIDI_Message message{midi_channel, controller, true};

  if (command_map_) {
    if (!command_map_->messageExistsInMap(message) ||
      command_map_->getCommandforMessage(message) == "Unmapped" ||
      find(LRCommandList::NextPrevProfile.begin(),
      LRCommandList::NextPrevProfile.end(),
      command_map_->getCommandforMessage(message)) != LRCommandList::NextPrevProfile.end())
      return;

    command_to_send_ = command_map_->getCommandforMessage(message);
    value_to_send_ = value;
    handleAsyncUpdate();
  }
}

void LR_IPC_OUT::handleMidiNote(int midi_channel, int note) {
  MIDI_Message message{midi_channel, note, false};

  if (command_map_) {
    if (!command_map_->messageExistsInMap(message) ||
      command_map_->getCommandforMessage(message) == "Unmapped" ||
      find(LRCommandList::NextPrevProfile.begin(),
      LRCommandList::NextPrevProfile.end(),
      command_map_->getCommandforMessage(message)) != LRCommandList::NextPrevProfile.end())
      return;

    command_to_send_ = command_map_->getCommandforMessage(message);
    value_to_send_ = 127;
    handleAsyncUpdate();
  }
}