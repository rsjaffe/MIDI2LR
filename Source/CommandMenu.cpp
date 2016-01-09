/*
  ==============================================================================

    CommandMenu.cpp
    Created: 3 Aug 2015 3:27:33pm
    Author:  Parth, Jaffe

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

#include <limits>
#include "CommandMenu.h"
#include "LRCommands.h"
#include "tools.h"

CommandMenu::CommandMenu(const MIDI_Message& msg): _msg(msg),
_selectedItem(std::numeric_limits<unsigned int>::max()),
TextButton("Unmapped")
{
    addListener(this);
}

void CommandMenu::setMsg(const MIDI_Message& msg)
{
    _msg = msg;
}

void CommandMenu::buttonClicked(Button* UNUSED_ARG(button))
{
    size_t idx = 1;
    bool subMenuTickSet = false;
    PopupMenu mainMenu;
    mainMenu.addItem(idx, "Unmapped", true, subMenuTickSet = (idx == _selectedItem));
    idx++;

    const std::vector<String> menus = { "Basic",
                                        "Tone Curve",
                                        "HSL / Color / B&W" ,
                                        "Reset HSL / Color / B&W" ,
                                        "Split Toning",
                                        "Detail",
                                        "Lens Correction",
                                        "Effects",
                                        "Camera Calibration",
                                        "Photo Actions",
                                        "Develop Presets",
                                        "Tools",
                                        "Modules",
                                        "View Modes",
                                        // MIDI2LR items
                                        "Profile"
    };
    const std::vector<std::vector<String>> menuEntries = { LRCommandList::AdjustmentStringList,
                                                           LRCommandList::ToneStringList,
                                                           LRCommandList::MixerStringList,
                                                           LRCommandList::ResetMixerStringList,
                                                           LRCommandList::SplitToningStringList,
                                                           LRCommandList::DetailStringList,
                                                           LRCommandList::LensCorrectionStringList,
                                                           LRCommandList::EffectsStringList,
                                                           LRCommandList::CalibrateStringList,
                                                           LRCommandList::SelectionList,
                                                           LRCommandList::PresetsList,
                                                           LRCommandList::ToolsList,
                                                           LRCommandList::ModulesList,
                                                           LRCommandList::ViewModesList,
                                                            // MIDI2LR items
                                                            LRCommandList::ProfileList,
    };

    // add each submenu
    for (size_t menuIdx = 0; menuIdx < menus.size(); menuIdx++)
    {
        PopupMenu subMenu;
        for (auto cmd : menuEntries[menuIdx])
        {
            bool alreadyMapped = false;
			if (idx - 1 < LRCommandList::LRStringList.size())
			{
				alreadyMapped = CommandMap::getInstance().commandHasAssociatedMessage(LRCommandList::LRStringList[idx - 1]);
			}

            // add each submenu entry, ticking the previously selected entry and disabling a previously mapped entry

            if (alreadyMapped)
                subMenu.addColouredItem(idx, cmd, Colours::red, true, idx == _selectedItem);
            else
                subMenu.addItem(idx, cmd, true, idx == _selectedItem);

            idx++;
        }
        // set whether or not the submenu is ticked (true if one of the submenu's entries is selected)
        mainMenu.addSubMenu(menus[menuIdx], subMenu, true, nullptr, _selectedItem < idx && !subMenuTickSet);
        subMenuTickSet |= (_selectedItem < idx && !subMenuTickSet);
    }

    if (unsigned int result = mainMenu.show())
    {
        // user chose a different command, remove previous command mapping associated to this menu
        if (_selectedItem < std::numeric_limits<unsigned int>::max())
            CommandMap::getInstance().removeMessage(_msg);

        if (result - 1 < LRCommandList::LRStringList.size())
            setButtonText(LRCommandList::LRStringList[result - 1]);
        else
            setButtonText(LRCommandList::ProfileList[result - 1 - LRCommandList::LRStringList.size()]);

        _selectedItem = result;

        // Map the selected command to the CC
        CommandMap::getInstance().addCommandforMessage(result - 1, _msg);
    }
}

void CommandMenu::setSelectedItem(unsigned int idx)
{
    _selectedItem = idx;
    if (idx - 1 < LRCommandList::LRStringList.size())
        setButtonText(LRCommandList::LRStringList[idx - 1]);
    else
        setButtonText(LRCommandList::ProfileList[idx - 1 - LRCommandList::LRStringList.size()]);
}