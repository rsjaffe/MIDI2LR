/*
  ==============================================================================

    CommandMap.cpp
    Created: 31 Jul 2015 9:16:41pm
    Author:  Parth

  ==============================================================================
*/

#include "CommandMap.h"
#include "LRCommands.h"

CommandMap::CommandMap()
{

}

CommandMap& CommandMap::getInstance()
{
    static CommandMap instance;
    return instance;
}

void CommandMap::addCommandforMessage(int command, MIDI_Message &msg)
{
    _messageMap[msg] = command;
    _commandStringMap[LRCommandList::LRStringList[command]] = msg;
}

int CommandMap::getCommandforMessage(MIDI_Message &msg)
{
    return _messageMap[msg];
}

MIDI_Message& CommandMap::getMessageForCommand(String &command)
{
    return _commandStringMap[command];
}

bool CommandMap::messageExistsInMap(MIDI_Message &msg)
{
    return _messageMap.count(msg);
}

bool CommandMap::commandHasAssociatedMessage(String &command)
{
    return _commandStringMap.count(command);
}

void CommandMap::removeMessage(MIDI_Message &msg)
{
    _commandStringMap.erase(LRCommandList::LRStringList[_messageMap[msg]]);
    _messageMap.erase(msg);
}

void CommandMap::clearMap()
{
    _commandStringMap.clear();
    _messageMap.clear();
}

void CommandMap::toXMLDocument(File& file)
{
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