#pragma once
/*
  ==============================================================================

    CommandMap.h

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

#ifndef COMMANDMAP_H_INCLUDED
#define COMMANDMAP_H_INCLUDED

#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include "../JuceLibraryCode/JuceHeader.h"

struct MIDI_Message_ID {
  bool isCC;
  int channel;
  union {
    int controller;
    int pitch;
    int data;
  };

  MIDI_Message_ID():
    isCC(0),
    channel(0),
    data(0)

  {}

  MIDI_Message_ID(int ch, int dat, bool iscc):
    isCC(iscc),
    channel(ch),
    data(dat) {}

  bool operator==(const MIDI_Message_ID& other) const noexcept {
    return (isCC == other.isCC && channel == other.channel && data == other.data);
  }

  bool operator<(const MIDI_Message_ID& other) const noexcept {
    if (channel < other.channel) return true;
    if (channel == other.channel) {
      if (data < other.data) return true;
      if (data == other.data && isCC && !other.isCC) return true;
    }
    return false;
  }
};

// hash functions
namespace std {
  template <>
  struct hash<MIDI_Message_ID> {
    std::size_t operator()(const MIDI_Message_ID& k) const noexcept {
      return std::hash<bool>()(k.isCC) ^ std::hash<int>()(k.channel) ^
        std::hash<int>()(k.data << 2);
    }
  };
}

class CommandMap {
public:
  CommandMap() noexcept;
  virtual ~CommandMap() {}

// adds an entry to the message:command map, and a corresponding entry to the
// command:message map will look up the string by the index (but it is preferred to
// directly use the string)
  void addCommandforMessage(size_t command, const MIDI_Message_ID& cc);

  // adds an entry to the message:command map, and a corresponding entry to the
  // command:message map
  void addCommandforMessage(const std::string& command, const MIDI_Message_ID& cc);

  // gets the LR command associated to a MIDI message
  const std::string& getCommandforMessage(const MIDI_Message_ID& message) const;

  // in the command:message map
  // removes a MIDI message from the message:command map, and it's associated entry
  void removeMessage(const MIDI_Message_ID& message);

  // clears both message:command and command:message maps
  void clearMap() noexcept;

  // returns true if there is a mapping for a particular MIDI message
  bool messageExistsInMap(const MIDI_Message_ID& message) const;

  std::vector<const MIDI_Message_ID*> getMessagesForCommand(const std::string& command) const;
  // gets the MIDI message associated to a LR command

  // returns true if there is a mapping for a particular LR command
  bool commandHasAssociatedMessage(const std::string& command) const;

  // saves the message:command map as an XML file
  void toXMLDocument(juce::File& file) const;

private:

  std::unordered_map<MIDI_Message_ID, std::string> message_map_;
  std::multimap<std::string, MIDI_Message_ID> command_string_map_;
};

inline void CommandMap::addCommandforMessage(const std::string& command, const MIDI_Message_ID& message) {
  message_map_[message] = command;
  command_string_map_.insert({command, message});
}

inline const std::string& CommandMap::getCommandforMessage(const MIDI_Message_ID& message) const {
  return message_map_.at(message);
}

inline void CommandMap::removeMessage(const MIDI_Message_ID& message) {
  // removes message from the message:command map, and its associated command from
  // the command:message map
  command_string_map_.erase(message_map_[message]);
  message_map_.erase(message);
}

inline void CommandMap::clearMap() noexcept {
  command_string_map_.clear();
  message_map_.clear();
}

inline bool CommandMap::messageExistsInMap(const MIDI_Message_ID& message) const {
  return message_map_.find(message) != message_map_.end();
}

inline bool CommandMap::commandHasAssociatedMessage(const std::string& command) const {
  return command_string_map_.find(command) != command_string_map_.end();
}
#endif  // COMMANDMAP_H_INCLUDED
