/*
  ==============================================================================
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
#include "CommandTableModel.h"
#include "LRCommands.h"

CommandTableModel::CommandTableModel() : _rows(0), m_commandMap(nullptr)
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
		CommandMenu* commandSelect =  dynamic_cast<CommandMenu *>(existingComponentToUpdate);

		// create a new command menu
		if (commandSelect == nullptr)
		{
			commandSelect = new CommandMenu(_commands[rowNumber]);
			commandSelect->Init(m_commandMap);
		}
		else
			commandSelect->setMsg(_commands[rowNumber]);

		if (m_commandMap)
		{
			// is this a key command?
			String strCommandToSend = m_commandMap->getCommandforMessage(_commands[rowNumber]);
			if (strCommandToSend.startsWith("keys:"))
				commandSelect->setButtonText(strCommandToSend);
			else
				// add 1 because 0 is reserved for no selection
				commandSelect->setSelectedItem(LRCommandList::getIndexOfCommand(m_commandMap->getCommandforMessage(_commands[rowNumber])) + 1);
		}
		
		return commandSelect;
	}
	else
		return nullptr;
}

void CommandTableModel::addRow(int midi_channel, int midi_data, bool isCC)
{
	MIDI_Message msg(midi_channel, midi_data, isCC);
	if (m_commandMap)
	{
		if (!m_commandMap->messageExistsInMap(msg))
		{
			_commands.push_back(msg);
			m_commandMap->addCommandforMessage(0, msg); // add an entry for 'no command'
			_rows++;
		}
	}
}

// return value -1 means can not find
int CommandTableModel::getRowForMessage(int midi_channel, int midi_data, bool isCC) const
{
	for (auto idx = 0; idx < _rows; idx++)
	{
		if (_commands[idx].channel == midi_channel && _commands[idx].controller == midi_data
			&& _commands[idx].isCC == isCC)
			return idx;
	}
	//could not find 
	return -1;
}

void CommandTableModel::removeRow(int row)
{
	MIDI_Message msg = _commands[row];
	_commands.erase(_commands.begin() + row);
	if (m_commandMap)
	{
		m_commandMap->removeMessage(msg);
	}
	_rows--;
}

void CommandTableModel::removeAllRows()
{
	_commands.clear();

	if (m_commandMap)
	{
		m_commandMap->clearMap();
	}
	
	_rows = 0;
}

void CommandTableModel::buildFromXml(XmlElement *root)
{
	if (root->getTagName().compare("settings") != 0)
		return;

	removeAllRows();

	XmlElement* setting = root->getFirstChildElement();
	while ((setting) && (m_commandMap))
	{
		if (setting->hasAttribute("key_command") /*&& (setting->getIntAttribute("keyboard_shortcut") == 1)*/)
		{
			MIDI_Message cc(setting->getIntAttribute("channel"), setting->getIntAttribute("controller"), true);
			addRow(cc.channel, cc.controller, true);

			m_commandMap->addCommandforMessage(setting->getStringAttribute("key_command"), cc);
		}
		else if (setting->hasAttribute("controller"))
		{
			MIDI_Message cc(setting->getIntAttribute("channel"), setting->getIntAttribute("controller"), true);
			addRow(cc.channel, cc.controller, true);

			// older versions of MIDI2LR stored the index of the string, so we should attempt to parse this as well
			if (setting->getIntAttribute("command", -1) != -1)
			{
				m_commandMap->addCommandforMessage(setting->getIntAttribute("command"), cc);
			}
			else
			{
				m_commandMap->addCommandforMessage(setting->getStringAttribute("command_string"), cc);
			}
		}
		else if (setting->hasAttribute("note"))
		{
			MIDI_Message note(setting->getIntAttribute("channel"), setting->getIntAttribute("note"), false);
			addRow(note.channel, note.pitch, false);

			// older versions of MIDI2LR stored the index of the string, so we should attempt to parse this as well
			if (setting->getIntAttribute("command", -1) != -1)
			{
				m_commandMap->addCommandforMessage(setting->getIntAttribute("command"), note);
			}
			else
			{
				m_commandMap->addCommandforMessage(setting->getStringAttribute("command_string"), note);
			}
		}
		setting = setting->getNextElement();
	}
}

void CommandTableModel::Init(CommandMap *mapCommand)
{
	//copy the pointer
	m_commandMap = mapCommand;
}