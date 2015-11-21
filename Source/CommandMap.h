/*
  ==============================================================================

	CommandMap.h
	Created: 31 Jul 2015 9:16:41pm
	Author:  Parth

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe, derived from code
by Parth.

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

// encapsulates a MIDI message (Note or CC)
struct MIDI_Message
{
	bool isCC;
	bool isNRPN;
	bool isRelative;
	int channel;
	union {
		int controller;
		int pitch;
		int data;
	};

	MIDI_Message() : isCC(0),
		isNRPN(0),
		isRelative(0),
		channel(0),
		data(0)

	{
	}

	MIDI_Message(int ch, int dat, bool iscc) : channel(ch),
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
namespace std {
	template <>
	struct hash<MIDI_Message>
	{
		std::size_t operator()(const MIDI_Message& k) const
		{
			return (std::hash<bool>()(k.isCC) ^ std::hash<int>()(k.channel) ^ (std::hash<int>()(k.data) << 1));
		}
	};

	template <>
	struct hash<String>
	{
		std::size_t operator()(const String& k) const
		{
			return k.hash();
		}
	};
}

class CommandMap
{
public:
	static const int UNMAPPED_INDEX;

	static CommandMap& getInstance();

	// adds an entry to the msg:command map, and a corresponding entry to the command:msg map
	// will look up the string by the index (but it is preferred to directly use the String)
	void addCommandforMessage(int command, const MIDI_Message &cc);

	// adds an entry to the msg:command map, and a corresponding entry to the command:msg map
	void addCommandforMessage(const String &command, const MIDI_Message &cc);

	// gets the LR command associated to a MIDI message
	const String& getCommandforMessage(const MIDI_Message &msg) const;

	// removes a MIDI message from the msg:command map, and it's associated entry in the command:msg map
	void removeMessage(const MIDI_Message &msg);

	// clears both msg:command and command:msg maps
	void clearMap();

	// returns true if there is a mapping for a particular MIDI message
	bool messageExistsInMap(const MIDI_Message &msg) const;

	// gets the MIDI message associated to a LR command
	const MIDI_Message& getMessageForCommand(const String &command) const;

	// returns true if there is a mapping for a particular LR command
	bool commandHasAssociatedMessage(const String &command) const;

	// saves the msg:command map as an XML file
	void toXMLDocument(File& file) const;

private:
	CommandMap();

	CommandMap(CommandMap const&) = delete;
	void operator=(CommandMap const&) = delete;

	std::unordered_map<MIDI_Message, String> _messageMap;
	std::unordered_map<String, MIDI_Message> _commandStringMap;
};


#endif  // COMMANDMAP_H_INCLUDED
