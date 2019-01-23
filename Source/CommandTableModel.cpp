/*
  ==============================================================================

  CommandTableModel.cpp

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
#include <algorithm>
#include <gsl/gsl>
#include "CommandTableModel.h"
#include "CommandMap.h"
#include "CommandMenu.h"
#include "LRCommands.h"
#include "Misc.h"

void CommandTableModel::Init(CommandMap* map_command) noexcept
{
   // copy the pointer
   command_map_ = map_command;
}

/**
 */
void CommandTableModel::sortOrderChanged(int new_sort_column_id, bool is_forwards)
{
   // This callback is made when the table's sort order is changed.

   // This could be because the user has clicked a column header, or because the
   // TableHeaderComponent::setSortColumnId() method was called.

   // If you implement this, your method should re - sort the table using the
   // given column as the key.
   std::unique_lock<decltype(cmd_table_mod_mtx_)> grd{cmd_table_mod_mtx_};
   current_sort_ = std::make_pair(new_sort_column_id, is_forwards);
   Sort();
}

int CommandTableModel::getNumRows()
{
   // This must return the number of rows currently in the table.

   // If the number of rows changes, you must call TableListBox::updateContent()
   // to cause it to refresh the list.
   std::shared_lock<decltype(cmd_table_mod_mtx_)> lck{cmd_table_mod_mtx_};
   return gsl::narrow_cast<int>(commands_.size());
}

void CommandTableModel::paintRowBackground(juce::Graphics& g,
    int /*rowNumber*/, //-V2009 overridden method
    int /*width*/, int /*height*/, bool row_is_selected)
{
   // This must draw the background behind one of the rows in the table.

   // The graphics context has its origin at the row's top-left, and your method
   // should fill the area specified by the width and height parameters.

   // Note that the rowNumber value may be greater than the number of rows in your
   // list, so be careful that you don't assume it's less than getNumRows().
   if (row_is_selected)
      g.fillAll(juce::Colours::lightblue);
}

void CommandTableModel::paintCell(
    juce::Graphics& g, int row_number, int column_id, int width, int height, bool /*rowIsSelected*/)
{
   try {
      juce::String format_str;
      // This must draw one of the cells.

      // The graphics context's origin will already be set to the top-left of the
      // cell, whose size is specified by(width, height).

      // Note that the rowNumber value may be greater than the number of rows in your
      // list, so be careful that you don't assume it's less than getNumRows().
      g.setColour(juce::Colours::black);
      g.setFont(12.0f);
      if (column_id == 1) // write the MIDI message in the MIDI command column
      {
         auto value = 0;
         auto channel = 0;
         std::shared_lock<decltype(cmd_table_mod_mtx_)> lck{cmd_table_mod_mtx_};
         // cmdmap_mutex_ should fix the following problem
         if (commands_.size()
             <= gsl::narrow_cast<size_t>(row_number)) { // guess--command cell not filled yet
            g.drawText("Unknown control", 0, 0, width, height, juce::Justification::centred);
         }
         else {
            switch (const auto cmd = commands_.at(gsl::narrow_cast<size_t>(row_number));
                    cmd.msg_id_type) {
            case rsj::MsgIdEnum::kNote:
               format_str = "%d | Note: %d";
               channel = cmd.channel;
               value = cmd.data;
               break;
            case rsj::MsgIdEnum::kCc:
               format_str = "%d | CC: %d";
               channel = cmd.channel;
               value = cmd.data;
               break;
            case rsj::MsgIdEnum::kPitchBend:
               format_str = "%d | Pitch: %d";
               channel = cmd.channel;
               break;
            }
            g.drawText(juce::String::formatted(format_str, channel, value), 0, 0, width, height,
                juce::Justification::centred);
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

juce::Component* CommandTableModel::refreshComponentForCell(int row_number, int column_id,
    bool /*isRowSelected*/, juce::Component* existing_component_to_update)
{
   try {
      // This is used to create or update a custom component to go in a cell.

      // Any cell may contain a custom component, or can just be drawn with the
      // paintCell() method and handle mouse clicks with cellClicked().

      // This method will be called whenever a custom component might need to be
      // updated - e.g. when the table is changed, or TableListBox::updateContent()
      // is called.

      // If you don't need a custom component for the specified cell, then return
      // nullptr. (Bear in mind that even if you're not creating a new component,
      // you may still need to delete existingComponentToUpdate if it's non-null).

      // If you do want a custom component, and the existingComponentToUpdate is
      // null, then this method must create a new component suitable for the cell,
      // and return it.

      // If the existingComponentToUpdate is non - null, it will be a pointer to a
      // component previously created by this method.In this case, the method must
      // either update it to make sure it's correctly representing the given
      // cell(which may be different from the one that the component was created
      // for), or it can delete this component and return a new one.
      //// Because Juce recycles these components when scrolling, we need to reset
      //// their properties
      if (column_id == 2) // LR command column
      {
         auto command_select = dynamic_cast<CommandMenu*>(existing_component_to_update);

         // create a new command menu
         std::shared_lock<decltype(cmd_table_mod_mtx_)> lck{cmd_table_mod_mtx_};
         if (command_select == nullptr) {
#pragma warning(suppress : 26400 26409 24623 24624)
            command_select = new CommandMenu{commands_.at(gsl::narrow_cast<size_t>(row_number))};
            command_select->Init(command_map_);
         }
         else
            command_select->SetMsg(commands_.at(gsl::narrow_cast<size_t>(row_number)));

         if (command_map_)
            // add 1 because 0 is reserved for no selection
            command_select->SetSelectedItem(
                LrCommandList::GetIndexOfCommand(command_map_->GetCommandforMessage(
                    commands_.at(gsl::narrow_cast<size_t>(row_number))))
                + 1);

         return command_select;
      }
      return nullptr;
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void CommandTableModel::AddRow(int midi_channel, int midi_data, rsj::MsgIdEnum msg_type)
{
   try {
      const rsj::MidiMessageId msg{midi_channel, midi_data, msg_type};
      std::unique_lock<decltype(cmd_table_mod_mtx_)> grd{cmd_table_mod_mtx_};
      if (command_map_ && !command_map_->MessageExistsInMap(msg)) {
         commands_.push_back(msg);
         command_map_->AddCommandforMessage(0, msg); // add an entry for 'no command'
         Sort();                                     // re-sort list
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void CommandTableModel::RemoveRow(size_t row)
{
   try {
      std::unique_lock<decltype(cmd_table_mod_mtx_)> grd{cmd_table_mod_mtx_};
      if (command_map_)
         command_map_->RemoveMessage(commands_.at(row));
      commands_.erase(commands_.cbegin() + row);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void CommandTableModel::RemoveAllRows()
{
   std::unique_lock<decltype(cmd_table_mod_mtx_)> grd{cmd_table_mod_mtx_};
   commands_.clear();
   if (command_map_)
      command_map_->ClearMap();
}

void CommandTableModel::BuildFromXml(const juce::XmlElement* root)
{
   try {
      // mutex here causes deadlock this method calls CommandTableModel methods
      // that lock the mutext, so don't lock it here just use mutex for Sort
      if (!root || root->getTagName().compare("settings") != 0)
         return;
      RemoveAllRows();
      const auto* setting = root->getFirstChildElement();
      while (setting && command_map_) {
         if (setting->hasAttribute("controller")) {
            const rsj::MidiMessageId message{setting->getIntAttribute("channel"),
                setting->getIntAttribute("controller"), rsj::MsgIdEnum::kCc};
            AddRow(message.channel, message.data, rsj::MsgIdEnum::kCc);
            command_map_->AddCommandforMessage(
                setting->getStringAttribute("command_string").toStdString(), message);
         }
         else if (setting->hasAttribute("note")) {
            const rsj::MidiMessageId note{setting->getIntAttribute("channel"),
                setting->getIntAttribute("note"), rsj::MsgIdEnum::kNote};
            AddRow(note.channel, note.data, rsj::MsgIdEnum::kNote);
            command_map_->AddCommandforMessage(
                setting->getStringAttribute("command_string").toStdString(), note);
         }
         else if (setting->hasAttribute("pitchbend")) {
            const rsj::MidiMessageId pb{
                setting->getIntAttribute("channel"), 0, rsj::MsgIdEnum::kPitchBend};
            AddRow(pb.channel, 0, rsj::MsgIdEnum::kPitchBend);
            command_map_->AddCommandforMessage(
                setting->getStringAttribute("command_string").toStdString(), pb);
         }
         setting = setting->getNextElement();
      }
      std::unique_lock<decltype(cmd_table_mod_mtx_)> grd{cmd_table_mod_mtx_};
      Sort();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

int CommandTableModel::GetRowForMessage(
    int midi_channel, int midi_data, rsj::MsgIdEnum msg_type) const
{
   std::shared_lock<decltype(cmd_table_mod_mtx_)> grd{cmd_table_mod_mtx_};
   const rsj::MidiMessageId msg_id{midi_channel, midi_data, msg_type};
   return gsl::narrow_cast<int>(
       std::find(commands_.begin(), commands_.end(), msg_id) - commands_.begin());
}

void CommandTableModel::Sort()
{ // always call within unique_lock
   // use LRCommandList::getIndexOfCommand(string); to sort by command
   const auto msg_idx = [this](rsj::MidiMessageId a) {
      return LrCommandList::GetIndexOfCommand(command_map_->GetCommandforMessage(a));
   };
   const auto msg_sort = [&msg_idx](rsj::MidiMessageId a, rsj::MidiMessageId b) {
      return msg_idx(a) < msg_idx(b);
   };

   if (current_sort_.first == 1)
      if (current_sort_.second)
         std::sort(commands_.begin(), commands_.end());
      else
         std::sort(commands_.rbegin(), commands_.rend());
   else if (current_sort_.second)
      std::sort(commands_.begin(), commands_.end(), msg_sort);
   else
      std::sort(commands_.rbegin(), commands_.rend(), msg_sort);
}