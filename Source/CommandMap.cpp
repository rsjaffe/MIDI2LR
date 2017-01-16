// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
  ==============================================================================

    CommandMap.cpp

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

#include "CommandMap.h"
#include "LRCommands.h"

CommandMap::CommandMap() noexcept {}

void CommandMap::addCommandforMessage(size_t command, const MIDI_Message_ID& message) {
    // adds a message to the message:command map, and its associated command to the
    // command:message map
  if (command < LRCommandList::LRStringList.size()) {
    message_map_[message] = LRCommandList::LRStringList[command];
    command_string_map_.insert({LRCommandList::LRStringList[command], message});
  }
  else
    message_map_[message] = LRCommandList::NextPrevProfile[command - LRCommandList::LRStringList.size()];
}

std::vector<const MIDI_Message_ID*> CommandMap::getMessagesForCommand(const std::string& command) const {
  std::vector<const MIDI_Message_ID*> mm;
  const auto range = command_string_map_.equal_range(command);
  for (auto it = range.first; it != range.second; ++it)
    mm.push_back(&it->second);
  return mm;
}

void CommandMap::toXMLDocument(const juce::File& file) const {
  if (message_map_.size()) {//don't bother if map is empty
    // save the contents of the command map to an xml file
    juce::XmlElement root{"settings"};
    for (const auto& map_entry : message_map_) {
      auto* setting = new juce::XmlElement{"setting"};
      setting->setAttribute("channel", map_entry.first.channel);
    switch (map_entry.first.messageType)
    {
      case NOTE: setting->setAttribute("note", map_entry.first.pitch);                break;
      case CC: setting->setAttribute("controller", map_entry.first.controller);       break;
      case PITCHBEND: setting->setAttribute("pitchbend", map_entry.first.controller); break;
    }
    setting->setAttribute("command_string", map_entry.second);
      root.addChildElement(setting);
    }
    if (!root.writeToFile(file, ""))
        // Give feedback if file-save doesn't work
      juce::AlertWindow::showMessageBox(juce::AlertWindow::WarningIcon, "File Save Error",
        "Unable to save file as specified. Please try again, and consider saving to a different location.");
  }
}