#ifndef MIDI2LR_COMMANDTABLEMODEL_H
#define MIDI2LR_COMMANDTABLEMODEL_H
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

#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "CommandMenu.h"
#include "CommandSet.h"
#include "Profile.h"

class CommandTableModel final : public juce::TableListBoxModel {
 public:
   CommandTableModel(const CommandSet& command_set, Profile& profile) noexcept;

   // ReSharper disable once CppMemberFunctionMayBeConst
   void RemoveRow(size_t row)
   {
      /* called from CommandTable, forward to Profile */
      profile_.RemoveRow(row);
   }

 private:
   [[nodiscard]] int getNumRows() override { return gsl::narrow_cast<int>(profile_.Size()); }

   CommandMenu* CreateNewCommandMenu(int row_number, juce::Component* existing_component) const;
   CommandMenu* UpdateCommandMenu(int row_number, gsl::not_null<CommandMenu *> command_select) const;
   juce::Component* refreshComponentForCell(int row_number, int column_id, bool is_row_selected,
       juce::Component* existing_component) override;
   void paintCell(juce::Graphics&, int row_number, int column_id, int width, int height,
       bool row_is_selected) override;
   void paintRowBackground(juce::Graphics&, int row_number, int width, int height,
       bool row_is_selected) override;
   void sortOrderChanged(int new_sort_column_id, bool is_forwards) override;

   const CommandSet& command_set_;
   Profile& profile_;
};
#endif
