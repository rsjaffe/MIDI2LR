/*
  ==============================================================================

	CommandMenu.h
	Created: 3 Aug 2015 3:27:33pm
	Author:  Parth

  ==============================================================================
*/

#ifndef COMMANDMENU_H_INCLUDED
#define COMMANDMENU_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "CommandMap.h"

class CommandMenu : public TextButton,
	protected ButtonListener
{
public:
	CommandMenu(const MIDI_Message& msg);

	// sets the MIDI message associated to this menu component
	void setMsg(const MIDI_Message& msg);

	// sets which item in the menu is selected
	void setSelectedItem(int idx);

	// ButtonListener interface
	virtual void buttonClicked(Button* button) override;

private:
	MIDI_Message _msg;
	int _selectedItem;
};




#endif  // COMMANDMENU_H_INCLUDED
