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
         cmd_by_number_.push_back(cmd_pair.first);
         cmd_idx_[cmd_pair.first] = idx++;
      }
}

CommandSet::~CommandSet() = default; // defined in cpp file in case go to pimpl idiom for Impl

CommandSet::Impl::Impl()
{
   auto& ls{*this}; // will use for running cereal
   { // scoped so archive gets flushed
      try {
#ifdef _WIN32
         fs::path p{rsj::AppDataFilePath("MenuTrans.xml")};
#else
         const auto p = rsj::AppDataFilePath("MenuTrans.xml");
#endif
         std::ifstream infile(p);
         if (infile.is_open()) {
            cereal::XMLInputArchive iarchive(infile);
            iarchive(ls);
#ifdef _WIN32
            rsj::Log("Cereal controls archive loaded from " + juce::String(p.c_str()));
#else
            rsj::Log("Cereal controls archive loaded from " + p);
#endif
         }
         else
            rsj::LogAndAlertError("Unable to load control settings from xml file. Unable to open file");
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