#include "CommandTableModel.h"
#include "LRCommands.h"

CommandTableModel::CommandTableModel() : _rows(0)
{
}

int CommandTableModel::getNumRows()
{
    return _rows;
}

void CommandTableModel::paintRowBackground (Graphics &g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll (Colours::lightblue);
}

void CommandTableModel::paintCell (Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    g.setColour(Colours::black);
    g.setFont(12.0f);

    if (columnId == 1) // MIDI command column
        g.drawText(String::formatted("%d | CC: %d", _commands[rowNumber].channel,
                   _commands[rowNumber].controller), 0, 0, width, height, Justification::centred);
}

Component *CommandTableModel::refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected, Component *existingComponentToUpdate)
{
    if (columnId == 2) // LR command column
    {
        CommandMenu* commandSelect = (CommandMenu *)existingComponentToUpdate;

        // create a new command menu
        if (commandSelect == nullptr)
            commandSelect = new CommandMenu(_commands[rowNumber]);
        commandSelect->setSelectedItem(CommandMap::getInstance().getCommandforCC(_commands[rowNumber]) + 1); // add one because
                                                                                                                 // zero is reserved
                                                                                                                 // for no selection

        return commandSelect;
    }
    else
        return nullptr;
}

void CommandTableModel::addRow(int midi_channel, int midi_controller)
{
    MIDI_CC cc(midi_channel, midi_controller);

    if (!CommandMap::getInstance().controllerExistsInMap(cc))
    {
        _commands.push_back(cc);
        CommandMap::getInstance().addCommandforCC(0, cc); // add an entry for 'no command'
        _rows++;
    }
}

int CommandTableModel::getRowForController(int midi_channel, int midi_controller)
{
    for (auto idx = 0; idx < _rows; idx++)
    {
        if (_commands[idx].channel == midi_channel && _commands[idx].controller == midi_controller)
            return idx;
    }
}

void CommandTableModel::removeRow(int row)
{
    MIDI_CC cc = _commands[row];
    _commands.erase(_commands.begin() + row);
    CommandMap::getInstance().removeCC(cc);
    _rows--;
}

void CommandTableModel::removeAllRows()
{
    _commands.clear();
    CommandMap::getInstance().clearMap();
    _rows = 0;
}

void CommandTableModel::buildFromXml(XmlElement *root)
{
    if (root->getTagName().compare("settings") != 0)
        return;
    
    removeAllRows();

    XmlElement* setting = root->getFirstChildElement();
    while (setting)
    {
        MIDI_CC cc(setting->getIntAttribute("channel"), setting->getIntAttribute("controller"));
        addRow(cc.channel, cc.controller);
        CommandMap::getInstance().addCommandforCC(setting->getIntAttribute("command"), cc);
        setting = setting->getNextElement();
    }
}