/*
  ==============================================================================

    CommandMap.cpp
    Created: 31 Jul 2015 9:16:41pm
    Author:  Parth

  ==============================================================================
*/

#include "CommandMap.h"
#include "LRCommands.h"

const int CommandMap::UNMAPPED_INDEX = 0;

CommandMap::CommandMap()
{

}

CommandMap& CommandMap::getInstance()
{
    static CommandMap instance;
    return instance;
}

void CommandMap::addCommandforMessage(int command, const MIDI_Message &msg)
{
    // adds a msg to the msg:command map, and it's associated command to the command:msg map
    _messageMap[msg] = command;

    if(command < LRCommandList::LRStringList.size())
      _commandStringMap[LRCommandList::LRStringList[command]] = msg;
}

int CommandMap::getCommandforMessage(const MIDI_Message &msg) const
{
    return _messageMap.at(msg);
}

const MIDI_Message& CommandMap::getMessageForCommand(const String &command) const
{
    return _commandStringMap.at(command);
}

bool CommandMap::messageExistsInMap(const MIDI_Message &msg) const
{
    return _messageMap.count(msg);
}

bool CommandMap::commandHasAssociatedMessage(const String &command) const
{
    return _commandStringMap.count(command);
}

void CommandMap::removeMessage(const MIDI_Message &msg)
{
    // removes msg from the msg:command map, and it's associated command from the command:msg map
    if (_messageMap[msg] < LRCommandList::LRStringList.size())
        _commandStringMap.erase(LRCommandList::LRStringList[_messageMap[msg]]);

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

        if (mapEntry.first.isCC)
            setting->setAttribute("controller", mapEntry.first.controller);
        else
            setting->setAttribute("note", mapEntry.first.pitch);

        setting->setAttribute("command", mapEntry.second);

        root.addChildElement(setting);
    }

    root.writeToFile(file, "");
}