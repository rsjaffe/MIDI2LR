#pragma once
/*
  ==============================================================================

    CommandMenu.h

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
#ifndef COMMANDMENU_H_INCLUDED
#define COMMANDMENU_H_INCLUDED

#include <limits>
#include <memory>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "CommandMap.h"

class CommandMenu final: public juce::TextButton,
  private juce::ButtonListener {
public:
  explicit CommandMenu(const MIDI_Message_ID& msg);
  void Init(std::shared_ptr<CommandMap>& map_command);
  // sets the MIDI message associated to this menu component
  void setMsg(const MIDI_Message_ID& msg) noexcept;

  // sets which item in the menu is selected
  void setSelectedItem(unsigned int idx);

private:
  // ButtonListener interface
  virtual void buttonClicked(juce::Button* button) override;


  MIDI_Message_ID message_;
  const std::vector<juce::String> menus_;
  const std::vector<std::vector<std::string>> menu_entries_;
  size_t selected_item_{std::numeric_limits<size_t>::max()};
  std::shared_ptr<CommandMap> command_map_{nullptr};
};

#endif  // COMMANDMENU_H_INCLUDED
