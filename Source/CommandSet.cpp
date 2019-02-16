#include "CommandSet.h"
#ifdef _WIN32
#include <filesystem> //not available in XCode yet
namespace fs = std::filesystem;
#endif
#include <fstream>
#include <JuceLibraryCode/JuceHeader.h>
#include "Misc.h"
/*
Proxy::Proxy() : data_(Make_Impl()) {}

std::shared_ptr<Proxy::Impl> Proxy::Make_Impl() const
{ // note that impls_ may be accessed by another thread with non-const member function
  // (assignment) if it is running slightly ahead of this thread with an uninitialized impls_ so
  // need mutex before shared_ptr.lock call. if we use an atomic weak pointer (C++20) can call
  // shared_ptr.lock before mutex, check for success, and only run mutex (and try lock again) after
  // failure
   static std::mutex mtx;
   auto lock = std::lock_guard(mtx);
   std::shared_ptr<Impl> internal_data = impls_.lock();
   if (internal_data)
      return internal_data;
   const auto new_impl(std::make_shared<Impl>());
   impls_ = new_impl;
   return new_impl;
}
Proxy::Impl::Impl()
{
   // implement constructor for Impl here
}
void Proxy::Impl::method1()
{
   // here is the real method1 for the implementation
}

*/

CommandSet::Impl::Impl()
{
   auto& ls{*this}; // will use for running cereal

   language_ = "en";
   categories_ = {"Keyboard Shortcuts for User", "Library filter", "General", "Library", "Develop",
       "Basic", "Tone Curve", "HSL / Color / B&W", "Reset HSL / Color / B&W", "Split Toning",
       "Detail", "Lens Corrections", "Transform", "Effects", "Camera Calibration",
       "Develop Presets", "Keywords", "Local Adjustments", "Crop", "Go to Tool, Module, or Panel",
       "Secondary Display", "Profiles", "Next/Prev Profile"};
   commands_ = {{{"Key1", "Key 1"}, {"Key2", "Key 2"}},
       {{"Filter_1", "Library filter 1"}, {"Filter_2", "Library filter 2"}},
       {{"ShoVwgrid", "Primary Display Grid"}, {"ShoVwloupe", "Primary Display Loupe"},
           {"ShoVwcompare", "Primary Display Compare"}}};
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
