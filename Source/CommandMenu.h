#pragma once
/*
  ==============================================================================

	CommandMenu.h

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
#ifndef COMMANDMENU_H_INCLUDED
#define COMMANDMENU_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "CommandMap.h"

/**********************************************************************************************//**
* @class   CommandMenu
*
* @brief   Command menu.
*
* @author  Rory Jaffe
* @date    3/20/2016
**************************************************************************************************/

class CommandMenu : public TextButton,
	protected ButtonListener
{
public:
	CommandMenu(const MIDI_Message& msg);

	// sets the MIDI message associated to this menu component
	void setMsg(const MIDI_Message& msg);

	// sets which item in the menu is selected
	void setSelectedItem(unsigned int idx);

	// ButtonListener interface
	virtual void buttonClicked(Button* button) override;
	void Init(CommandMap *mapCommand);
private:
	MIDI_Message _msg;
	size_t _selectedItem;
	CommandMap *m_commandMap;

	const std::vector<String> m_menus;
	const std::vector<std::vector<String>> m_menuEntries;
};




#endif  // COMMANDMENU_H_INCLUDED
