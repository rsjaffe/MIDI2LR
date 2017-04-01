#pragma once
/*
  ==============================================================================

    CommandMenu.h

This file is part of MIDI2LR. Copyright 2015-2017 by Rory Jaffe.

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
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "MidiUtilities.h"
class CommandMap;

class CommandMenu final: public juce::TextButton {
public:
    explicit CommandMenu(const RSJ::MidiMessageId& msg);
    void Init(CommandMap* const map_command) noexcept;
    // sets the MIDI message associated to this menu component
    void setMsg(const RSJ::MidiMessageId& msg) noexcept;

    // sets which item in the menu is selected
    void setSelectedItem(size_t idx);

private:
    virtual void clicked(const juce::ModifierKeys& modifiers) override;

    CommandMap* command_map_{nullptr};
    const std::vector<juce::String> menus_;
    const std::vector<std::vector<std::string>> menu_entries_;
    RSJ::MidiMessageId message_;
    size_t selected_item_{std::numeric_limits<size_t>::max()};
};

#endif  // COMMANDMENU_H_INCLUDED
