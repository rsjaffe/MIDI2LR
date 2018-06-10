// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
#include <exception>
#include "CommandMap.h"
#include "LRCommands.h"
#include "Misc.h"

void CommandMap::AddCommandforMessage(size_t command, const rsj::MidiMessageId& message)
{
   try {
      // adds a message to the message:command map, and its associated command to the
      // command:message map
      if (command < LrCommandList::LrStringList.size()) {
         auto cmd_abbreviation = LrCommandList::LrStringList.at(command);
         message_map_[message] = cmd_abbreviation;
         command_string_map_.emplace(cmd_abbreviation, message);
      }
      else
         message_map_[message] =
             LrCommandList::NextPrevProfile.at(command - LrCommandList::LrStringList.size());
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

std::vector<const rsj::MidiMessageId*> CommandMap::GetMessagesForCommand(
    const std::string& command) const
{
   try {
      std::vector<const rsj::MidiMessageId*> mm;
      const auto range = command_string_map_.equal_range(command);
      for (auto it = range.first; it != range.second; ++it)
         mm.push_back(&it->second);
      return mm;
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void CommandMap::ToXmlDocument(const juce::File& file) const
{
   try {
      if (!message_map_.empty()) { // don't bother if map is empty
         // save the contents of the command map to an xml file
         juce::XmlElement root{"settings"};
         for (const auto& map_entry : message_map_) {
            auto setting = std::make_unique<juce::XmlElement>("setting");
            setting->setAttribute("channel", map_entry.first.channel);
            switch (map_entry.first.msg_id_type) {
            case rsj::MsgIdEnum::kNote:
               setting->setAttribute("note", map_entry.first.pitch);
               break;
            case rsj::MsgIdEnum::kCc:
               setting->setAttribute("controller", map_entry.first.controller);
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
            rsj::LogAndAlertError(
                "Unable to save file as specified. Please try again, and consider saving to a "
                "different location.");
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}