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
#include "MidiUtilities.h"

// All methods with underscore at end don't include a mutex and are for internal use only. Methods
// without underscore do have mutex and could be called by another class
class CommandMap {
 public:
   void AddCommandForMessage(size_t command, rsj::MidiMessageId message);
   void AddRowMapped(const std::string& command, rsj::MidiMessageId message);
   void AddRowUnmapped(rsj::MidiMessageId message);
   [[nodiscard]] bool CommandHasAssociatedMessage(const std::string& command) const;
   void FromXml(const juce::XmlElement* root);
   [[nodiscard]] const std::string& GetCommandForMessage(rsj::MidiMessageId message) const;
   [[nodiscard]] rsj::MidiMessageId GetMessageForNumber(size_t num) const;
   [[nodiscard]] std::vector<rsj::MidiMessageId> GetMessagesForCommand(
       const std::string& command) const;
   [[nodiscard]] int GetRowForMessage(rsj::MidiMessageId message) const;
   [[nodiscard]] bool MessageExistsInMap(rsj::MidiMessageId message) const;
   void RemoveAllRows();
   void RemoveMessage(rsj::MidiMessageId message);
   void RemoveRow(size_t row);
   void Resort(std::pair<int, bool> new_order);
   [[nodiscard]] size_t Size() const;
   void ToXmlFile(const juce::File& file) const;

 private:
   void AddCommandForMessage_(size_t command, rsj::MidiMessageId message);
   const std::string& GetCommandForMessage_(rsj::MidiMessageId message) const
   {
      return message_map_.at(message);
   }
   bool MessageExistsInMap_(rsj::MidiMessageId message) const
   {
      return message_map_.find(message) != message_map_.end();
   }
   rsj::MidiMessageId GetMessageForNumber_(size_t num) const
   {
      return command_table_.at(num);
   }
   void Sort_();

   CommandSet command_set_{};
   mutable std::shared_mutex cmdmap_mtx_;
   std::multimap<std::string, rsj::MidiMessageId> command_string_map_;
   std::pair<int, bool> current_sort_{2, true};
   std::unordered_map<rsj::MidiMessageId, std::string> message_map_;
   std::vector<rsj::MidiMessageId> command_table_;
};

inline void CommandMap::AddCommandForMessage(size_t command, rsj::MidiMessageId message)
{
   auto guard = std::unique_lock{cmdmap_mtx_};
   AddCommandForMessage_(command, message);
}

inline bool CommandMap::CommandHasAssociatedMessage(const std::string& command) const
{
   auto guard = std::shared_lock{cmdmap_mtx_};
   return command_string_map_.find(command) != command_string_map_.end();
}

inline const std::string& CommandMap::GetCommandForMessage(rsj::MidiMessageId message) const
{
   auto guard = std::shared_lock{cmdmap_mtx_};
   return GetCommandForMessage_(message);
}

inline rsj::MidiMessageId CommandMap::GetMessageForNumber(size_t num) const
{
   auto guard = std::shared_lock{cmdmap_mtx_};
   return command_table_.at(num);
}

inline int CommandMap::GetRowForMessage(rsj::MidiMessageId message) const
{
   auto guard = std::shared_lock{cmdmap_mtx_};
   return gsl::narrow_cast<int>(
       std::find(command_table_.begin(), command_table_.end(), message) - command_table_.begin());
}

inline size_t CommandMap::Size() const
{
   auto guard = std::shared_lock{cmdmap_mtx_};
   return command_table_.size();
}

inline bool CommandMap::MessageExistsInMap(rsj::MidiMessageId message) const
{
   auto guard = std::shared_lock{cmdmap_mtx_};
   return MessageExistsInMap_(message);
}

inline void CommandMap::RemoveAllRows()
{
   auto guard = std::unique_lock{cmdmap_mtx_};
   command_string_map_.clear();
   command_table_.clear();
   message_map_.clear();
}

inline void CommandMap::RemoveMessage(rsj::MidiMessageId message)
{
   auto guard = std::unique_lock{cmdmap_mtx_};
   command_string_map_.erase(message_map_.at(message));
   message_map_.erase(message);
}

#endif // COMMANDMAP_H_INCLUDED
