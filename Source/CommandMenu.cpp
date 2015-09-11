/*
  ==============================================================================

    CommandMenu.cpp
    Created: 3 Aug 2015 3:27:33pm
    Author:  Parth

  ==============================================================================
*/

#include <limits>
#include "CommandMenu.h"
#include "LRCommands.h"

CommandMenu::CommandMenu(MIDI_Message& msg) : _msg(msg),
                                        _selectedItem(std::numeric_limits<int>::max()),
                                        TextButton("Unmapped")
{
    addListener(this);
}

void CommandMenu::setMsg(MIDI_Message& msg)
{
    _msg = msg;
}

void CommandMenu::buttonClicked(Button* button)
{
    auto idx = 1;
    bool subMenuTickSet = false;
    PopupMenu mainMenu;
    mainMenu.addItem(idx, "Unmapped", true, subMenuTickSet = (idx == _selectedItem));
    idx++;

    const std::vector<String> menus = { "Basic", 
                                        "Tone Curve", 
                                        "HSL / Color / B&W" ,
                                        "Split Toning",
                                        "Detail",
                                        "Lens Correction",
                                        "Effects",
                                        "Camera Calibration",
                                        "Photo Actions",
                                        "Tools",
                                        // MIDI2LR items
                                        "Profile"
                                      };
    const std::vector<std::vector<String>> menuEntries = { LRCommandList::AdjustmentStringList,
                                                           LRCommandList::ToneStringList,
                                                           LRCommandList::MixerStringList,
                                                           LRCommandList::SplitToningStringList,
                                                           LRCommandList::DetailStringList,
                                                           LRCommandList::LensCorrectionStringList,
                                                           LRCommandList::EffectsStringList,
                                                           LRCommandList::CalibrateStringList,
                                                           LRCommandList::SelectionList,
                                                           LRCommandList::ToolsList,
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
                alreadyMapped = CommandMap::getInstance().commandHasAssociatedMessage(LRCommandList::LRStringList[idx - 1]);

            // add each submenu entry, ticking the previously selected entry and disabling a previously mapped entry
            subMenu.addItem(idx, cmd, !alreadyMapped, idx == _selectedItem);
            idx++;
        }
        // set whether or not the submenu is ticked (true if one of the submenu's entries is selected)
        mainMenu.addSubMenu(menus[menuIdx], subMenu, true, nullptr, _selectedItem < idx && !subMenuTickSet);
        subMenuTickSet |= (_selectedItem < idx && !subMenuTickSet);
    }

    if (auto result = mainMenu.show())
    {
        // user chose a different command, remove previous command mapping associated to this menu
        if (_selectedItem < std::numeric_limits<int>::max())
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

void CommandMenu::setSelectedItem(int idx)
{
    _selectedItem = idx;
    if (idx - 1 < LRCommandList::LRStringList.size())
        setButtonText(LRCommandList::LRStringList[idx - 1]);
    else
        setButtonText(LRCommandList::ProfileList[idx - 1 - LRCommandList::LRStringList.size()]);
}