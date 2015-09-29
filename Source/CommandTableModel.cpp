#include "CommandTableModel.h"
#include "LRCommands.h"

CommandTableModel::CommandTableModel() : _rows(0)
{
}

int CommandTableModel::getNumRows()
{
	return _rows;
}

void CommandTableModel::paintRowBackground(Graphics &g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
	if (rowIsSelected)
		g.fillAll(Colours::lightblue);
}

void CommandTableModel::paintCell(Graphics &g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	g.setColour(Colours::black);
	g.setFont(12.0f);

	if (columnId == 1) // write the MIDI message in the MIDI command column
	{
		if (_commands[rowNumber].isCC)
			g.drawText(String::formatted("%d | CC: %d", _commands[rowNumber].channel,
				_commands[rowNumber].controller), 0, 0, width, height, Justification::centred);
		else
			g.drawText(String::formatted("%d | Note: %d", _commands[rowNumber].channel,
				_commands[rowNumber].pitch), 0, 0, width, height, Justification::centred);
	}
}

Component *CommandTableModel::refreshComponentForCell(int rowNumber, int columnId, bool /*isRowSelected*/, Component *existingComponentToUpdate)
{
	if (columnId == 2) // LR command column
	{
		// because Juce recycles these components when scrolling, we need to reset their properties
		CommandMenu* commandSelect = (CommandMenu *)existingComponentToUpdate;

		// create a new command menu
		if (commandSelect == nullptr)
			commandSelect = new CommandMenu(_commands[rowNumber]);
		else
			commandSelect->setMsg(_commands[rowNumber]);
		// add 1 because 0 is reserved for no selection
		commandSelect->setSelectedItem(LRCommandList::getIndexOfCommand(CommandMap::getInstance().getCommandforMessage(_commands[rowNumber])) + 1);

		return commandSelect;
	}
	else
		return nullptr;
}

void CommandTableModel::addRow(int midi_channel, int midi_data, bool isCC)
{
	MIDI_Message msg(midi_channel, midi_data, isCC);

	if (!CommandMap::getInstance().messageExistsInMap(msg))
	{
		_commands.push_back(msg);
		CommandMap::getInstance().addCommandforMessage(0, msg); // add an entry for 'no command'
		_rows++;
	}
}

int CommandTableModel::getRowForMessage(int midi_channel, int midi_data, bool isCC) const
{
	for (auto idx = 0; idx < _rows; idx++)
	{
		if (_commands[idx].channel == midi_channel && _commands[idx].controller == midi_data
			&& _commands[idx].isCC == isCC)
			return idx;
	}
}

void CommandTableModel::removeRow(int row)
{
	MIDI_Message msg = _commands[row];
	_commands.erase(_commands.begin() + row);
	CommandMap::getInstance().removeMessage(msg);
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
		if (setting->hasAttribute("controller"))
		{
			MIDI_Message cc(setting->getIntAttribute("channel"), setting->getIntAttribute("controller"), true);
			addRow(cc.channel, cc.controller, true);

			// older versions of MIDI2LR stored the index of the string, so we should attempt to parse this as well
			if (setting->getIntAttribute("command", -1) != -1)
				CommandMap::getInstance().addCommandforMessage(setting->getIntAttribute("command"), cc);
			else
				CommandMap::getInstance().addCommandforMessage(setting->getStringAttribute("command_string"), cc);

		}
		else if (setting->hasAttribute("note"))
		{
			MIDI_Message note(setting->getIntAttribute("channel"), setting->getIntAttribute("note"), false);
			addRow(note.channel, note.pitch, false);

			// older versions of MIDI2LR stored the index of the string, so we should attempt to parse this as well
			if (setting->getIntAttribute("command", -1) != -1)
				CommandMap::getInstance().addCommandforMessage(setting->getIntAttribute("command"), note);
			else
				CommandMap::getInstance().addCommandforMessage(setting->getStringAttribute("command_string"), note);
		}
		setting = setting->getNextElement();
	}
}