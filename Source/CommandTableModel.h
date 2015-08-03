#ifndef COMMANDTABLEMODEL_H
#define COMMANDTABLEMODEL_H

#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "CommandMap.h"

class CommandTableModel : public TableListBoxModel,
						  public ComboBoxListener
{
public:
	CommandTableModel();

	// TableListBoxModel overrides
	virtual int getNumRows() override;
	virtual void paintRowBackground (Graphics &, int rowNumber, int width, int height, bool rowIsSelected) override;
	virtual void paintCell (Graphics &, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
	virtual Component *refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected, Component *existingComponentToUpdate) override;

	// ComboBoxListener interface
	void comboBoxChanged(ComboBox* cb) override;

	void addRow(int midi_channel, int midi_controller);
	int getRowForController(int midi_channel, int midi_controller);
private:
	int _rows;
	OwnedArray<ComboBox> _commandComboBoxes;
	std::vector<MIDI_CC> _commands;

	class CustomComboBox : public ComboBox
	{
	public:
		CustomComboBox(MIDI_CC& cc) : _cc(cc) {}
		MIDI_CC& getCC() { return _cc;  }
	private:
		MIDI_CC _cc;
	};

	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CommandTableModel)
};

#endif