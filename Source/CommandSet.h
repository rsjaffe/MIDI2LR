#ifndef MIDI2LR_COMMANDSET_H_INCLUDED
#define MIDI2LR_COMMANDSET_H_INCLUDED
/*
  ==============================================================================

    CommandSet.h

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
#include <string>
#include <unordered_map>
#include <vector>

#include <cereal/access.hpp>
#include <cereal/types/utility.hpp>
#include <JuceLibraryCode/JuceHeader.h>
#include "Misc.h"

class CommandSet {
   using MenuStringT = juce::String;

 public:
   CommandSet();
   [[nodiscard]] size_t CommandTextIndex(const std::string& command) const;
   [[nodiscard]] const auto& CommandAbbrevAt(size_t index) const
   {
      return cmd_by_number_.at(index);
   }

   [[nodiscard]] auto CommandAbbrevSize() const noexcept
   {
      return cmd_by_number_.size();
   }

   [[nodiscard]] const auto& GetLanguage() const noexcept
   {
      return m_impl_.language_;
   }

   [[nodiscard]] const auto& GetMenus() const noexcept
   {
      return menus_;
   }

   [[nodiscard]] const auto& GetMenuEntries() const noexcept
   {
      return menu_entries_;
   }

 private:
   class Impl {
    public:
      Impl();
      ~Impl() = default;
      Impl(const Impl& other) = delete;
      Impl(Impl&& other) = delete;
      Impl& operator=(const Impl& other) = delete;
      Impl& operator=(Impl&& other) = delete;
      template<class Archive> void serialize(Archive& archive, std::uint32_t const version)
      {
         if (version == 1)
            archive(cereal::make_nvp("language", language_),
                cereal::make_nvp("all_commands", allcommands_));
         else
            rsj::LogAndAlertError(
                juce::String("Unsupported archive version for CommandSet. Version is ")
                + std::to_string(version));
      }
      std::string language_;
      std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
          allcommands_;
   };

   friend class cereal::access;
   // see https://github.com/USCiLab/cereal/issues/270
   friend struct cereal::detail::Version<CommandSet::Impl>;
   const Impl& m_impl_;
   [[nodiscard]] const Impl& MakeImpl() const;

   std::unordered_map<std::string, size_t> cmd_idx_{}; // for CommandTextIndex
   std::vector<std::string> cmd_by_number_{}; // use for command_set_.CommandAbbrevAt, .size
   std::vector<MenuStringT> menus_{};         // use for commandmenu
   std::vector<std::vector<MenuStringT>> menu_entries_{}; // use for commandmenu
};
#pragma warning(push)
#pragma warning(disable : 26426 26440 26444)
CEREAL_CLASS_VERSION(CommandSet::Impl, 1)
#pragma warning(pop)
#endif // MIDI2LR_COMMANDSET_H_INCLUDED