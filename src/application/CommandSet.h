#ifndef MIDI2LR_COMMANDSET_H_INCLUDED
#define MIDI2LR_COMMANDSET_H_INCLUDED
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
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cereal/access.hpp>
#include <cereal/types/utility.hpp>
#include <fmt/format.h>

#include <juce_core/juce_core.h>

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

   [[nodiscard]] auto CommandAbbrevSize() const noexcept { return cmd_by_number_.size(); }

   [[nodiscard]] auto CommandLabelAt(size_t index) const { return cmd_label_by_number_.at(index); }

   [[nodiscard]] const auto& GetLanguage() const noexcept { return m_impl_.language_; }

   [[nodiscard]] const auto& GetMenus() const noexcept { return menus_; }

   [[nodiscard]] const auto& GetMenuEntries() const noexcept { return menu_entries_; }

   [[nodiscard]] const auto& GetRepeats() const noexcept { return m_impl_.repeat_messages_; }

   [[nodiscard]] const auto& GetWraps() const noexcept { return m_impl_.wraps_; }

   [[nodiscard]] static const auto& UnassignedTranslated()
   {
      static const auto unassigned {juce::translate("Unassigned").toStdString()};
      return unassigned;
   }

   inline static const std::string kUnassigned {"Unassigned"};

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
         try {
            if (std::cmp_equal(version, 2)) {
               archive(cereal::make_nvp("language", language_),
                   cereal::make_nvp("all_commands", allcommands_),
                   cereal::make_nvp("repeats", repeat_messages_),
                   cereal::make_nvp("wraps", wraps_));
            }
            else {
               constexpr auto msg {
                   "The file, 'MenuTrans.xml', is marked as a version not supported by the current "
                   "version of MIDI2LR, and won't be loaded. File version: {}."};
               rsj::LogAndAlertError(fmt::format(juce::translate(msg).toStdString(), version),
                   fmt::format(msg, version));
            }
         }
         catch (const std::exception& e) {
            rsj::ExceptionResponse(e);
            throw;
         }
      }

      std::string language_;
      std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
          allcommands_;
      std::unordered_map<std::string, std::pair<std::string, std::string>> repeat_messages_;
      std::vector<std::string> wraps_;
   };

   friend class cereal::access;
   /* see https://github.com/USCiLab/cereal/issues/270 */
   friend struct cereal::detail::Version<CommandSet::Impl>;
   [[nodiscard]] const Impl& MakeImpl() const;
   const Impl& m_impl_;
   std::unordered_map<std::string, size_t> cmd_idx_ {}; /* for CommandTextIndex */
   std::vector<MenuStringT> menus_ {};                  /* use for commandmenu */
   std::vector<std::string> cmd_by_number_ {}; /* use for command_set_.CommandAbbrevAt, .size */
   std::vector<std::string> cmd_label_by_number_ {};
   std::vector<std::vector<MenuStringT>> menu_entries_ {}; /* use for commandmenu */
};

#pragma warning(push)
#pragma warning(disable : 26426 26440 26444)
CEREAL_CLASS_VERSION(CommandSet::Impl, 2) //-V837
#pragma warning(pop)
#endif
