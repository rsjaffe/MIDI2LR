#pragma once
/*
  ==============================================================================

    CommandMap.h

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

#ifndef COMMANDMAP_H_INCLUDED
#define COMMANDMAP_H_INCLUDED

#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include "../JuceLibraryCode/JuceHeader.h"
#include "MidiUtilities.h"

class CommandMap {
public:
    CommandMap() noexcept;
    virtual ~CommandMap() = default;
    CommandMap(const CommandMap&) = delete;
    CommandMap& operator=(const CommandMap&) = delete;

    // adds an entry to the message:command map, and a corresponding entry to the
    // command:message map will look up the string by the index (but it is preferred to
    // directly use the string)
    void addCommandforMessage(size_t command, const RSJ::MidiMessageId& cc);

    // adds an entry to the message:command map, and a corresponding entry to the
    // command:message map
    void addCommandforMessage(const std::string& command, const RSJ::MidiMessageId& cc);

    // gets the LR command associated to a MIDI message
    const std::string& getCommandforMessage(const RSJ::MidiMessageId& message) const;

    // in the command:message map
    // removes a MIDI message from the message:command map, and it's associated entry
    void removeMessage(const RSJ::MidiMessageId& message);

    // clears both message:command and command:message maps
    void clearMap() noexcept;

    // returns true if there is a mapping for a particular MIDI message
    bool messageExistsInMap(const RSJ::MidiMessageId& message) const;

    std::vector<const RSJ::MidiMessageId*> getMessagesForCommand(const std::string& command) const;
    // gets the MIDI message associated to a LR command

    // returns true if there is a mapping for a particular LR command
    bool commandHasAssociatedMessage(const std::string& command) const;

    // saves the message:command map as an XML file
    void toXMLDocument(const juce::File& file) const;

private:
    std::multimap<std::string, RSJ::MidiMessageId> command_string_map_;
    std::unordered_map<RSJ::MidiMessageId, std::string> message_map_;
};

inline void CommandMap::addCommandforMessage(const std::string& command, const RSJ::MidiMessageId& message)
{
    message_map_[message] = command;
    command_string_map_.insert({command, message});
}

inline const std::string& CommandMap::getCommandforMessage(const RSJ::MidiMessageId& message) const
{
    return message_map_.at(message);
}

inline void CommandMap::removeMessage(const RSJ::MidiMessageId& message)
{
    // removes message from the message:command map, and its associated command from
    // the command:message map
    command_string_map_.erase(message_map_[message]);
    message_map_.erase(message);
}

inline void CommandMap::clearMap() noexcept
{
    command_string_map_.clear();
    message_map_.clear();
}

inline bool CommandMap::messageExistsInMap(const RSJ::MidiMessageId& message) const
{
    return message_map_.find(message)!=message_map_.end();
}

inline bool CommandMap::commandHasAssociatedMessage(const std::string& command) const
{
    return command_string_map_.find(command)!=command_string_map_.end();
}
#endif  // COMMANDMAP_H_INCLUDED
