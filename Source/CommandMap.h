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
#include <unordered_map>
#include "../JuceLibraryCode/JuceHeader.h"
#include "Pattern/Subject.h"

enum MessageType{NOTE, CC, PITCHBEND};

struct MIDI_Message {
	MessageType messageType;
  int channel;
  union {
    int controller;
    int pitch;
    int data;
  };

  MIDI_Message(): messageType(NOTE),
    channel(0),
    data(0)

  {}

  MIDI_Message(int ch, int dat, MessageType msgType): channel(ch),
    messageType(msgType),
    data(dat) {}

  bool operator==(const MIDI_Message &other) const {
    return (messageType == other.messageType && channel == other.channel && data == other.data);
  }
};

// hash functions for MIDI_Message and String
namespace std {
  template <>
  struct hash<MIDI_Message> {
    std::size_t operator()(const MIDI_Message& k) const noexcept {
      return (std::hash<int>()(k.messageType) ^ std::hash<int>()(k.channel) ^
        (std::hash<int>()(k.data) << 1));
    }
  };

  template <>
  struct hash<String> {
    std::size_t operator()(const String& k) const noexcept {
      return k.hash();
    }
  };
}

class CommandMap: public Subject {
public:
  CommandMap() noexcept;
  virtual ~CommandMap() {}

// adds an entry to the message:command map, and a corresponding entry to the
// command:message map will look up the string by the index (but it is preferred to
// directly use the String)
  void addCommandforMessage(unsigned int command, const MIDI_Message &cc);

  // adds an entry to the message:command map, and a corresponding entry to the
  // command:message map
  void addCommandforMessage(const String &command, const MIDI_Message &cc);

  // gets the LR command associated to a MIDI message
  const String& getCommandforMessage(const MIDI_Message &message) const;

  // in the command:message map
  // removes a MIDI message from the message:command map, and it's associated entry
  void removeMessage(const MIDI_Message &message);

  // clears both message:command and command:message maps
  void clearMap() noexcept;

  // returns true if there is a mapping for a particular MIDI message
  bool messageExistsInMap(const MIDI_Message &message) const;

  // gets the MIDI message associated to a LR command
  const MIDI_Message& getMessageForCommand(const String &command) const;

  // returns true if there is a mapping for a particular LR command
  bool commandHasAssociatedMessage(const String &command) const;

  // saves the message:command map as an XML file
  void toXMLDocument(File& file) const;

private:

  std::unordered_map<MIDI_Message, String> message_map_;
  std::unordered_map<String, MIDI_Message> command_string_map_;
};

#endif  // COMMANDMAP_H_INCLUDED
