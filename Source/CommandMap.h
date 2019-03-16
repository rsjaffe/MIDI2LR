#ifndef MIDI2LR_COMMANDMAP_H_INCLUDED
#define MIDI2LR_COMMANDMAP_H_INCLUDED
/*
  ==============================================================================

    CommandMap.h

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
#include <map>
#include <shared_mutex>
#include <string>
#include <unordered_map>

#include <gsl/gsl>
#include <JuceLibraryCode/JuceHeader.h>
#include "CommandSet.h"
#include "Misc.h"
#include "MidiUtilities.h"

// All methods with underscore at end don't include a mutex and are for internal use only. Methods
// without underscore do have mutex and could be called by another class
class CommandMap {
 public:
   void AddCommandforMessage(const std::string& command, const rsj::MidiMessageId& message);
   void AddCommandforMessage(size_t command, const rsj::MidiMessageId& message);
   void AddRowMapped(const std::string& command, const rsj::MidiMessageId message);
   void AddRowUnmapped(int midi_channel, int midi_data, rsj::MsgIdEnum msg_type);
   void BuildFromXml(const juce::XmlElement* root);
   [[nodiscard]] bool CommandHasAssociatedMessage(const std::string& command) const;
   [[nodiscard]] const std::string& GetCommandforMessage(const rsj::MidiMessageId& message) const;
   [[nodiscard]] rsj::MidiMessageId GetMessageForNumber(size_t num) const;
   std::vector<const rsj::MidiMessageId*> GetMessagesForCommand(const std::string& command) const;
   [[nodiscard]] int GetRowForMessage(
       int midi_channel, int midi_data, rsj::MsgIdEnum msg_type) const;
   [[nodiscard]] bool MessageExistsInMap(const rsj::MidiMessageId& message) const;
   void RemoveAllRows();
   void RemoveMessage(const rsj::MidiMessageId& message);
   void RemoveRow(size_t row);
   void Resort(std::pair<int, bool> new_order);
   size_t Size() const;
   void ToXmlDocument(const juce::File& file) const;

 private:
   void AddCommandforMessage_(const std::string& command, const rsj::MidiMessageId& message);
   void AddCommandforMessage_(size_t command, const rsj::MidiMessageId& message);
   const std::string& GetCommandforMessage_(const rsj::MidiMessageId& message) const
   {
      return message_map_.at(message);
   }
   bool MessageExistsInMap_(const rsj::MidiMessageId& message) const
   {
      return message_map_.find(message) != message_map_.end();
   }
   rsj::MidiMessageId GetMessageForNumber_(size_t num) const
   {
      return command_table_.at(num);
   }
   void RemoveAllRows_();
   void Sort_();

   CommandSet command_set_{};
   mutable std::shared_mutex cmdmap_mtx_;
   std::multimap<std::string, rsj::MidiMessageId> command_string_map_;
   std::pair<int, bool> current_sort_{2, true};
   std::unordered_map<rsj::MidiMessageId, std::string> message_map_;
   std::vector<rsj::MidiMessageId> command_table_;
};

inline void CommandMap::AddCommandforMessage_(
    const std::string& command, const rsj::MidiMessageId& message)
{ // only called from BuildFromXml, rely on mtx there
   message_map_[message] = command;
   command_string_map_.emplace(command, message);
}

inline const std::string& CommandMap::GetCommandforMessage(const rsj::MidiMessageId& message) const
{ // only called by outsiders, use mutex
   auto guard = std::shared_lock{cmdmap_mtx_};
   return GetCommandforMessage_(message);
}

inline void CommandMap::RemoveAllRows()
{ // external use
   auto guard = std::unique_lock{cmdmap_mtx_};
   RemoveAllRows_();
}

inline void CommandMap::RemoveMessage(const rsj::MidiMessageId& message)
{ // for external calls, uses mutex
   // removes message from the message:command map, and its associated command
   // from the command:message map
   auto guard = std::unique_lock{cmdmap_mtx_};
   command_string_map_.erase(message_map_.at(message));
   message_map_.erase(message);
}

inline bool CommandMap::MessageExistsInMap(const rsj::MidiMessageId& message) const
{ // external use
   auto guard = std::shared_lock{cmdmap_mtx_};
   return MessageExistsInMap_(message);
}

inline bool CommandMap::CommandHasAssociatedMessage(const std::string& command) const
{ // external use
   auto guard = std::shared_lock{cmdmap_mtx_};
   return command_string_map_.find(command) != command_string_map_.end();
}

inline void CommandMap::AddCommandforMessage(size_t command, const rsj::MidiMessageId& message)
{ // external use
   auto guard = std::unique_lock{cmdmap_mtx_};
   AddCommandforMessage_(command, message);
}

inline void CommandMap::AddCommandforMessage(
    const std::string& command, const rsj::MidiMessageId& message)
{
   auto guard = std::unique_lock{cmdmap_mtx_};
   AddCommandforMessage_(command, message);
}

inline rsj::MidiMessageId CommandMap::GetMessageForNumber(size_t num) const
{
   auto guard = std::shared_lock{cmdmap_mtx_};
   return command_table_.at(num);
}
inline size_t CommandMap::Size() const
{
   auto guard = std::shared_lock{cmdmap_mtx_};
   return command_table_.size();
}
#endif // COMMANDMAP_H_INCLUDED
