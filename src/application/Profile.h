#ifndef MIDI2LR_PROFILE_H_INCLUDED
#define MIDI2LR_PROFILE_H_INCLUDED
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
//-V813_MINSIZE=13 /* warn if passing structure by value > 12 bytes (3*sizeof(int)) */

#include <map>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <gsl/gsl>

#include <juce_core/juce_core.h>

#include "CommandSet.h"
#include "MidiUtilities.h"

/* All methods with I at end don't include a mutex and are for internal use only. Methods without I
 * do have mutex and could be called by another class */
class Profile {
 public:
   explicit Profile(const CommandSet& command_set) noexcept : command_set_ {command_set} {}
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
   [[nodiscard]] bool ProfileUnsaved() const;
   void RemoveAllRows();
   void RemoveMessage(rsj::MidiMessageId message);
   void RemoveRow(size_t row);
   void RemoveUnassignedMessages();
   void Resort(std::pair<int, bool> new_order);
   [[nodiscard]] size_t Size() const;
   void ToXmlFile(const juce::File& file);

 private:
   void AddCommandForMessageI(size_t command, rsj::MidiMessageId message);
   const std::string& GetCommandForMessageI(rsj::MidiMessageId message) const;
   rsj::MidiMessageId GetMessageForNumberI(size_t num) const;
   bool MessageExistsInMapI(rsj::MidiMessageId message) const;
   void SortI();

   bool profile_unsaved_ {false};
   const CommandSet& command_set_;
   mutable std::shared_mutex mutex_;
   std::multimap<std::string, rsj::MidiMessageId> command_string_map_ {};
   std::pair<int, bool> current_sort_ {2, true};
   std::unordered_map<rsj::MidiMessageId, std::string> message_map_ {};
   std::unordered_map<rsj::MidiMessageId, std::string> saved_map_ {};
   std::vector<rsj::MidiMessageId> command_table_ {};
};

inline void Profile::AddCommandForMessage(size_t command, rsj::MidiMessageId message)
{
   auto guard {std::unique_lock {mutex_}};
   AddCommandForMessageI(command, message);
}

inline bool Profile::CommandHasAssociatedMessage(const std::string& command) const
{
   auto guard {std::shared_lock {mutex_}};
   return command_string_map_.find(command) != command_string_map_.end();
}

inline const std::string& Profile::GetCommandForMessage(rsj::MidiMessageId message) const
{
   auto guard {std::shared_lock {mutex_}};
   return GetCommandForMessageI(message);
}

inline const std::string& Profile::GetCommandForMessageI(rsj::MidiMessageId message) const
{
   return message_map_.at(message);
}

inline rsj::MidiMessageId Profile::GetMessageForNumber(size_t num) const
{
   auto guard {std::shared_lock {mutex_}};
   return GetMessageForNumberI(num);
}

inline rsj::MidiMessageId Profile::GetMessageForNumberI(size_t num) const
{
   return command_table_.at(num);
}

inline int Profile::GetRowForMessage(rsj::MidiMessageId message) const
{
   auto guard {std::shared_lock {mutex_}};
   return gsl::narrow_cast<int>(
       std::find(command_table_.begin(), command_table_.end(), message) - command_table_.begin());
}

inline bool Profile::MessageExistsInMap(rsj::MidiMessageId message) const
{
   auto guard {std::shared_lock {mutex_}};
   return MessageExistsInMapI(message);
}

inline bool Profile::MessageExistsInMapI(rsj::MidiMessageId message) const
{
   return message_map_.find(message) != message_map_.end();
}

inline bool Profile::ProfileUnsaved() const
{
   auto guard {std::shared_lock {mutex_}};
   return profile_unsaved_ && !command_table_.empty() && saved_map_ != message_map_;
}

inline size_t Profile::Size() const
{
   auto guard {std::shared_lock {mutex_}};
   return command_table_.size();
}

#endif
