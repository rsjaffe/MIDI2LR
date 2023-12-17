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
#include "CommandSet.h"

#include <exception>
#include <filesystem>
#include <fstream>
#include <memory>
#include <utility>

#include <cereal/archives/xml.hpp>
#include <cereal/types/string.hpp> /*ReSharper false alarm*/
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp> /*ReSharper false alarm*/

#include "Translate.h"

namespace fs = std::filesystem;

CommandSet::CommandSet() : m_impl_(MakeImpl())
{
   /* manually insert unmapped at first position */
   try {
      rsj::Translate(m_impl_.language_); /* so UnassignedTranslated translated properly */
      cmd_by_number_.push_back(kUnassigned);
      cmd_label_by_number_.push_back(UnassignedTranslated());
      cmd_idx_[kUnassigned] = 0;
      size_t idx {1};
      for (const auto& [cmd_group, cmd_abbrev_label] : m_impl_.allcommands_) {
         std::vector<MenuStringT> menu_items_temp {};
         const auto group_colon {cmd_group + " : "}; /* concatenation optimization */
         for (const auto& [cmd_abbrev, cmd_label] : cmd_abbrev_label) {
            cmd_by_number_.push_back(cmd_abbrev);
            cmd_label_by_number_.push_back(group_colon + cmd_label);
            cmd_idx_[cmd_abbrev] = idx++;
            menu_items_temp.push_back(cmd_label);
         }
         menus_.push_back(cmd_group);
         menu_entries_.push_back(std::move(menu_items_temp));
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

CommandSet::Impl::Impl()
{
   try {
      const fs::path p {rsj::AppDataFilePath(MIDI2LR_UC_LITERAL("MenuTrans.xml"))};
      if (std::ifstream infile {p}; infile.is_open()) {
#pragma warning(suppress : 26414) /* too large to construct on stack */
         const auto iarchive {std::make_unique<cereal::XMLInputArchive>(infile)};
         (*iarchive)(*this);
         rsj::Log(fmt::format(FMT_STRING("MenuTrans.xml archive loaded from {}."), p.string()));
      }
      else {
         rsj::LogAndAlertError(juce::translate("Unable to load MenuTrans.xml."),
             "Unable to load MenuTrans.xml.");
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

const CommandSet::Impl& CommandSet::MakeImpl() const
{
   try {
      static const Impl kImpl;
      return kImpl;
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

size_t CommandSet::CommandTextIndex(const std::string& command) const
{
   try {
      using namespace std::string_literals;
      if (const auto found {cmd_idx_.find(command)}; found != cmd_idx_.end()) {
         return found->second;
      }
      if (command != "Unmapped"s) { /*Old version of Unassigned*/
         rsj::Log(fmt::format(FMT_STRING("Command not found in CommandTextIndex: {}."), command));
      }
      return 0;
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}