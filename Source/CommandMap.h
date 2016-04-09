#pragma once
/*
  ==============================================================================

    CommandMap.h

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

#ifndef COMMANDMAP_H_INCLUDED
#define COMMANDMAP_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <unordered_map>
#include "Pattern/Subject.h"

/**********************************************************************************************//**
 * @struct  MIDI_Message
 *
 * @brief   encapsulates a MIDI message (Note or CC)
 *
 *
 *
 **************************************************************************************************/

struct MIDI_Message
{
    bool isCC;
    bool isNRPN;
    bool isRelative;
    int channel;
    union
    {
        int controller;
        int pitch;
        int data;
    };

    MIDI_Message(): isCC(0),
        isNRPN(0),
        isRelative(0),
        channel(0),
        data(0)

    {}

    MIDI_Message(int ch, int dat, bool iscc): channel(ch),
        data(dat),
        isCC(iscc),
        isNRPN(0),
        isRelative(0)
    {}

    bool operator==(const MIDI_Message &other) const
    {
        return (isCC == other.isCC && channel == other.channel && data == other.data);
    }
};

// hash functions for MIDI_Message and String
namespace std
{

/**********************************************************************************************//**
 * @struct  std::hash<MIDI_Message>
 *
 * @brief   A hash for MIDI messages.
 *
 *
 *
 **************************************************************************************************/

    template <>
    struct hash<MIDI_Message>
    {
        std::size_t operator()(const MIDI_Message& k) const noexcept
        {
            return (std::hash<bool>()(k.isCC) ^ std::hash<int>()(k.channel) ^ (std::hash<int>()(k.data) << 1));
        }
    };

    /**********************************************************************************************//**
     * @struct  std::hash<String>
     *
     * @brief   A hash for Juce String.
     *
     *
     *
     **************************************************************************************************/

    template <>
    struct hash<String>
    {
        std::size_t operator()(const String& k) const noexcept
        {
            return k.hash();
        }
    };
}

/**********************************************************************************************//**
 * @class   CommandMap
 *
 * @brief   A command map.
 *
 *
 *
 **************************************************************************************************/

class CommandMap: public Subject
{
public:
    CommandMap() noexcept;
    virtual ~CommandMap()
    {}

// adds an entry to the msg:command map, and a corresponding entry to the command:msg map
// will look up the string by the index (but it is preferred to directly use the String)
    void addCommandforMessage(unsigned int command, const MIDI_Message &cc);

    // adds an entry to the msg:command map, and a corresponding entry to the command:msg map
    void addCommandforMessage(const String &command, const MIDI_Message &cc);

    // gets the LR command associated to a MIDI message
    const String& getCommandforMessage(const MIDI_Message &msg) const;

    // removes a MIDI message from the msg:command map, and it's associated entry in the command:msg map
    void removeMessage(const MIDI_Message &msg);

    // clears both msg:command and command:msg maps
    void clearMap() noexcept;

    // returns true if there is a mapping for a particular MIDI message
    bool messageExistsInMap(const MIDI_Message &msg) const;

    // gets the MIDI message associated to a LR command
    const MIDI_Message& getMessageForCommand(const String &command) const;

    // returns true if there is a mapping for a particular LR command
    bool commandHasAssociatedMessage(const String &command) const;

    // saves the msg:command map as an XML file
    void toXMLDocument(File& file) const;

private:

    std::unordered_map<MIDI_Message, String> _messageMap;
    std::unordered_map<String, MIDI_Message> _commandStringMap;
};


#endif  // COMMANDMAP_H_INCLUDED
