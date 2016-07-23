/*
  ==============================================================================

  CommandTableModel.cpp

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
#include "CommandTableModel.h"
#include <algorithm>
#include "LRCommands.h"

CommandTableModel::CommandTableModel() noexcept {}

void CommandTableModel::Init(std::shared_ptr<CommandMap>& map_command) noexcept {
    //copy the pointer
  command_map_ = map_command;
}

/**
*/
void CommandTableModel::sortOrderChanged(int newSortColumnId, bool isForwards) {
  //This callback is made when the table's sort order is changed.

  // This could be because the user has clicked a column header, or because the
  // TableHeaderComponent::setSortColumnId() method was called.

  // If you implement this, your method should re - sort the table using the
  // given column as the key.
  current_sort = std::make_pair(newSortColumnId, isForwards);
  Sort();
}

int CommandTableModel::getNumRows() {
  //This must return the number of rows currently in the table.

  // If the number of rows changes, you must call TableListBox::updateContent()
  // to cause it to refresh the list.
  return commands_.size();
}

void CommandTableModel::paintRowBackground(juce::Graphics& g, int /*rowNumber*/,
  int /*width*/, int /*height*/, bool row_is_selected) {
  //This must draw the background behind one of the rows in the table.

  // The graphics context has its origin at the row's top-left, and your method
  // should fill the area specified by the width and height parameters.

  // Note that the rowNumber value may be greater than the number of rows in your
  // list, so be careful that you don't assume it's less than getNumRows().
  if (row_is_selected)
    g.fillAll(juce::Colours::lightblue);
}

void CommandTableModel::paintCell(juce::Graphics& g, int row_number, int column_id,
  int width, int height, bool /*rowIsSelected*/) {
  //This must draw one of the cells.

  // The graphics context's origin will already be set to the top-left of the
  // cell, whose size is specified by(width, height).

  // Note that the rowNumber value may be greater than the number of rows in your
  // list, so be careful that you don't assume it's less than getNumRows().
  g.setColour(juce::Colours::black);
  g.setFont(12.0f);

  if (column_id == 1) // write the MIDI message in the MIDI command column
  {
    if (commands_[row_number].isCC)
      g.drawText(juce::String::formatted("%d | CC: %d", commands_[row_number].channel,
        commands_[row_number].controller), 0, 0, width, height, juce::Justification::centred);
    else
      g.drawText(String::formatted("%d | Note: %d", commands_[row_number].channel,
        commands_[row_number].pitch), 0, 0, width, height, Justification::centred);
  }
}

juce::Component *CommandTableModel::refreshComponentForCell(int row_number,
  int column_id, bool /*isRowSelected*/, juce::Component *existing_component_to_update) {
    //This is used to create or update a custom component to go in a cell.

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
    CommandMenu* command_select = dynamic_cast<CommandMenu *>(existing_component_to_update);

    // create a new command menu
    if (command_select == nullptr) {
      command_select = new CommandMenu{commands_[row_number]};
      command_select->Init(command_map_);
    }
    else
      command_select->setMsg(commands_[row_number]);

    if (command_map_) {
        // add 1 because 0 is reserved for no selection
      command_select->setSelectedItem(LRCommandList::getIndexOfCommand(command_map_->
        getCommandforMessage(commands_[row_number])) + 1);
    }

    return command_select;
  }
  else
    return nullptr;
}

void CommandTableModel::addRow(int midi_channel, int midi_data, bool is_cc) {
  const MIDI_Message_ID msg{midi_channel, midi_data, is_cc};
  if (command_map_ && !command_map_->messageExistsInMap(msg)) {
    commands_.push_back(msg);
    command_map_->addCommandforMessage(0, msg); // add an entry for 'no command'
    Sort(); //re-sort list
  }
}

void CommandTableModel::removeRow(int row) {
  if (command_map_) {
    command_map_->removeMessage(commands_[row]);
  }
  commands_.erase(commands_.cbegin() + row);
}

void CommandTableModel::removeAllRows() {
  commands_.clear();

  if (command_map_) {
    command_map_->clearMap();
  }
}

void CommandTableModel::buildFromXml(const juce::XmlElement * const root) {
  if (root->getTagName().compare("settings") != 0)
    return;

  removeAllRows();

  auto* setting = root->getFirstChildElement();
  while ((setting) && (command_map_)) {
    if (setting->hasAttribute("controller")) {
      const MIDI_Message_ID message{setting->getIntAttribute("channel"),
        setting->getIntAttribute("controller"), true};
      addRow(message.channel, message.controller, true);

      // older versions of MIDI2LR stored the index of the string, so we should attempt to parse this as well
      if (setting->getIntAttribute("command", -1) != -1) {
        command_map_->addCommandforMessage(setting->
          getIntAttribute("command"), message);
      }
      else {
        command_map_->addCommandforMessage(setting->
          getStringAttribute("command_string").toStdString(), message);
      }
    }
    else if (setting->hasAttribute("note")) {
      const MIDI_Message_ID note{setting->getIntAttribute("channel"),
        setting->getIntAttribute("note"), false};
      addRow(note.channel, note.pitch, false);

      // older versions of MIDI2LR stored the index of the string, so we should attempt to parse this as well
      if (setting->getIntAttribute("command", -1) != -1) {
        command_map_->addCommandforMessage(setting->getIntAttribute("command"),
          note);
      }
      else {
        command_map_->addCommandforMessage(setting->
          getStringAttribute("command_string").toStdString(), note);
      }
    }
    setting = setting->getNextElement();
  }
  Sort();
}

int CommandTableModel::getRowForMessage(int midi_channel, int midi_data, bool is_cc) const {
  for (size_t idx = 0u; idx < commands_.size(); idx++) {
    if (commands_[idx].channel == midi_channel && commands_[idx].controller == midi_data
      && commands_[idx].isCC == is_cc)
      return idx;
  }
  //could not find
  return -1;
}

void CommandTableModel::Sort() {
  // use LRCommandList::getIndexOfCommand(string); to sort by command
  // sort the command map
  const auto msg_idx = [this](MIDI_Message_ID a) {return LRCommandList::getIndexOfCommand
  (command_map_->getCommandforMessage(a)); };

  if (current_sort.first == 1)
    if (current_sort.second)
      std::sort(commands_.begin(), commands_.end());
    else
      std::sort(commands_.rbegin(), commands_.rend());
  else
    if (current_sort.second)
      std::sort(commands_.begin(), commands_.end(),
        [&msg_idx](MIDI_Message_ID a, MIDI_Message_ID b) { return msg_idx(a) < msg_idx(b); });
    else
      std::sort(commands_.rbegin(), commands_.rend(),
        [&msg_idx](MIDI_Message_ID a, MIDI_Message_ID b) { return msg_idx(a) < msg_idx(b); });
}