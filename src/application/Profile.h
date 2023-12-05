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

#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <string>
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

   [[nodiscard]] bool CommandHasAssociatedMessage(const std::string& command) const;
   void FromXml(const juce::XmlElement* root);
   [[nodiscard]] const std::string& GetCommandForMessage(rsj::MidiMessageId message) const;
   [[nodiscard]] rsj::MidiMessageId GetMessageForNumber(size_t num) const;
   [[nodiscard]] std::vector<rsj::MidiMessageId> GetMessagesForCommand(
       const std::string& command) const;
   [[nodiscard]] int GetRowForMessage(rsj::MidiMessageId message) const;
   void InsertOrAssign(const std::string& command, rsj::MidiMessageId message);
   void InsertOrAssign(size_t command, rsj::MidiMessageId message);
   void InsertUnassigned(rsj::MidiMessageId message);
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
   using mm_abbrv_lmnt_t = std::pair<rsj::MidiMessageId, std::string>;
   void InsertOrAssignI(const std::string& command, const rsj::MidiMessageId& message);
   [[nodiscard]] bool MessageExistsInMapI(rsj::MidiMessageId message) const;
   void SortI();

   bool profile_unsaved_ {false};
   const CommandSet& command_set_;
   /* access saved_mm_abbrv_table_ and profile_unsaved_ either under write lock mutex_ or lock
    * saved_table_mtx_ with read lock mutex_. Acquire saved_table_mtx_ before read lock attempt on
    * mutex_ */
   mutable std::mutex saved_table_mtx_;
   mutable std::shared_mutex mutex_;
   std::pair<int, bool> current_sort_ {2, true};
   std::vector<mm_abbrv_lmnt_t> mm_abbrv_table_ {};
   std::vector<mm_abbrv_lmnt_t> saved_mm_abbrv_table_ {};
};

inline bool Profile::CommandHasAssociatedMessage(const std::string& command) const
{
   auto guard {std::shared_lock {mutex_}};
#ifdef __cpp_lib_ranges_contains
   return std::ranges::contains(mm_abbrv_table_, command, &mm_abbrv_lmnt_t::second);
#else
   return std::ranges::any_of(mm_abbrv_table_,
       [&command](const auto& p) { return p.second == command; });
#endif
}

inline const std::string& Profile::GetCommandForMessage(rsj::MidiMessageId message) const
{
   auto guard {std::shared_lock {mutex_}};
   const auto found = std::ranges::find(mm_abbrv_table_, message, &mm_abbrv_lmnt_t::first);
   if (found != mm_abbrv_table_.end()) { return found->second; }
   return CommandSet::kUnassigned;
}

inline rsj::MidiMessageId Profile::GetMessageForNumber(size_t num) const
{
   auto guard {std::shared_lock {mutex_}};
   return mm_abbrv_table_.at(num).first;
}

inline int Profile::GetRowForMessage(rsj::MidiMessageId message) const
{
   auto guard {std::shared_lock {mutex_}};
   return gsl::narrow_cast<int>(std::ranges::find(mm_abbrv_table_, message, &mm_abbrv_lmnt_t::first)
                                - mm_abbrv_table_.begin());
}

inline void Profile::InsertOrAssign(const std::string& command, rsj::MidiMessageId message)
{
   auto guard {std::unique_lock {mutex_}};
   InsertOrAssignI(command, message);
}

inline void Profile::InsertOrAssign(size_t command, rsj::MidiMessageId message)
{
   if (command < command_set_.CommandAbbrevSize()) {
      auto guard {std::unique_lock {mutex_}};
      InsertOrAssignI(command_set_.CommandAbbrevAt(command), message);
   }
}

inline bool Profile::MessageExistsInMap(rsj::MidiMessageId message) const
{
   auto guard {std::shared_lock {mutex_}};
   return MessageExistsInMapI(message);
}

inline bool Profile::MessageExistsInMapI(rsj::MidiMessageId message) const
{
#ifdef __cpp_lib_ranges_contains
   return std::ranges::contains(mm_abbrv_table_, message, &mm_abbrv_lmnt_t::first);
#else
   return std::ranges::any_of(mm_abbrv_table_, [message](auto& p) { return p.first == message; });
#endif
}

inline bool Profile::ProfileUnsaved() const
{ /* technically, should check std::is_permutation, but much quicker to check if saved==current,
     accept occasional false positive for much better program responsiveness */
   auto abbrv_lock {std::scoped_lock {saved_table_mtx_}};
   auto guard {std::shared_lock {mutex_}};
   return profile_unsaved_ && !mm_abbrv_table_.empty() && mm_abbrv_table_ != saved_mm_abbrv_table_;
}

inline size_t Profile::Size() const
{
   auto guard {std::shared_lock {mutex_}};
   return mm_abbrv_table_.size();
}

#endif
