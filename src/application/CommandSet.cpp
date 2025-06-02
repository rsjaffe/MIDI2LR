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
#include <numeric>
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
      const auto command_count {
          std::accumulate(m_impl_.allcommands_.begin(), m_impl_.allcommands_.end(), size_t {1},
              [](size_t sum, const auto& pair) { return sum + pair.second.size(); })};
      cmd_by_number_.reserve(command_count);
      cmd_label_by_number_.reserve(command_count);
      cmd_by_number_.push_back(kUnassigned);
      cmd_label_by_number_.push_back(UnassignedTranslated());
      cmd_idx_[kUnassigned] = 0;
      size_t idx {1};
      for (const auto& [cmd_group, cmd_abbrev_label] : m_impl_.allcommands_) {
         std::vector<MenuStringT> menu_items_temp {};
         menu_items_temp.reserve(cmd_abbrev_label.size());
         const auto group_colon {cmd_group + " : "}; /* concatenation optimization */
         for (const auto& [cmd_abbrev, cmd_label] : cmd_abbrev_label) {
            cmd_by_number_.push_back(cmd_abbrev);
            cmd_label_by_number_.push_back(group_colon + cmd_label);
            cmd_idx_.emplace(cmd_abbrev, idx++);
            menu_items_temp.emplace_back(cmd_label);
         }
         menus_.emplace_back(cmd_group);
         menu_entries_.push_back(std::move(menu_items_temp));
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
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
         rsj::Log(fmt::format("MenuTrans.xml archive loaded from {}.", p.string()),
             std::source_location::current());
      }
      else {
         rsj::LogAndAlertError(juce::translate("Unable to load MenuTrans.xml."),
             "Unable to load MenuTrans.xml.", std::source_location::current());
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
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
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}