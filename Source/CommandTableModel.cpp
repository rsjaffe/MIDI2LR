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
			                                        _commands[rowNumber].controller),
		0, 0, width, height, Justification::centred);
}

Component *CommandTableModel::refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected, Component *existingComponentToUpdate)
{
	if (columnId == 2) // LR command column
	{
		CustomComboBox* commandSelect = (CustomComboBox *)existingComponentToUpdate;

		// if we need to create a new combo box, do so and fill it with LR commands
		if (commandSelect == nullptr)
		{
			commandSelect = new CustomComboBox(_commands[rowNumber]);
			commandSelect->addListener(this);
			auto idx = 1;
			for (auto cmd : LRCommandList::StringList)
				commandSelect->addItem(cmd, idx++);
		}
		return commandSelect;
	}
	else
		return nullptr;
}

void CommandTableModel::comboBoxChanged(ComboBox* cb)
{
	CommandMap::getInstance().addCommandforCC(cb->getSelectedId()-1, ((CustomComboBox*)cb)->getCC());
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
		if (_commands[idx].channel == midi_channel
			&& _commands[idx].controller == midi_controller)
			return idx;
	}
}