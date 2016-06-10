/*
  ==============================================================================

    LR_IPC_In.cpp

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
#include "LR_IPC_In.h"
#include <bitset>

constexpr auto kLrInPort = 58764;

LR_IPC_IN::LR_IPC_IN(): StreamingSocket{}, Thread{"LR_IPC_IN"} {}

LR_IPC_IN::~LR_IPC_IN() {
  stopTimer();
  stopThread(1000);
  close();
  command_map_.reset();
  profile_manager_.reset();
  midi_sender_.reset();
}

void LR_IPC_IN::PleaseStopThread() {
  signalThreadShouldExit();
  notify();
}

void LR_IPC_IN::timerCallback() {
  std::lock_guard< decltype(in_timer_) > lock(in_timer_);
  if (!isConnected()) {
    if (connect("127.0.0.1", kLrInPort, 100))
      if (!thread_started_) {
        startThread(); //avoid starting thread during shutdown
        thread_started_ = true;
      }
  }
}

void LR_IPC_IN::Init(std::shared_ptr<CommandMap>& map_command,
  std::shared_ptr<ProfileManager>& profile_manager,
  std::shared_ptr<MIDISender>& midi_sender) noexcept {
  command_map_ = map_command;
  profile_manager_ = profile_manager;
  midi_sender_ = midi_sender;
  //start the timer
  startTimer(1000);
}

void LR_IPC_IN::run() {
  while (!threadShouldExit()) {
    //if not connected, executes a wait 333 then goes back to while
    //if connected, tries to read a line, checks thread status and connection
    //status before each read attempt
    //doesn't terminate thread if disconnected, as currently don't have graceful
    //way to restart thread
    if (!isConnected()) {
      wait(333);
    } //end if (is not connected)
    else {
      constexpr auto kBufferSize = 256;
      char line[kBufferSize + 1] = {'\0'};//plus one for \0 at end
      auto size_read = 0;
      auto can_read_line = true;
      // parse input until we have a line, then process that line, quit if
      // connection lost
      while (!juce::String(line).endsWithChar('\n') && isConnected()) {
        if (threadShouldExit())
          goto threadExit;//break out of nested whiles
        auto wait_status = waitUntilReady(true, 0);
        switch (wait_status) {
          case -1:
            can_read_line = false;
            goto dumpLine; //read line failed, break out of switch and while         
          case 0:
            wait(100);
            break; //try again to read until char shows up
          case 1:
            if (size_read == kBufferSize)
              throw std::out_of_range("Buffer overflow in LR_IPC_IN");
            size_read += read(line + size_read, 1, false);
            break;
          default:
            throw std::invalid_argument("waitUntilReady returned unexpected value");
        }
      } // end while !\n and is connected

      // if lose connection, line may not be terminated
      if (can_read_line && juce::String(line).endsWithChar('\n')) {
        juce::String param{line};
        processLine(param);
      }
    dumpLine: /* empty statement */;
    } //end else (is connected)
  } //while not threadshouldexit
threadExit: /* empty statement */;
  std::lock_guard< decltype(in_timer_) > lock(in_timer_);
  stopTimer();
  //thread_started_ = false; //don't change flag while depending upon it
}

void LR_IPC_IN::processLine(const juce::String& line) {
    // process input into [parameter] [Value]
  const auto trimmed_line = line.trim();
  const auto command = trimmed_line.upToFirstOccurrenceOf(" ", false, false);
  const auto value_string = trimmed_line.fromFirstOccurrenceOf(" ", false, false);
  const auto value = value_string.getIntValue();

  if (command_map_) {
    if (command == juce::String{"SwitchProfile"}) {
      if (profile_manager_) {
        profile_manager_->switchToProfile(value_string);
      }
    }
    else if (command == juce::String{"SendKey"}) {
      std::bitset<3> modifiers{static_cast<decltype(modifiers)>(value)};
      std::string str{value_string.trimCharactersAtStart("0123456789 ").toStdString()};
      send_keys_.SendKeyDownUp(str, modifiers[0], modifiers[1], modifiers[2]);
    }
    else {
        // store updates in map
      parameter_map_[command] = value;

      // send associated CC messages to MIDI OUT devices
      if (command_map_->commandHasAssociatedMessage(command)) {
        const auto& msg = command_map_->getMessageForCommand(command);

        if (midi_sender_) {
          midi_sender_->sendCC(msg.channel, msg.controller, value);
        }
      }
    }
  }
}

void LR_IPC_IN::refreshMIDIOutput() {
  if (command_map_) {
      // send associated CC messages to MIDI OUT devices
    for (auto map_entry : parameter_map_) {
      if ((command_map_->commandHasAssociatedMessage(map_entry.first)) &&
        (midi_sender_)) {
        const auto& msg = command_map_->getMessageForCommand(map_entry.first);
        midi_sender_->sendCC(msg.channel, msg.controller, map_entry.second);
      }
    }
  }
}