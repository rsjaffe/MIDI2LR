/*
  ==============================================================================

    CommandMap.cpp

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
#include "CommandMap.h"

#include <exception>

#include <gsl/gsl>
#include "Misc.h"

void CommandMap::Resort(std::pair<int, bool> new_order)
{
   auto guard = std::unique_lock{cmdmap_mtx_};
   current_sort_ = new_order;
   Sort_();
}

void CommandMap::AddCommandforMessage_(size_t command, const rsj::MidiMessageId& message)
{ // internal use
   if (command < command_set_.CommandAbbrevSize()) {
      auto cmd_abbreviation = command_set_.CommandAbbrevAt(command);
      message_map_[message] = cmd_abbreviation;
      command_string_map_.emplace(cmd_abbreviation, message);
   }
}

void CommandMap::AddRowMapped(const std::string& command, const rsj::MidiMessageId message)
{
   auto guard = std::unique_lock{cmdmap_mtx_};
   if (!MessageExistsInMap_(message)) {
      message_map_[message] = command;
      command_string_map_.emplace(command, message);
      command_table_.push_back(message);
      Sort_();
   }
}

std::vector<const rsj::MidiMessageId*> CommandMap::GetMessagesForCommand(
    const std::string& command) const
{ // external use only
   auto guard = std::shared_lock{cmdmap_mtx_};
   std::vector<const rsj::MidiMessageId*> mm;
   const auto range = command_string_map_.equal_range(command);
   for (auto it = range.first; it != range.second; ++it)
      mm.push_back(&it->second);
   return mm;
}

void CommandMap::ToXmlDocument(const juce::File& file) const
{ // external use only
   try {
      auto guard = std::shared_lock{cmdmap_mtx_};
      if (!message_map_.empty()) { // don't bother if map is empty
         // save the contents of the command map to an xml file
         juce::XmlElement root{"settings"};
         for (const auto& map_entry : message_map_) {
            auto setting = std::make_unique<juce::XmlElement>("setting");
            setting->setAttribute("channel", map_entry.first.channel);
            switch (map_entry.first.msg_id_type) {
            case rsj::MsgIdEnum::kNote:
               setting->setAttribute("note", map_entry.first.data);
               break;
            case rsj::MsgIdEnum::kCc:
               setting->setAttribute("controller", map_entry.first.data);
               break;
            case rsj::MsgIdEnum::kPitchBend:
               setting->setAttribute("pitchbend", 0);
               break;
            }
            setting->setAttribute("command_string", map_entry.second);
            root.addChildElement(setting.release());
         }
         if (!root.writeToFile(file, "")) {
            // Give feedback if file-save doesn't work
            rsj::LogAndAlertError("Unable to save file as specified. Please try again, and "
                                  "consider saving to a different location. "
                                  + file.getFullPathName());
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}



void CommandMap::AddRowUnmapped(int midi_channel, int midi_data, rsj::MsgIdEnum msg_type)
{ 
   auto guard = std::unique_lock{cmdmap_mtx_};
   const rsj::MidiMessageId msg{midi_channel, midi_data, msg_type};
   if (!MessageExistsInMap_(msg)) {
      AddCommandforMessage_(0, msg); // add an entry for 'no command'
      command_table_.push_back(msg);
      Sort_();
   }
}

void CommandMap::BuildFromXml(const juce::XmlElement* root)
{ // external use only, but mutex here causes deadlock. will use external versions of CommandMap
  // calls to lock individual accesses, manually lock any internal calls
  // CommandTableModel calls CommandMap from refreshComponentForCell and Sort_
   try {
      // auto guard = std::unique_lock{cmdmap_mtx_};
      if (!root || root->getTagName().compare("settings") != 0)
         return;
      RemoveAllRows();
      const auto* setting = root->getFirstChildElement();
      while (setting) {
         if (setting->hasAttribute("controller")) {
            const rsj::MidiMessageId message{setting->getIntAttribute("channel"),
                setting->getIntAttribute("controller"), rsj::MsgIdEnum::kCc};
            AddRowMapped(setting->getStringAttribute("command_string").toStdString(), message);
         }
         else if (setting->hasAttribute("note")) {
            const rsj::MidiMessageId note{setting->getIntAttribute("channel"),
                setting->getIntAttribute("note"), rsj::MsgIdEnum::kNote};
            AddRowMapped(setting->getStringAttribute("command_string").toStdString(), note);
         }
         else if (setting->hasAttribute("pitchbend")) {
            const rsj::MidiMessageId pb{
                setting->getIntAttribute("channel"), 0, rsj::MsgIdEnum::kPitchBend};
            AddRowMapped(setting->getStringAttribute("command_string").toStdString(), pb);
         }
         setting = setting->getNextElement();
      }
      auto guard = std::unique_lock{cmdmap_mtx_};
      Sort_();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void CommandMap::RemoveAllRows_()
{ // internal use
   command_string_map_.clear();
   command_table_.clear();
   message_map_.clear();
}

void CommandMap::RemoveRow(size_t row)
{ // external callers only
   auto guard = std::unique_lock{cmdmap_mtx_};
   const auto msg = GetMessageForNumber_(row);
   command_string_map_.erase(message_map_.at(msg));
   command_table_.erase(command_table_.cbegin() + row);
   message_map_.erase(msg);
}

void CommandMap::Sort_()
{ // always call within unique_lock. only call from tablemodel is sortOrderChanged callback
   // use LRCommandList::getIndexOfCommand(string); to sort by command
   const auto msg_idx = [this](rsj::MidiMessageId a) {
      return command_set_.CommandTextIndex(GetCommandforMessage_(a));
   };
   const auto msg_sort = [&msg_idx](rsj::MidiMessageId a, rsj::MidiMessageId b) {
      return msg_idx(a) < msg_idx(b);
   };

   if (current_sort_.first == 1)
      if (current_sort_.second)
         std::sort(command_table_.begin(), command_table_.end());
      else
         std::sort(command_table_.rbegin(), command_table_.rend());
   else if (current_sort_.second)
      std::sort(command_table_.begin(), command_table_.end(), msg_sort);
   else
      std::sort(command_table_.rbegin(), command_table_.rend(), msg_sort);
}

int CommandMap::GetRowForMessage(int midi_channel, int midi_data, rsj::MsgIdEnum msg_type) const
{
   const rsj::MidiMessageId msg_id{midi_channel, midi_data, msg_type};
   return gsl::narrow_cast<int>(
       std::find(command_table_.begin(), command_table_.end(), msg_id) - command_table_.begin());
}