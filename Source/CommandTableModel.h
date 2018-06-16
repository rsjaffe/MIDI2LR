#pragma once
/*
  ==============================================================================

  CommandTableModel.h

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
#ifndef MIDI2LR_COMMANDTABLEMODEL_H
#define MIDI2LR_COMMANDTABLEMODEL_H

#include <mutex>
#include <shared_mutex>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
#include "MidiUtilities.h"

class CommandMap;

class CommandTableModel final : public juce::TableListBoxModel {
 public:
   void Init(CommandMap* map_command) noexcept;

   // adds a row with a corresponding MIDI message to the table
   void AddRow(int midi_channel, int midi_data, rsj::MsgIdEnum msg_type);
   // removes a row from the table
   void RemoveRow(size_t row);
   // removes all rows from the table
   void RemoveAllRows() noexcept;
   // builds the table from an XML file
   void BuildFromXml(const juce::XmlElement* root);
   // returns the index of the row associated to a particular MIDI message
   [[nodiscard]] int GetRowForMessage(
       int midi_channel, int midi_data, rsj::MsgIdEnum msg_type) const;

 private:
   // TableListBoxModel overrides
   void sortOrderChanged(int new_sort_column_id, bool is_forwards) override;
   [[nodiscard]] int getNumRows() noexcept override;
   void paintRowBackground(
       juce::Graphics&, int row_number, int width, int height, bool row_is_selected) override;
   void paintCell(juce::Graphics&, int row_number, int column_id, int width, int height,
       bool row_is_selected) override;
   juce::Component* refreshComponentForCell(int row_number, int column_id, bool is_row_selected,
       juce::Component* existing_component_to_update) override;
   void Sort();
   CommandMap* command_map_{nullptr};
   mutable std::shared_mutex cmdmap_mutex_;
   std::pair<int, bool> current_sort_{2, true};
   std::pair<int, bool> prior_sort_{2, true};
   std::vector<rsj::MidiMessageId> commands_;
};

#endif