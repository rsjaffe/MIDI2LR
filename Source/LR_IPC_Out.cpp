// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
  ==============================================================================

    LR_IPC_OUT.cpp

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
#include "LR_IPC_OUT.h"
#include "CommandMap.h"
#include "ControlsModel.h"
#include "LRCommands.h"

namespace {
  constexpr int kConnectTryTime = 100;
  constexpr auto kHost = "127.0.0.1";
  constexpr int kLrOutPort = 58763;
  constexpr int kTimerInterval = 1000;
}

LR_IPC_OUT::LR_IPC_OUT(ControlsModel* c_model, CommandMap const * const mapCommand):
  controls_model_{c_model}, command_map_{mapCommand},
  juce::InterprocessConnection() {}

LR_IPC_OUT::~LR_IPC_OUT() {
  {
    std::lock_guard<decltype(timer_mutex_)> lock(timer_mutex_);
    timer_off_ = true;
    juce::Timer::stopTimer();
  }
  juce::InterprocessConnection::disconnect();
}

void LR_IPC_OUT::Init(
  std::shared_ptr<MIDIProcessor>& midi_processor) {
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

void LR_IPC_OUT::handleMIDI(RSJ::Message mm) {
  MessageType mt;
  switch (mm.MessageType) {//this is needed because mapping uses custom structure
    case RSJ::kCCFlag:
      mt = CC;
      break;
    case RSJ::kNoteOnFlag:
      mt = NOTE;
      break;
    case RSJ::kPWFlag:
      mt = PITCHBEND;
      break;
    default:
      assert(0);//shouldn't have non-handled message type
      mt = CC;
  }
//used to handling channel numbers 1-based
  MIDI_Message_ID message{mm.Channel + 1, mm.Number, mt};

  if (command_map_) {
    if (!command_map_->messageExistsInMap(message) ||
      command_map_->getCommandforMessage(message) == "Unmapped" ||
      find(LRCommandList::NextPrevProfile.begin(),
        LRCommandList::NextPrevProfile.end(),
        command_map_->getCommandforMessage(message)) != LRCommandList::NextPrevProfile.end())
      return;

    auto command_to_send = command_map_->getCommandforMessage(message);
    double computed_value = controls_model_->ControllerToPlugin(mm.MessageType, mm.Channel,
      mm.Number, mm.Value);
    command_to_send += ' ' + std::to_string(computed_value) + '\n';
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
      write(command_copy.c_str(), static_cast<int>(command_copy.length()));
  }
}

void LR_IPC_OUT::timerCallback() {
  std::lock_guard<decltype(timer_mutex_)> lock(timer_mutex_);
  if (!timer_off_ && !juce::InterprocessConnection::isConnected())
    juce::InterprocessConnection::connectToSocket(kHost, kLrOutPort, kConnectTryTime);
}