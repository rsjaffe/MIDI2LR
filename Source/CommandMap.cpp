/*
  ==============================================================================

    CommandMap.cpp
    Created: 31 Jul 2015 9:16:41pm
    Author:  Parth

  ==============================================================================
*/

#include "CommandMap.h"

CommandMap::CommandMap()
{

}

CommandMap& CommandMap::getInstance()
{
    static CommandMap instance;
    return instance;
}

void CommandMap::addCommandforCC(int command, MIDI_CC &cc)
{
    _controllerMap[cc] = command;
}

int CommandMap::getCommandforCC(MIDI_CC &cc)
{
    return _controllerMap[cc];
}

bool CommandMap::controllerExistsInMap(MIDI_CC &cc)
{
    return _controllerMap.count(cc);
}

void CommandMap::removeCC(MIDI_CC &cc)
{
    _controllerMap.erase(cc);
}

void CommandMap::clearMap()
{
    _controllerMap.clear();
}

void CommandMap::toXMLDocument(File& file)
{
    XmlElement root("settings");
    for (auto mapEntry : _controllerMap)
    {
        XmlElement* setting = new XmlElement("setting");
        setting->setAttribute("channel", mapEntry.first.channel);
        setting->setAttribute("controller", mapEntry.first.controller);
        setting->setAttribute("command", mapEntry.second);

        root.addChildElement(setting);
    }

    root.writeToFile(file, "");
}