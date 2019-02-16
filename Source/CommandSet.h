#pragma once
#ifndef MIDI2LR_SCRIMPL_H_INCLUDED
#include <cereal/access.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/vector.hpp>
#include <string>
#include <vector>
/*
class Proxy {
 public:
   Proxy();
   void method1() // inline dispatch to hidden single class
   {
      data_->method1();
   }

 private:
   class Impl {
    public:
      void method1();
      Impl();
      ~Impl() = default;
   };
   std::shared_ptr<Impl> Make_Impl() const;
   const std::shared_ptr<Impl> data_{};
   inline static std::weak_ptr<Impl> impls_{};
};
*/

class CommandSet {
 public:
   CommandSet() : m_impl(make_impl()) {}

   size_t GetIndexOfCommand(const std::string& command) const;
   std::string GetLanguage() const
   {
      return m_impl.language_;
   }

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
            archive(cereal::make_nvp("commands", commands_),
                cereal::make_nvp("language", language_),
                cereal::make_nvp("categories", categories_),
                cereal::make_nvp("all_commands", allcommands_));
      }
      std::vector<std::vector<std::pair<std::string, std::string>>> commands_;
      std::string language_;
      std::vector<std::string> categories_;
      std::vector<std::pair<std::string, std::vector<std::pair<std::string, std::string>>>>
          allcommands_;

    private:
   };

 private:
   Impl& m_impl;
   Impl& make_impl()
   {
      static Impl singleimpl;
      return singleimpl;
   }
};
#pragma warning(push)
#pragma warning(disable : 26440 26426 26444)
CEREAL_CLASS_VERSION(CommandSet::Impl, 1)
#pragma warning(pop)
#endif // MIDI2LR_SCRIMPL_H_INCLUDED