/*
  ==============================================================================

  CommandTableModel.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/
#include "CommandTableModel.h"
#include "LRCommands.h"

CommandTableModel::CommandTableModel() noexcept {}

int CommandTableModel::getNumRows() {
  return rows_;
}

void CommandTableModel::paintRowBackground(Graphics &g, int /*rowNumber*/, 
  int /*width*/, int /*height*/, bool row_is_selected) {
  if (row_is_selected)
    g.fillAll(Colours::lightblue);
}

void CommandTableModel::paintCell(Graphics &g, int row_number, int column_id, 
  int width, int height, bool /*rowIsSelected*/) {
  g.setColour(Colours::black);
  g.setFont(12.0f);

  if (column_id == 1) // write the MIDI message in the MIDI command column
  {
    if (commands_[row_number].isCC)
      g.drawText(String::formatted("%d | CC: %d", commands_[row_number].channel,
      commands_[row_number].controller), 0, 0, width, height, Justification::centred);
    else
      g.drawText(String::formatted("%d | Note: %d", commands_[row_number].channel,
      commands_[row_number].pitch), 0, 0, width, height, Justification::centred);
  }
}

Component *CommandTableModel::refreshComponentForCell(int row_number, 
  int column_id, bool /*isRowSelected*/, Component *existing_component_to_update) {
  if (column_id == 2) // LR command column
  {
      // because Juce recycles these components when scrolling, we need to reset their properties
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
  MIDI_Message msg{midi_channel, midi_data, is_cc};
  if (command_map_) {
    if (!command_map_->messageExistsInMap(msg)) {
      commands_.push_back(msg);
      command_map_->addCommandforMessage(0, msg); // add an entry for 'no command'
      rows_++;
    }
  }
}

int CommandTableModel::getRowForMessage(int midi_channel, int midi_data, bool is_cc) const {
  for (auto idx = 0; idx < rows_; idx++) {
    if (commands_[idx].channel == midi_channel && commands_[idx].controller == midi_data
      && commands_[idx].isCC == is_cc)
      return idx;
  }
  //could not find
  return -1;
}

void CommandTableModel::removeRow(int row) {
  MIDI_Message msg = commands_[row];
  commands_.erase(commands_.begin() + row);
  if (command_map_) {
    command_map_->removeMessage(msg);
  }
  rows_--;
}

void CommandTableModel::removeAllRows() {
  commands_.clear();

  if (command_map_) {
    command_map_->clearMap();
  }

  rows_ = 0;
}

void CommandTableModel::buildFromXml(const XmlElement * const root) {
  if (root->getTagName().compare("settings") != 0)
    return;

  removeAllRows();

  auto* setting = root->getFirstChildElement();
  while ((setting) && (command_map_)) {
    if (setting->hasAttribute("controller")) {
      MIDI_Message message{setting->getIntAttribute("channel"), 
        setting->getIntAttribute("controller"), true};
      addRow(message.channel, message.controller, true);

      // older versions of MIDI2LR stored the index of the string, so we should attempt to parse this as well
      if (setting->getIntAttribute("command", -1) != -1) {
        command_map_->addCommandforMessage(setting->
          getIntAttribute("command"), message);
      }
      else {
        command_map_->addCommandforMessage(setting->
          getStringAttribute("command_string"), message);
      }
    }
    else if (setting->hasAttribute("note")) {
      MIDI_Message note{setting->getIntAttribute("channel"), 
        setting->getIntAttribute("note"), false};
      addRow(note.channel, note.pitch, false);

      // older versions of MIDI2LR stored the index of the string, so we should attempt to parse this as well
      if (setting->getIntAttribute("command", -1) != -1) {
        command_map_->addCommandforMessage(setting->getIntAttribute("command"), 
          note);
      }
      else {
        command_map_->addCommandforMessage(setting->
          getStringAttribute("command_string"), note);
      }
    }
    setting = setting->getNextElement();
  }
}

void CommandTableModel::Init(std::shared_ptr<CommandMap>& map_command) noexcept {
    //copy the pointer
  command_map_ = map_command;
}