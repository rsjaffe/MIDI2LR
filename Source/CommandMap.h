#pragma once
/*
  ==============================================================================

    CommandMap.h

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

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

#ifndef MIDI2LR_COMMANDMAP_H_INCLUDED
#define MIDI2LR_COMMANDMAP_H_INCLUDED

#include <map>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include "../JuceLibraryCode/JuceHeader.h"
#include "CommandSet.h"
#include "Misc.h"
#include "MidiUtilities.h"

class CommandMap {
 public:
   // adds an entry to the message:command map, and a corresponding entry to the
   // command:message map will look up the string by the index (but it is
   // preferred to directly use the string)
   void AddCommandforMessage(size_t command, const rsj::MidiMessageId& message);

   // adds an entry to the message:command map, and a corresponding entry to the
   // command:message map
   void AddCommandforMessage(const std::string& command, const rsj::MidiMessageId& message);

   // gets the LR command associated to a MIDI message
   [[nodiscard]] const std::string& GetCommandforMessage(const rsj::MidiMessageId& message) const;

   // in the command:message map
   // removes a MIDI message from the message:command map, and it's associated
   // entry
   void RemoveMessage(const rsj::MidiMessageId& message);

   // clears both message:command and command:message maps
   void ClearMap();

   // returns true if there is a mapping for a particular MIDI message
   [[nodiscard]] bool MessageExistsInMap(const rsj::MidiMessageId& message) const;

   std::vector<const rsj::MidiMessageId*> GetMessagesForCommand(const std::string& command) const;
   // gets the MIDI message associated to a LR command

   // returns true if there is a mapping for a particular LR command
   [[nodiscard]] bool CommandHasAssociatedMessage(const std::string& command) const;

   // saves the message:command map as an XML file
   void ToXmlDocument(const juce::File& file) const;

 private:
   mutable std::shared_mutex cmdmap_mutex_;
   CommandSet command_set_{};
   std::multimap<std::string, rsj::MidiMessageId> command_string_map_;
   std::unordered_map<rsj::MidiMessageId, std::string> message_map_;
};

inline void CommandMap::AddCommandforMessage(
    const std::string& command, const rsj::MidiMessageId& message)
{
   auto guard = std::unique_lock{cmdmap_mutex_};
   message_map_[message] = command;
   command_string_map_.emplace(command, message);
}

inline const std::string& CommandMap::GetCommandforMessage(const rsj::MidiMessageId& message) const
{
   try {
      auto guard = std::shared_lock{cmdmap_mutex_};
      return message_map_.at(message);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

inline void CommandMap::RemoveMessage(const rsj::MidiMessageId& message)
{
   // removes message from the message:command map, and its associated command
   // from the command:message map
   auto guard = std::unique_lock{cmdmap_mutex_};
   command_string_map_.erase(message_map_.at(message));
   message_map_.erase(message);
}

inline void CommandMap::ClearMap()
{
   auto guard = std::unique_lock{cmdmap_mutex_};
   command_string_map_.clear();
   message_map_.clear();
}

inline bool CommandMap::MessageExistsInMap(const rsj::MidiMessageId& message) const
{
   auto guard = std::shared_lock{cmdmap_mutex_};
   return message_map_.find(message) != message_map_.end();
}

inline bool CommandMap::CommandHasAssociatedMessage(const std::string& command) const
{
   auto guard = std::shared_lock{cmdmap_mutex_};
   return command_string_map_.find(command) != command_string_map_.end();
}
#endif // COMMANDMAP_H_INCLUDED
