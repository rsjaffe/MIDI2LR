/*
  ==============================================================================

    CommandMap.cpp

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

#include "CommandMap.h"
#include "LRCommands.h"

CommandMap::CommandMap() noexcept : Subject{} {}

void CommandMap::addCommandforMessage(unsigned int command, const MIDI_Message &message) {
    // adds a msg to the msg:command map, and it's associated command to the command:msg map
  if (command < LRCommandList::LRStringList.size()) {
    message_map_[message] = LRCommandList::LRStringList[command];
    command_string_map_[LRCommandList::LRStringList[command]] = message;
  }
  else
    message_map_[message] = LRCommandList::NextPrevProfile[command - LRCommandList::LRStringList.size()];
}

void CommandMap::addCommandforMessage(const String& command, const MIDI_Message &message) {
  message_map_[message] = command;
  command_string_map_[command] = message;
}

const String& CommandMap::getCommandforMessage(const MIDI_Message &message) const {
  return message_map_.at(message);
}

const MIDI_Message& CommandMap::getMessageForCommand(const String &command) const {
  return command_string_map_.at(command);
}

bool CommandMap::messageExistsInMap(const MIDI_Message &message) const {
  return message_map_.count(message) > 0 ? true : false;
}

bool CommandMap::commandHasAssociatedMessage(const String &command) const {
  return command_string_map_.count(command) > 0 ? true : false;
}

void CommandMap::removeMessage(const MIDI_Message &message) {
    // removes msg from the msg:command map, and it's associated command from the command:msg map
  command_string_map_.erase(message_map_[message]);
  message_map_.erase(message);
}

void CommandMap::clearMap() noexcept {
  command_string_map_.clear();
  message_map_.clear();
}

void CommandMap::toXMLDocument(File& file) const {
    // save the contents of the command map to an xml file
  XmlElement root{"settings"};
  for (auto map_entry : message_map_) {
    auto* setting = new XmlElement{"setting"};
    setting->setAttribute("channel", map_entry.first.channel);

    setting->setAttribute("NRPN", (map_entry.first.isNRPN) ? "True" : "False");
    setting->setAttribute("Relative", (map_entry.first.isRelative) ? "True" : "False");

    if (map_entry.first.isCC)
      setting->setAttribute("controller", map_entry.first.controller);
    else
      setting->setAttribute("note", map_entry.first.pitch);

    setting->setAttribute("command_string", map_entry.second);

    root.addChildElement(setting);
  }

  if (!root.writeToFile(file, ""))
      // Give feedback if file-save doesn't work
    AlertWindow::showMessageBox(AlertWindow::WarningIcon, "File Save Error", 
    "Unable to save file as specified. Please try again, and consider saving to a different location.");
}