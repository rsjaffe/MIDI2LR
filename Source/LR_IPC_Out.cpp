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

namespace {
  constexpr int kConnectTryTime = 100;
  constexpr char * kHost = "127.0.0.1";
  constexpr int kLrOutPort = 58763;
  constexpr double kMaxMIDI = 127.0;
  constexpr double kMaxNRPN = 16383.0;
  constexpr int kTimerInterval = 1000;
}

LR_IPC_OUT::LR_IPC_OUT(): juce::InterprocessConnection() {}

LR_IPC_OUT::~LR_IPC_OUT() {
  {
    std::lock_guard<decltype(timer_mutex_)> lock(timer_mutex_);
    timer_off_ = true;
    juce::Timer::stopTimer();
  }
  juce::InterprocessConnection::disconnect();
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
  juce::Timer::startTimer(kTimerInterval);
}

void LR_IPC_OUT::addListener(LRConnectionListener *listener) {
  for (const auto current_listener : listeners_)
    if (current_listener == listener)
      return; //don't add duplicates
  listeners_.push_back(listener);
}

void LR_IPC_OUT::sendCommand(const std::string& command) {
  {
    std::lock_guard<decltype(command_mutex_)> lock(command_mutex_);
    command_ += command;
  }
  juce::AsyncUpdater::triggerAsyncUpdate();
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

    auto command_to_send = command_map_->getCommandforMessage(message);
    double computed_value = value;
    computed_value /= (controller < 128) ? kMaxMIDI : kMaxNRPN;

    command_to_send += ' ' + std::to_string(computed_value) + '\n';
    {
      std::lock_guard<decltype(command_mutex_)> lock(command_mutex_);
      command_ += command_to_send;
    }
    juce::AsyncUpdater::triggerAsyncUpdate();
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

    auto command_to_send = command_map_->getCommandforMessage(message);
    command_to_send += " 1\n";
    {
      std::lock_guard<decltype(command_mutex_)> lock(command_mutex_);
      command_ += command_to_send;
    }
    juce::AsyncUpdater::triggerAsyncUpdate();
  }
}

void LR_IPC_OUT::connectionMade() {
  for (const auto& listener : listeners_)
    listener->connected();
}

void LR_IPC_OUT::connectionLost() {
  for (const auto& listener : listeners_)
    listener->disconnected();
}

void LR_IPC_OUT::messageReceived(const juce::MemoryBlock& /*msg*/) {}

void LR_IPC_OUT::handleAsyncUpdate() {
  std::string command_copy;
  {
    std::lock_guard<decltype(command_mutex_)> lock(command_mutex_);
    command_copy.swap(command_);
  }
    //check if there is a connection
  if (juce::InterprocessConnection::isConnected()) {
    juce::InterprocessConnection::getSocket()->
      write(command_copy.c_str(), command_copy.length());
  }
}

void LR_IPC_OUT::timerCallback() {
  std::lock_guard<decltype(timer_mutex_)> lock(timer_mutex_);
  if (!timer_off_ && !juce::InterprocessConnection::isConnected())
    juce::InterprocessConnection::connectToSocket(kHost, kLrOutPort, kConnectTryTime);
}