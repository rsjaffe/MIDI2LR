#ifndef MIDI2LR_PROFILE_H_INCLUDED
#define MIDI2LR_PROFILE_H_INCLUDED
/*
  ==============================================================================

    Profile.h

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
#include <map>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <gsl/gsl>
#include <JuceLibraryCode/JuceHeader.h>
#include "CommandSet.h"
#include "MidiUtilities.h"

// All methods with _i at end don't include a mutex and are for internal use only. Methods
// without _i do have mutex and could be called by another class
class Profile {
 public:
   Profile(const CommandSet& command_set) : command_set_{command_set} {}
   void AddCommandForMessage(size_t command, const rsj::MidiMessageId& message);
   void AddRowMapped(const std::string& command, const rsj::MidiMessageId& message);
   void AddRowUnmapped(const rsj::MidiMessageId& message);
   [[nodiscard]] bool CommandHasAssociatedMessage(const std::string& command) const;
   void FromXml(const juce::XmlElement* root);
   [[nodiscard]] const std::string& GetCommandForMessage(const rsj::MidiMessageId& message) const;
   [[nodiscard]] const rsj::MidiMessageId& GetMessageForNumber(size_t num) const;
   [[nodiscard]] std::vector<rsj::MidiMessageId> GetMessagesForCommand(
       const std::string& command) const;
   [[nodiscard]] int GetRowForMessage(const rsj::MidiMessageId& message) const;
   [[nodiscard]] bool MessageExistsInMap(const rsj::MidiMessageId& message) const;
   [[nodiscard]] bool ProfileUnsaved() const;
   void RemoveAllRows();
   void RemoveMessage(const rsj::MidiMessageId& message);
   void RemoveRow(size_t row);
   void Resort(std::pair<int, bool> new_order);
   [[nodiscard]] size_t Size() const;
   void ToXmlFile(const juce::File& file);

 private:
   void AddCommandForMessage_i(size_t command, const rsj::MidiMessageId& message);
   const std::string& GetCommandForMessage_i(const rsj::MidiMessageId& message) const;
   const rsj::MidiMessageId& GetMessageForNumber_i(size_t num) const;
   bool MessageExistsInMap_i(const rsj::MidiMessageId& message) const;
   void Sort_i();

   bool profile_unsaved_{false};
   const CommandSet& command_set_;
   mutable std::shared_mutex mutex_;
   std::multimap<std::string, rsj::MidiMessageId> command_string_map_;
   std::pair<int, bool> current_sort_{2, true};
   std::unordered_map<rsj::MidiMessageId, std::string> message_map_;
   std::vector<rsj::MidiMessageId> command_table_;
};

inline void Profile::AddCommandForMessage(size_t command, const rsj::MidiMessageId& message)
{
   try {
      auto guard = std::unique_lock{mutex_};
      AddCommandForMessage_i(command, message);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

inline bool Profile::CommandHasAssociatedMessage(const std::string& command) const
{
   try {
      auto guard = std::shared_lock{mutex_};
      return command_string_map_.find(command) != command_string_map_.end();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

inline const std::string& Profile::GetCommandForMessage(const rsj::MidiMessageId& message) const
{
   try {
      auto guard = std::shared_lock{mutex_};
      return GetCommandForMessage_i(message);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

inline const std::string& Profile::GetCommandForMessage_i(const rsj::MidiMessageId& message) const
{
   try {
      return message_map_.at(message);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

inline const rsj::MidiMessageId& Profile::GetMessageForNumber(size_t num) const
{
   try {
      auto guard = std::shared_lock{mutex_};
      return GetMessageForNumber_i(num);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

inline const rsj::MidiMessageId& Profile::GetMessageForNumber_i(size_t num) const
{
   try {
      return command_table_.at(num);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

inline int Profile::GetRowForMessage(const rsj::MidiMessageId& message) const
{
   try {
      auto guard = std::shared_lock{mutex_};
      return gsl::narrow_cast<int>(std::find(command_table_.begin(), command_table_.end(), message)
                                   - command_table_.begin());
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

inline bool Profile::MessageExistsInMap(const rsj::MidiMessageId& message) const
{
   try {
      auto guard = std::shared_lock{mutex_};
      return MessageExistsInMap_i(message);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

inline bool Profile::MessageExistsInMap_i(const rsj::MidiMessageId& message) const
{
   try {
      return message_map_.find(message) != message_map_.end();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

inline bool Profile::ProfileUnsaved() const
{
   try {
      auto guard = std::shared_lock{mutex_};
      return profile_unsaved_ && !command_table_.empty();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

inline size_t Profile::Size() const
{
   try {
      auto guard = std::shared_lock{mutex_};
      return command_table_.size();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

#endif // MIDI2LR_PROFILE_H_INCLUDED
