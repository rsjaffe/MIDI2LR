/*
  ==============================================================================

    CommandMenu.cpp

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

#include <limits>
#include "CommandMenu.h"
#include "LRCommands.h"


/**********************************************************************************************//**
 * @fn  CommandMenu::CommandMenu(const MIDI_Message& msg): _msg(msg), _selectedItem(std::numeric_limits<unsigned int>::max()), TextButton("Unmapped"), m_commandMap(nullptr), m_menus(
 *
 * @brief   Constructor.
 *
 *
 *
 *
 * @param   msg The message.
 **************************************************************************************************/

CommandMenu::CommandMenu(const MIDI_Message& msg): _msg{ msg },
    _selectedItem{ std::numeric_limits<unsigned int>::max() },
    TextButton{ "Unmapped" }, m_commandMap{ nullptr },
    m_menus({ "Keyboard Shortcuts for User", "General", "Library", "Develop", "Basic", "Tone Curve",
        "HSL / Color / B&W", "Reset HSL / Color / B&W", "Split Toning", "Detail", "Lens Corrections",
        "Effects", "Camera Calibration", "Develop Presets", "Local Adjustments", "Crop",
        "Go to Tool, Module, or Panel", "Secondary Display", "Profiles", "Next/Prev Profile" }),

    m_menuEntries({ LRCommandList::KeyShortcuts, LRCommandList::General, LRCommandList::Library,
        LRCommandList::Develop, LRCommandList::BasicAdjustments, LRCommandList::ToneCurve, LRCommandList::Mixer,
        LRCommandList::ResetMixer, LRCommandList::SplitToning, LRCommandList::Detail,
        LRCommandList::LensCorrections, LRCommandList::Effects, LRCommandList::Calibration,
        LRCommandList::DevelopPresets, LRCommandList::LocalAdjustments, LRCommandList::Crop,
        LRCommandList::ToolModulePanel, LRCommandList::SecondaryDisplay, LRCommandList::ProgramProfiles,
        LRCommandList::NextPrevProfile })
{}

/**********************************************************************************************//**
 * @fn  void CommandMenu::setMsg(const MIDI_Message& msg)
 *
 * @brief   Sets a message.
 *
 *
 *
 *
 * @param   msg The message.
 **************************************************************************************************/

void CommandMenu::setMsg(const MIDI_Message& msg)
{
    _msg = msg;
}

/**********************************************************************************************//**
 * @fn  void CommandMenu::buttonClicked(Button* )
 *
 * @brief   Button clicked.
 *
 * @param [in,out]  button  button)  If non-null, the unused argument (button)
 **************************************************************************************************/

void CommandMenu::buttonClicked(Button* /*button*/)
{
    size_t idx = 1;
    auto subMenuTickSet = false;
    PopupMenu mainMenu;
    mainMenu.addItem(idx, "Unmapped", true, subMenuTickSet = (idx == _selectedItem));
    idx++;

    // add each submenu
    for (size_t menuIdx = 0; menuIdx < m_menus.size(); menuIdx++)
    {
        PopupMenu subMenu;
        for (auto cmd : m_menuEntries[menuIdx])
        {
            auto alreadyMapped = false;
            if ((idx - 1 < LRCommandList::LRStringList.size()) && (m_commandMap))
            {
                alreadyMapped = m_commandMap->commandHasAssociatedMessage(LRCommandList::LRStringList[idx - 1]);
            }

            // add each submenu entry, ticking the previously selected entry and disabling a previously mapped entry

            if (alreadyMapped)
                subMenu.addColouredItem(idx, cmd, Colours::red, true, idx == _selectedItem);
            else
                subMenu.addItem(idx, cmd, true, idx == _selectedItem);

            idx++;
        }
        // set whether or not the submenu is ticked (true if one of the submenu's entries is selected)
        mainMenu.addSubMenu(m_menus[menuIdx], subMenu, true, nullptr, _selectedItem < idx && !subMenuTickSet);
        subMenuTickSet |= (_selectedItem < idx && !subMenuTickSet);
    }

    auto result = static_cast<size_t>(mainMenu.show());
    if ((result) && (m_commandMap))
    {
        // user chose a different command, remove previous command mapping associated to this menu
        if (_selectedItem < std::numeric_limits<unsigned int>::max())
            m_commandMap->removeMessage(_msg);

        if (result - 1 < LRCommandList::LRStringList.size())
            setButtonText(LRCommandList::LRStringList[result - 1]);
        else
            setButtonText(LRCommandList::NextPrevProfile[result - 1 - LRCommandList::LRStringList.size()]);

        _selectedItem = result;

        // Map the selected command to the CC
        m_commandMap->addCommandforMessage(result - 1, _msg);
    }
}

/**********************************************************************************************//**
 * @fn  void CommandMenu::setSelectedItem(unsigned int idx)
 *
 * @brief   Sets selected item.
 *
 *
 *
 *
 * @param   idx The index.
 **************************************************************************************************/

void CommandMenu::setSelectedItem(unsigned int idx)
{
    _selectedItem = idx;
    if (idx - 1 < LRCommandList::LRStringList.size())
        setButtonText(LRCommandList::LRStringList[idx - 1]);
    else
        setButtonText(LRCommandList::NextPrevProfile[idx - 1 - LRCommandList::LRStringList.size()]);
}

/**********************************************************************************************//**
 * @fn  void CommandMenu::Init(std::shared_ptr<CommandMap>& mapCommand)
 *
 * @brief   Inits the given map command.
 *
 *
 *
 * @param [in,out]  mapCommand  If non-null, the map command.
 **************************************************************************************************/

void CommandMenu::Init(std::shared_ptr<CommandMap>& mapCommand)
{
    //copy the pointer
    m_commandMap = mapCommand;
    addListener(this);
}