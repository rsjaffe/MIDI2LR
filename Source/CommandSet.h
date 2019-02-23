#pragma once
#ifndef MIDI2LR_SCRIMPL_H_INCLUDED
#include <cereal/access.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#include <JuceLibraryCode/JuceHeader.h>
#include "Misc.h"

class CommandSet {
   using MenuStringT = juce::String;

 public:
   CommandSet();
   ~CommandSet();
   size_t CommandTextIndex(const MenuStringT& command) const;
   const auto& CommandAbbrevAt(size_t index) const
   {
      return cmd_by_number_.at(index);
   }

   auto CommandAbbrevSize() const noexcept
   {
      return cmd_by_number_.size();
   }

   auto GetLanguage() const noexcept
   {
      return m_impl.language_;
   }

   const auto& GetMenus() const noexcept
   {
      return menus_;
   }

   const auto& GetMenuEntries() const noexcept
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
                "Unsupported archive version for CommandSet. Version is " + version);
      }
      std::string language_;
      std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
          allcommands_;

    private:
   };

   friend class cereal::access;
   // see https://github.com/USCiLab/cereal/issues/270
   friend struct cereal::detail::Version<CommandSet::Impl>;
   const Impl& m_impl;
   const Impl& make_impl();

   std::unordered_map<MenuStringT, size_t> cmd_idx_{}; // for CommandTextIndex
   std::vector<std::string> cmd_by_number_{}; // use for command_set_.CommandAbbrevAt, .size
   std::vector<MenuStringT> menus_{};         // use for commandmenu
   std::vector<std::vector<MenuStringT>> menu_entries_{}; // use for commandmenu
};
#pragma warning(push)
#pragma warning(disable : 26440 26426 26444)
CEREAL_CLASS_VERSION(CommandSet::Impl, 1)
#pragma warning(pop)
#endif // MIDI2LR_SCRIMPL_H_INCLUDED