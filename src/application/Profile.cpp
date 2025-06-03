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
#include "Profile.h"

#include <exception>
#include <memory>
#include <ranges>

#include "Misc.h"

namespace {
   const std::map<std::string, std::string> replace_me {
       {            "CropAngle",      "straightenAngle"},
       {       "ResetCropAngle", "ResetstraightenAngle"},
       {     "local_Whites2012",         "local_Whites"},
       {     "local_Blacks2012",         "local_Blacks"},
       {"Resetlocal_Whites2012",    "Resetlocal_Whites"},
       {"Resetlocal_Blacks2012",    "Resetlocal_Blacks"},
       {             "Unmapped",           "Unassigned"}
   };
} // namespace

void Profile::FromXml(const juce::XmlElement* root)
{
   /* external use only, but will either use external versions of Profile calls to lock individual
    * accesses or manually lock any internal calls instead of using mutex for entire method */
   try {
      if (!root || root->getTagName().compare("settings") != 0) { return; }
      RemoveAllRows();
      for (const gsl::not_null<juce::XmlElement*> setting : root->getChildIterator()) {
         auto command {setting->getStringAttribute("command_string").toStdString()};
         if (const auto it = replace_me.find(command); it != replace_me.end()) {
            command = it->second;
         }
         if (setting->hasAttribute("controller")) {
            const rsj::MidiMessageId message {setting->getIntAttribute("channel"),
                setting->getIntAttribute("controller"), rsj::MessageType::kCc};
            InsertOrAssign(command, message);
         }
         else if (setting->hasAttribute("note")) {
            const rsj::MidiMessageId note {setting->getIntAttribute("channel"),
                setting->getIntAttribute("note"), rsj::MessageType::kNoteOn};
            InsertOrAssign(command, note);
         }
         else if (setting->hasAttribute("pitchbend")) {
            const rsj::MidiMessageId pb {
                setting->getIntAttribute("channel"), 0, rsj::MessageType::kPw};
            InsertOrAssign(command, pb);
         }
         else { /* no action needed */
            continue;
         }
      }
      auto guard {std::unique_lock {mutex_}};
      SortI();
      saved_mm_abbrv_table_ = mm_abbrv_table_;
      profile_unsaved_ = false;
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

std::vector<rsj::MidiMessageId> Profile::GetMessagesForCommand(const std::string& command) const
{
   try {
      auto guard {std::shared_lock {mutex_}};
      const auto filt {[&command](const auto& p) { return p.second == command; }};
#ifdef __cpp_lib_ranges_to_container
      auto mm {mm_abbrv_table_ | std::views::filter(filt)
               | std::views::elements<0> | std::ranges::to<std::vector>()};
#else
      std::vector<rsj::MidiMessageId> mm;
      std::ranges::copy(mm_abbrv_table_ | std::views::filter(filt) | std::views::elements<0>,
          std::back_inserter(mm));
#endif
      return mm;
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void Profile::InsertOrAssignI(const std::string& command, const rsj::MidiMessageId& message)
{
   try {
      const auto found {std::ranges::find(mm_abbrv_table_, message, &mm_abbrv_lmnt_t::first)};
      if (found != mm_abbrv_table_.end()) { found->second = command; }
      else {
         mm_abbrv_table_.emplace_back(message, command);
      }
      SortI();
      profile_unsaved_ = true;
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void Profile::InsertUnassigned(rsj::MidiMessageId message)
{
   try {
      auto guard {std::unique_lock {mutex_}};
      if (!MessageExistsInMapI(message)) {
         mm_abbrv_table_.emplace_back(message, CommandSet::kUnassigned);
         SortI();
         profile_unsaved_ = true;
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void Profile::RemoveAllRows()
{
   try {
      auto guard {std::unique_lock {mutex_}};
      mm_abbrv_table_.clear();
      /*avoid repeated allocations when building*/
      mm_abbrv_table_.reserve(128);
      /* no reason for profile_unsaved_ here. nothing to save */
      profile_unsaved_ = false;
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void Profile::RemoveMessage(rsj::MidiMessageId message)
{
   try {
      auto guard {std::unique_lock {mutex_}};
      const auto found {std::ranges::find(mm_abbrv_table_, message, &mm_abbrv_lmnt_t::first)};
      if (found != mm_abbrv_table_.end()) [[likely]] {
         mm_abbrv_table_.erase(found);
         profile_unsaved_ = true;
      }
      else {
         rsj::Log(fmt::format("Error in Profile::RemoveMessage. Message not found. Message is: "
                              "channel {} control number {} type {}.",
                      message.channel, message.control_number, message.msg_id_type),
             std::source_location::current());
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void Profile::RemoveRow(const size_t row)
{
   try {
      auto guard {std::unique_lock {mutex_}};
      if (row >= mm_abbrv_table_.size()) [[unlikely]] {
         rsj::Log(fmt::format("Error in Profile::RemoveRow. Row {} out of range.", row),
             std::source_location::current());
         return;
      }
      mm_abbrv_table_.erase(mm_abbrv_table_.begin() + gsl::narrow_cast<std::ptrdiff_t>(row));
      profile_unsaved_ = true;
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void Profile::RemoveUnassignedMessages()
{
   try {
      auto guard {std::unique_lock {mutex_}};
      if (std::erase_if(mm_abbrv_table_,
              [](const auto& p) { return p.second == CommandSet::kUnassigned; })) {
         profile_unsaved_ = true;
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void Profile::Resort(const std::pair<int, bool> new_order)
{
   try {
      auto guard {std::unique_lock {mutex_}};
      current_sort_ = new_order;
      SortI();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void Profile::SortI()
{
   try {
      const auto CommandNumber {
          [this](const auto& a) { return command_set_.CommandTextIndex(a.second); }};
      if (current_sort_.first == 1) {
         if (current_sort_.second) { std::ranges::sort(mm_abbrv_table_); }
         else {
            std::ranges::sort(mm_abbrv_table_ | std::views::reverse);
         }
      }
      else if (current_sort_.second) {
         std::ranges::stable_sort(mm_abbrv_table_, {}, CommandNumber);
      }
      else {
         std::ranges::stable_sort(mm_abbrv_table_ | std::views::reverse, {}, CommandNumber);
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void Profile::ToXmlFile(const juce::File& file)
{
   try { /* except for saved_mm_abbrv_table_ and profile_unsaved_, doesn't alter anything, so to
            allow for better responsiveness in slow serialization and file write, use shared_lock +
            mtx solely guarding the two members that are overwritten */
      auto abbrv_lock {std::scoped_lock {saved_table_mtx_}};
      auto guard {std::shared_lock {mutex_}};
      /* don't bother if map is empty */
      if (!mm_abbrv_table_.empty()) {
         /* save the contents of the command map to an xml file */
         juce::XmlElement root {"settings"};
         for (const auto& [msg_id, cmd_str] : mm_abbrv_table_) {
            auto setting {std::make_unique<juce::XmlElement>("setting")};
            setting->setAttribute("channel", msg_id.channel);
            switch (msg_id.msg_id_type) {
            case rsj::MessageType::kNoteOn:
               setting->setAttribute("note", msg_id.control_number);
               break;
            case rsj::MessageType::kCc:
               setting->setAttribute("controller", msg_id.control_number);
               break;
            case rsj::MessageType::kPw:
               setting->setAttribute("pitchbend", 0);
               break;
            case rsj::MessageType::kChanPressure:
            case rsj::MessageType::kKeyPressure:
            case rsj::MessageType::kNoteOff:
            case rsj::MessageType::kPgmChange:
            case rsj::MessageType::kSystem:
               /* can't handle other types */
               continue;
            }
            setting->setAttribute("command_string", cmd_str);
            root.prependChildElement(setting.release());
         }
         if (!root.writeTo(file)) {
            /* Give feedback if file-save doesn't work */
            const auto& p {file.getFullPathName()};
            rsj::LogAndAlertError(juce::translate("Unable to save file. Choose a different "
                                                  "location and try again.")
                                      + ' ' + p,
                "Unable to save file. Choose a different location and try again. " + p,
                std::source_location::current());
         }
         else {
            /*could use shared_mutex above if it were upgradable to unique here*/
            saved_mm_abbrv_table_ = mm_abbrv_table_;
            profile_unsaved_ = false;
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}