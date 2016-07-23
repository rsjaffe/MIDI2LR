/*
  ==============================================================================

    CommandMenu.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/

#include "CommandMenu.h"
#include <limits>
#include "LRCommands.h"

CommandMenu::CommandMenu(const MIDI_Message_ID& message):
  juce::TextButton{"Unmapped"},
  message_{message},

  menus_({"Keyboard Shortcuts for User", "General", "Library", "Develop",
  "Basic", "Tone Curve", "HSL / Color / B&W", "Reset HSL / Color / B&W",
  "Split Toning", "Detail", "Lens Corrections", "Effects", "Camera Calibration",
  "Develop Presets", "Local Adjustments", "Crop", "Go to Tool, Module, or Panel",
  "Secondary Display", "Profiles", "Next/Prev Profile"}),

  menu_entries_({LRCommandList::KeyShortcuts, LRCommandList::General,
  LRCommandList::Library, LRCommandList::Develop,
  LRCommandList::BasicAdjustments, LRCommandList::ToneCurve,
  LRCommandList::Mixer, LRCommandList::ResetMixer, LRCommandList::SplitToning,
  LRCommandList::Detail, LRCommandList::LensCorrections, LRCommandList::Effects,
  LRCommandList::Calibration, LRCommandList::DevelopPresets,
  LRCommandList::LocalAdjustments, LRCommandList::Crop,
  LRCommandList::ToolModulePanel, LRCommandList::SecondaryDisplay,
  LRCommandList::ProgramProfiles, LRCommandList::NextPrevProfile})
{}

void CommandMenu::Init(std::shared_ptr<CommandMap>& mapCommand) {
    //copy the pointer
  command_map_ = mapCommand;
  juce::Button::addListener(this);
}

void CommandMenu::setMsg(const MIDI_Message_ID& message) noexcept {
  message_ = message;
}

void CommandMenu::setSelectedItem(unsigned int index) {
  selected_item_ = index;
  if (index - 1 < LRCommandList::LRStringList.size())
    setButtonText(LRCommandList::LRStringList[index - 1]);
  else
    setButtonText(LRCommandList::NextPrevProfile[index - 1 - LRCommandList::LRStringList.size()]);
}

void CommandMenu::buttonClicked(juce::Button* /*button*/) {
  size_t index = 1;
  auto submenu_tick_set = false;
  juce::PopupMenu main_menu;
  main_menu.addItem(index, "Unmapped", true, submenu_tick_set = (index == selected_item_));
  index++;

  // add each submenu
  for (size_t menu_index = 0; menu_index < menus_.size(); menu_index++) {
    juce::PopupMenu subMenu;
    for (const auto& command : menu_entries_[menu_index]) {
      auto already_mapped = false;
      if ((index - 1 < LRCommandList::LRStringList.size()) && (command_map_)) {
        already_mapped =
          command_map_->commandHasAssociatedMessage(LRCommandList::LRStringList[index - 1]);
      }

      // add each submenu entry, ticking the previously selected entry and
      // disabling a previously mapped entry

      if (already_mapped)
        subMenu.addColouredItem(index, command, juce::Colours::red, true,
          index == selected_item_);
      else
        subMenu.addItem(index, command, true, index == selected_item_);

      index++;
    }
    // set whether or not the submenu is ticked (true if one of the submenu's
    // entries is selected)
    main_menu.addSubMenu(menus_[menu_index], subMenu, true, nullptr,
      selected_item_ < index && !submenu_tick_set);
    submenu_tick_set |= (selected_item_ < index && !submenu_tick_set);
  }

  const auto result = static_cast<size_t>(main_menu.show());
  if ((result) && (command_map_)) {
      // user chose a different command, remove previous command mapping
      // associated to this menu
    if (selected_item_ < std::numeric_limits<unsigned int>::max())
      command_map_->removeMessage(message_);

    if (result - 1 < LRCommandList::LRStringList.size())
      setButtonText(LRCommandList::LRStringList[result - 1]);
    else
      setButtonText(LRCommandList::NextPrevProfile[result - 1 - LRCommandList::LRStringList.size()]);

    selected_item_ = result;

    // Map the selected command to the CC
    command_map_->addCommandforMessage(result - 1, message_);
  }
}