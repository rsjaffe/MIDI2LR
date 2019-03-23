/*
  ==============================================================================

    CommandSet.cpp

This file is part of MIDI2LR. Copyright 2019 by Rory Jaffe.

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
#include "CommandSet.h"

#include <exception>
#ifdef _WIN32
#include <filesystem> //not available in XCode yet
namespace fs = std::filesystem;
#endif
#include <fstream>

#include <cereal/archives/xml.hpp>
// ReSharper disable CppUnusedIncludeDirective
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
// ReSharper restore CppUnusedIncludeDirective

CommandSet::CommandSet() : m_impl_(MakeImpl())
{ // manually insert unmapped at first position
   try {
      cmd_by_number_.emplace_back("Unmapped");
      cmd_idx_["Unmapped"] = 0;
      size_t idx = 1;
      for (const auto& bycategory : m_impl_.allcommands_) {
         std::vector<MenuStringT> menu_items_temp{};
         for (const auto& cmd_pair : bycategory.second) {
            cmd_by_number_.push_back(cmd_pair.first);
            cmd_idx_[cmd_pair.first] = idx++;
            menu_items_temp.emplace_back(cmd_pair.second);
         }
         menus_.emplace_back(bycategory.first);
         menu_entries_.emplace_back(std::move(menu_items_temp));
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

CommandSet::Impl::Impl()
{
   try {
#ifdef _WIN32
      fs::path p{rsj::AppDataFilePath(L"MenuTrans.xml")};
#else
      const auto p = rsj::AppDataFilePath("MenuTrans.xml");
#endif
      std::ifstream infile(p);
      if (infile.is_open()) {
         cereal::XMLInputArchive iarchive(infile);
         iarchive(*this);
#ifdef _WIN32
         rsj::Log("MenuTrans.xml archive loaded from " + juce::String(p.c_str()));
#else
         rsj::Log("MenuTrans.xml archive loaded from " + p);
#endif
      }
      else
         rsj::LogAndAlertError("Unable to load MenuTrans.xml. Unable to open file.");
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
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
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
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
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}