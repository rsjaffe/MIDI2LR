/*
  ==============================================================================

    CommandMenu.cpp
    Created: 3 Aug 2015 3:27:33pm
    Author:  Parth

  ==============================================================================
*/

#include "CommandMenu.h"

CommandMenu::CommandMenu(MIDI_CC& cc) : _cc(cc),
                                        TextButton("Choose Command")
{
    addListener(this);
}

void CommandMenu::buttonClicked(Button* button)
{
    auto idx = 1;
    PopupMenu mainMenu;
    mainMenu.addItem(idx++, "Unmapped");

    PopupMenu adjustmentMenu;
    for (auto cmd : LRCommandList::AdjustmentStringList)
        adjustmentMenu.addItem(idx++, cmd);
    mainMenu.addSubMenu("Adjustment", adjustmentMenu);

    PopupMenu toneMenu;
    for (auto cmd : LRCommandList::ToneStringList)
        toneMenu.addItem(idx++, cmd);
    mainMenu.addSubMenu("Tone", toneMenu);

    auto result = mainMenu.show();
    if (result)
    {
        setButtonText(LRCommandList::LRStringList[result - 1]);
        CommandMap::getInstance().addCommandforCC(result - 1, _cc);
    }
}