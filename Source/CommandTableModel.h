/*
  ==============================================================================
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
#ifndef COMMANDTABLEMODEL_H
#define COMMANDTABLEMODEL_H

#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "CommandMap.h"
#include "CommandMenu.h"

class CommandTableModel : public TableListBoxModel
{
public:
	CommandTableModel();

	// TableListBoxModel overrides
	virtual int getNumRows() override;
	virtual void paintRowBackground(Graphics &, int rowNumber, int width, int height, bool rowIsSelected) override;
	virtual void paintCell(Graphics &, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
	virtual Component *refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component *existingComponentToUpdate) override;

	// adds a row with a corresponding MIDI message to the table
	void addRow(int midi_channel, int midi_data, bool isCC);

	// removes a row from the table
	void removeRow(int row);

	// removes all rows from the table
	void removeAllRows();

	// builds the table from an XML file
	void buildFromXml(XmlElement *elem);

	// returns the index of the row associated to a particular MIDI message
	int getRowForMessage(int midi_channel, int midi_data, bool isCC) const;
private:
	int _rows;

	std::vector<MIDI_Message> _commands;

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CommandTableModel)
};

#endif