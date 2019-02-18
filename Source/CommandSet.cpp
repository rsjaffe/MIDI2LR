#include "CommandSet.h"
#ifdef _WIN32
#include <filesystem> //not available in XCode yet
namespace fs = std::filesystem;
#endif
#include <fstream>
#include <JuceLibraryCode/JuceHeader.h>
#include "Misc.h"

CommandSet::CommandSet() : m_impl(make_impl())
{
   size_t idx = 0;
   for (const auto& bycategory : m_impl.allcommands_)
      for (const auto& cmd_pair : bycategory.second) {
         cmd_by_number_[idx] = cmd_pair.first;
         cmd_idx_[cmd_pair.first] = idx++;
      }
}

CommandSet::~CommandSet() = default; // defined in cpp file in case go to pimpl idiom for Impl

CommandSet::Impl::Impl()
{
   auto& ls{*this}; // will use for running cereal

   language_ = "en";
   allcommands_ = {{"Keyboard Shortcuts for User", {{"Key1", "Key 1"}, {"Key2", "Key 2"}}},
       {"Library filter", {{"Filter_1", "Library filter 1"}, {"Filter_2", "Library filter 2"}}},
       {"General", {{"ShoVwgrid", "Primary Display Grid"}, {"ShoVwloupe", "Primary Display Loupe"},
                       {"ShoVwcompare", "Primary Display Compare"}}}};

   { // scoped so archive gets flushed
      try {
#ifdef _WIN32
         fs::path p{rsj::AppDataFilePath("commandlisttest.xml")};
#else
         const auto p = rsj::AppDataFilePath("commandlisttest.xml");
#endif
         // change all the following to infile once we get communication going
         std::ofstream outfile(p, std::ios::out | std::ios::trunc);
         if (outfile.is_open()) {
            cereal::XMLOutputArchive oarchive(outfile);
            oarchive(ls);
#ifdef _WIN32
            rsj::Log("Cereal controls archive saved to " + juce::String(p.c_str()));
#else
            rsj::Log("Cereal controls archive saved to " + p);
#endif
         }
         else
            rsj::LogAndAlertError("Unable to save control settings to xml file.");
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      }
   }
   // once we load here, will fill up ancillary structures (e.g. command to text maps) here};
}

const CommandSet::Impl& CommandSet::make_impl()
{
   static const Impl singleimpl;
   return singleimpl;
}

size_t CommandSet::GetIndexOfCommand(const std::string& command) const
{
   auto found = cmd_idx_.find(command);
   if (found == cmd_idx_.end()) {
      rsj::LogAndAlertError("Command not found in GetIndexOfCommand: " + command);
      return 0;
   }
   return found->second;
}