#ifndef MIDI2LR_COMMANDMENU_H_INCLUDED
#define MIDI2LR_COMMANDMENU_H_INCLUDED
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
#include <limits>

#include <JuceLibraryCode/JuceHeader.h>
#include "MidiUtilities.h"
class CommandSet;
class Profile;

class CommandMenu final : public juce::TextButton {
 public:
   CommandMenu(const rsj::MidiMessageId& message, const CommandSet& command_set, Profile& profile);
   // sets the MIDI message associated to this menu component
   void SetMsg(const rsj::MidiMessageId& message) noexcept;

   // sets which item in the menu is selected
   void SetSelectedItem(size_t index);

 private:
   void clicked(const juce::ModifierKeys& modifiers) override;

   const CommandSet& command_set_;
   Profile& profile_;
   rsj::MidiMessageId message_;
   size_t selected_item_{std::numeric_limits<size_t>::max()}; //-V122
};

#endif // MIDI2LR_COMMANDMENU_H_INCLUDED
