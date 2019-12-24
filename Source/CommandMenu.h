#ifndef MIDI2LR_COMMANDMENU_H_INCLUDED
#define MIDI2LR_COMMANDMENU_H_INCLUDED
/*
 * This file is part of MIDI2LR. Copyright (C) 2015 by Rory Jaffe.
 *
 * MIDI2LR is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with MIDI2LR.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
//-V813_MINSIZE=13 /*warn if passing structure by value > 12 bytes (3*sizeof(int)) */

#include <limits>

#include <JuceLibraryCode/JuceHeader.h>
#include "CommandSet.h"
#include "MidiUtilities.h"
#include "TextButtonAligned.h"
class Profile;

class CommandMenu final : public TextButtonAligned {
 public:
   CommandMenu(rsj::MidiMessageId message, const CommandSet& command_set, Profile& profile);

   void SetMsg(rsj::MidiMessageId message) noexcept
   {
      message_ = message;
   }

   void SetSelectedItem(size_t index)
   {
      selected_item_ = index;
      if (index - 1 < command_set_.CommandAbbrevSize())
         setButtonText(command_set_.CommandLabelAt(index - 1));
   }

 private:
   void clicked(const juce::ModifierKeys& modifiers) override;

   const CommandSet& command_set_;
   Profile& profile_;
   rsj::MidiMessageId message_;
   size_t selected_item_ {std::numeric_limits<size_t>::max()};
};

#endif
