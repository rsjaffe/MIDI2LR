/*
  ==============================================================================

	CommandMap.cpp
	Created: 31 Jul 2015 9:16:41pm
	Author:  Parth, Jaffe

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

CommandMap::CommandMap() : Subject()
{

}

void CommandMap::addCommandforMessage(unsigned int command, const MIDI_Message &msg)
{
	// adds a msg to the msg:command map, and it's associated command to the command:msg map
	if (command < LRCommandList::LRStringList.size())
	{
		_messageMap[msg] = LRCommandList::LRStringList[command];
		_commandStringMap[LRCommandList::LRStringList[command]] = msg;
	}
	else
		_messageMap[msg] = LRCommandList::NextPrevProfile[command - LRCommandList::LRStringList.size()];
}

void CommandMap::addCommandforMessage(const String& command, const MIDI_Message &msg)
{
	_messageMap[msg] = command;
	_commandStringMap[command] = msg;
}

const String& CommandMap::getCommandforMessage(const MIDI_Message &msg) const
{
	return _messageMap.at(msg);
}

const MIDI_Message& CommandMap::getMessageForCommand(const String &command) const
{
	return _commandStringMap.at(command);
}

bool CommandMap::messageExistsInMap(const MIDI_Message &msg) const
{	
	return _messageMap.count(msg) > 0 ? true : false;
}

bool CommandMap::commandHasAssociatedMessage(const String &command) const
{
	return _commandStringMap.count(command) > 0 ? true : false;
}

void CommandMap::removeMessage(const MIDI_Message &msg)
{
	// removes msg from the msg:command map, and it's associated command from the command:msg map
	_commandStringMap.erase(_messageMap[msg]);
	_messageMap.erase(msg);
}

void CommandMap::clearMap()
{
	_commandStringMap.clear();
	_messageMap.clear();
}

void CommandMap::toXMLDocument(File& file) const
{
	// save the contents of the command map to an xml file
	XmlElement root("settings");
	for (auto mapEntry : _messageMap)
	{
		XmlElement* setting = new XmlElement("setting");
		setting->setAttribute("channel", mapEntry.first.channel);


		setting->setAttribute("NRPN", (mapEntry.first.isNRPN) ? "True" : "False");
		setting->setAttribute("Relative", (mapEntry.first.isRelative) ? "True" : "False");

		if (mapEntry.first.isCC)
			setting->setAttribute("controller", mapEntry.first.controller);
		else
			setting->setAttribute("note", mapEntry.first.pitch);

		setting->setAttribute("command_string", mapEntry.second);

		root.addChildElement(setting);
	}

	if (!root.writeToFile(file, ""))
        // Give feedback if file-save doesn't work
        AlertWindow::showMessageBox(AlertWindow::WarningIcon,"File Save Error", "Unable to save file as specified. Please try again, and consider saving to a different location.");
}