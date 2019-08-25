/*
  ==============================================================================

    CommandMenu.cpp

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
#include "CommandMenu.h"

#include <exception>

#include <gsl/gsl>
#include "CCoptions.h"
#include "CommandSet.h"
#include "Misc.h"
#include "Profile.h"
#include "PWoptions.h"

CommandMenu::CommandMenu(
    rsj::MidiMessageId message, const CommandSet& command_set, Profile& profile)
try : juce
   ::TextButton{"Unmapped"}, command_set_(command_set), profile_(profile), message_{message} {}
catch (const std::exception& e) {
   rsj::ExceptionResponse(typeid(this).name(), __func__, e);
   throw;
}

void CommandMenu::SetMsg(rsj::MidiMessageId message) noexcept
{
   message_ = message;
}

void CommandMenu::SetSelectedItem(size_t index)
{
   try {
      selected_item_ = index;
      if (index - 1 < command_set_.CommandAbbrevSize())
         setButtonText(command_set_.CommandAbbrevAt(index - 1));
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void CommandMenu::clicked(const juce::ModifierKeys& modifiers)
{
   try {
      if (modifiers.isPopupMenu()) {
         switch (message_.msg_id_type) {
         case rsj::MessageType::Cc: {
            CCoptions ccopt;
            // convert 1-based to  0-based
            ccopt.BindToControl(message_.channel - 1, message_.control_number);
            juce::DialogWindow::showModalDialog(juce::translate("Adjust CC dialog"), &ccopt,
                nullptr, juce::Colour::fromRGB(0xFF, 0xFF, 0xFF), true);
            break;
         }
         case rsj::MessageType::Pw: {
            PWoptions pwopt;
            pwopt.BindToControl(message_.channel - 1); // convert 1-based
                                                       // to 0 based
            juce::DialogWindow::showModalDialog(juce::translate("Adjust PW dialog"), &pwopt,
                nullptr, juce::Colour::fromRGB(0xFF, 0xFF, 0xFF), true);
            break;
         }
         default:
             /* do nothing for other types of controllers */;
         }
      }
      else {
         size_t index = 1;
         auto submenu_tick_set = false;
         juce::PopupMenu main_menu;
         main_menu.addItem(gsl::narrow_cast<int>(index), "Unmapped", true,
             submenu_tick_set = index == selected_item_);
         index++;
         // add each submenu
         for (size_t menu_index = 0; menu_index < command_set_.GetMenus().size(); ++menu_index) {
            juce::PopupMenu sub_menu;
            for (const auto& command : command_set_.GetMenuEntries().at(menu_index)) {
               auto already_mapped = false;
               if (index - 1 < command_set_.CommandAbbrevSize())
                  already_mapped =
                      profile_.CommandHasAssociatedMessage(command_set_.CommandAbbrevAt(index - 1));

               // add each submenu entry, ticking the previously selected entry and
               // disabling a previously mapped entry
               if (already_mapped)
                  sub_menu.addColouredItem(gsl::narrow_cast<int>(index), command,
                      juce::Colours::red, true, index == selected_item_);
               else
                  sub_menu.addItem(
                      gsl::narrow_cast<int>(index), command, true, index == selected_item_);

               index++;
            }
            // set whether or not the submenu is ticked (true if one of the submenu's
            // entries is selected)
            main_menu.addSubMenu(command_set_.GetMenus().at(menu_index), sub_menu, true, nullptr,
                selected_item_ < index && !submenu_tick_set);
            submenu_tick_set |= selected_item_ < index && !submenu_tick_set;
         }
         const auto result = gsl::narrow_cast<size_t>(main_menu.show());
         if (result) {
            // user chose a different command, remove previous command mapping
            // associated to this menu
            if (selected_item_ < std::numeric_limits<size_t>::max())
               profile_.RemoveMessage(message_);
            if (result - 1 < command_set_.CommandAbbrevSize())
               setButtonText(command_set_.CommandAbbrevAt(result - 1));
            selected_item_ = result;
            // Map the selected command to the CC
            profile_.AddCommandForMessage(result - 1, message_);
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}