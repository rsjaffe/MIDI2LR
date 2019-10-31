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
#ifndef _WIN32
#include <AvailabilityMacros.h>
#if defined(MAC_OS_X_VERSION_10_15) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_15     \
    && defined(__cpp_lib_filesystem)
#define FILESYSTEM_AVAILABLE_MIDI2LR
#endif
#else
#ifdef __cpp_lib_filesystem
#define FILESYSTEM_AVAILABLE_MIDI2LR
#endif
#endif
#ifdef FILESYSTEM_AVAILABLE_MIDI2LR
#include <filesystem>
namespace fs = std::filesystem;
#endif
#include <fstream>
#include <memory>

#include <cereal/archives/xml.hpp>
// ReSharper disable CppUnusedIncludeDirective
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
// ReSharper restore CppUnusedIncludeDirective

CommandSet::CommandSet() : m_impl_(MakeImpl())
{
   /* manually insert unmapped at first position */
   try {
      cmd_by_number_.emplace_back("Unmapped");
      cmd_idx_["Unmapped"] = 0;
      size_t idx = 1;
      for (const auto& by_category : m_impl_.allcommands_) {
         std::vector<MenuStringT> menu_items_temp{};
         for (const auto& cmd_pair : by_category.second) {
            cmd_by_number_.push_back(cmd_pair.first);
            cmd_idx_[cmd_pair.first] = idx++;
            menu_items_temp.emplace_back(cmd_pair.second);
         }
         menus_.emplace_back(by_category.first);
         menu_entries_.emplace_back(std::move(menu_items_temp));
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), MIDI2LR_FUNC, e);
      throw;
   }
}

CommandSet::Impl::Impl()
{
   try {
#ifdef FILESYSTEM_AVAILABLE_MIDI2LR
      const fs::path p{rsj::AppDataFilePath("MenuTrans.xml")};
#else
      const auto p = rsj::AppDataFilePath("MenuTrans.xml");
#endif
      std::ifstream infile(p);
      if (infile.is_open()) {
#pragma warning(suppress : 26414) /* too large to construct on stack */
         const auto iarchive = std::make_unique<cereal::XMLInputArchive>(infile);
         (*iarchive)(*this);
#ifdef FILESYSTEM_AVAILABLE_MIDI2LR
         rsj::Log("MenuTrans.xml archive loaded from " + juce::String(p.c_str()));
#else
         rsj::Log("MenuTrans.xml archive loaded from " + p);
#endif
      }
      else
         rsj::LogAndAlertError(
             juce::translate("Unable to load MenuTrans.xml."), "Unable to load MenuTrans.xml.");
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), MIDI2LR_FUNC, e);
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
      rsj::ExceptionResponse(typeid(this).name(), MIDI2LR_FUNC, e);
      throw;
   }
}

size_t CommandSet::CommandTextIndex(const std::string& command) const
{
   try {
      const auto found = cmd_idx_.find(command);
      if (found == cmd_idx_.end()) {
         rsj::Log("Command not found in CommandTextIndex: " + command);
         return 0;
      }
      return found->second;
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), MIDI2LR_FUNC, e);
      throw;
   }
}