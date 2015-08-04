/*
  ==============================================================================

    CommandMenu.cpp
    Created: 3 Aug 2015 3:27:33pm
    Author:  Parth

  ==============================================================================
*/

#include <limits>
#include "CommandMenu.h"

CommandMenu::CommandMenu(MIDI_CC& cc) : _cc(cc),
                                        _selectedItem(std::numeric_limits<int>::max()),
                                        TextButton("Choose Command")
{
    addListener(this);
}

void CommandMenu::buttonClicked(Button* button)
{
    auto idx = 1;
    bool subMenuTickSet = false;
    PopupMenu mainMenu;
    mainMenu.addItem(idx, "Unmapped", true, subMenuTickSet = (idx == _selectedItem));
    idx++;

    PopupMenu adjustmentMenu;
    for (auto cmd : LRCommandList::AdjustmentStringList)
    {
        adjustmentMenu.addItem(idx, cmd, true, idx == _selectedItem);
        idx++;
    }
    mainMenu.addSubMenu("Basic", adjustmentMenu, true, nullptr, _selectedItem < idx && !subMenuTickSet);
    subMenuTickSet |= (_selectedItem < idx && !subMenuTickSet);

    PopupMenu toneMenu;
    for (auto cmd : LRCommandList::ToneStringList)
    {
        toneMenu.addItem(idx, cmd, true, idx == _selectedItem);
        idx++;
    }
    mainMenu.addSubMenu("Tone Curve", toneMenu, true, nullptr, _selectedItem < idx && !subMenuTickSet);
    subMenuTickSet |= (_selectedItem < idx && !subMenuTickSet);

    PopupMenu mixerMenu;
    for (auto cmd : LRCommandList::MixerStringList)
    {
        mixerMenu.addItem(idx, cmd, true, idx == _selectedItem);
        idx++;
    }

    mainMenu.addSubMenu("HSL / Color / B&W", mixerMenu, true, nullptr, _selectedItem < idx && !subMenuTickSet);
    subMenuTickSet |= (_selectedItem < idx && !subMenuTickSet);

    auto result = mainMenu.show();
    if (result)
    {
        _selectedItem = result;
        setButtonText(LRCommandList::LRStringList[result - 1]);
        CommandMap::getInstance().addCommandforCC(result - 1, _cc);
    }
}