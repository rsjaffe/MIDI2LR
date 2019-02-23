#pragma once
/*
  ==============================================================================

    CommandMenu.h

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

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
#ifndef MIDI2LR_COMMANDMENU_H_INCLUDED
#define MIDI2LR_COMMANDMENU_H_INCLUDED

#include <limits>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "MidiUtilities.h"
class CommandMap;
class CommandSet;

class CommandMenu final : public juce::TextButton {
 public:
   explicit CommandMenu(
       const rsj::MidiMessageId& message, const CommandSet& command_set, CommandMap& map_command);
   // sets the MIDI message associated to this menu component
   void SetMsg(const rsj::MidiMessageId& message) noexcept;

   // sets which item in the menu is selected
   void SetSelectedItem(size_t index);

 private:
   void clicked(const juce::ModifierKeys& modifiers) override;

   CommandMap& command_map_;
   const CommandSet& command_set_;
   rsj::MidiMessageId message_;
   size_t selected_item_{std::numeric_limits<size_t>::max()};
};

#endif // COMMANDMENU_H_INCLUDED
