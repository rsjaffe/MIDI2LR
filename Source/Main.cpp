/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/
/*
  ==============================================================================

  Main.cpp

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

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
#include <exception>
#include <fstream>
#include <memory>
#include <mutex>
#ifdef _WIN32
#include <filesystem> //not available in XCode yet

namespace fs = std::filesystem;
#include "WinDef.h"
#undef NOUSER
#include <Windows.h>
#endif

#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <JuceLibraryCode/JuceHeader.h>
#include "CCoptions.h"
#include "CommandSet.h"
#include "ControlsModel.h"
#include "LR_IPC_In.h"
#include "LR_IPC_Out.h"
#include "MainWindow.h"
#include "MIDIReceiver.h"
#include "MIDISender.h"
#include "Misc.h"
#include "Profile.h"
#include "ProfileManager.h"
#include "PWoptions.h"
#include "SettingsManager.h"
#include "Translate.h"
#include "VersionChecker.h"

namespace {
   constexpr auto kShutDownString{"--LRSHUTDOWN"};
   constexpr auto kSettingsFile{"settings.bin"};
   constexpr auto kSettingsFileX("settings.xml");
   constexpr auto kDefaultsFile{"default.xml"};

   class UpdateCurrentLogger {
    public:
      explicit UpdateCurrentLogger(juce::Logger* new_logger) noexcept
      {
         juce::Logger::setCurrentLogger(new_logger);
      }
   };
} // namespace

class MIDI2LRApplication final : public juce::JUCEApplication {
 public:
   MIDI2LRApplication()
   {
      CCoptions::LinkToControlsModel(&controls_model_);
      PWoptions::LinkToControlsModel(&controls_model_);
      juce::LookAndFeel::setDefaultLookAndFeel(&look_feel_);
      SetAppLanguage(); // set language and load appropriate fonts and files
   }

   // ReSharper disable once CppConstValueFunctionReturnType
   const juce::String getApplicationName() override
   {
      return ProjectInfo::projectName;
   }
   // ReSharper disable once CppConstValueFunctionReturnType
   const juce::String getApplicationVersion() override
   {
      return ProjectInfo::versionString;
   }
   bool moreThanOneInstanceAllowed() noexcept override
   {
      return false;
   }

   //==============================================================================

   void initialise(const juce::String& command_line) override
   {
      try {
         // Called when the application starts.

         // This will be called once to let the application do whatever
         // initialization it needs, create its windows, etc.

         // After the method returns, the normal event - dispatch loop will be
         // run, until the quit() method is called, at which point the shutdown()
         // method will be called to let the application clear up anything it
         // needs to delete.

         // If during the initialise() method, the application decides not to
         // start - up after all, it can just call the quit() method and the event
         // loop won't be run.
         if (command_line != kShutDownString) {
            CerealLoad();
            midi_receiver_->Start();
            midi_sender_->Start();
            lr_ipc_out_->Start();
            lr_ipc_in_->Start();
            main_window_ =
                std::make_unique<MainWindow>(getApplicationName(), command_set_, profile_,
                    profile_manager_, settings_manager_, lr_ipc_out_, midi_receiver_, midi_sender_);
            // Check for latest version
            version_checker_.startThread();
         }
         else {
            // apparently the application is already terminated
            quit();
         }
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   void shutdown() noexcept override
   {
      // Called to allow the application to clear up before exiting.

      // After JUCEApplication::quit() has been called, the event - dispatch
      // loop will terminate, and this method will get called to allow the
      // application to sort itself out.

      // Be careful that nothing happens in this method that might rely on
      // messages being sent, or any kind of window activity, because the
      // message loop is no longer running at this point.
      lr_ipc_in_->PleaseStopThread();
      DefaultProfileSave();
      CerealSave();
      lr_ipc_out_.reset();
      lr_ipc_in_.reset();
      midi_receiver_.reset();
      midi_sender_.reset();
      main_window_.reset(); // (deletes our window)
      juce::Logger::setCurrentLogger(nullptr);
   }

   //==========================================================================
   void systemRequestedQuit() override
   {
      try {
         // This is called when the application is being asked to quit: you can
         // ignore this request and let the application carry on running, or call
         // quit() to allow the application to close.
         static std::once_flag of; // function might be called twice during LR shutdown
         std::call_once(of, [this]() {
            if (profile_.ProfileUnsaved() && main_window_) {
               const auto result = juce::NativeMessageBox::showYesNoBox(
                   juce::AlertWindow::WarningIcon, juce::translate("MIDI2LR profiles"),
                   juce::translate(
                       "Profile changed. Do you want to save it before exiting the program?"));
               if (result)
                  main_window_->SaveProfile();
            }
            quit();
         });
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   void anotherInstanceStarted(const juce::String& command_line) override
   {
      // When another instance of the application is launched while this one is
      // running, this method is invoked, and the commandLine parameter tells
      // you what the other instance's command-line arguments were.
      if (command_line == kShutDownString)
         // shutting down
         systemRequestedQuit();
   }

   [[noreturn]] void unhandledException(
       const std::exception* e, const juce::String& source_filename, int lineNumber) override {
      // If any unhandled exceptions make it through to the message dispatch
      // loop, this callback will be triggered, in case you want to log them or
      // do some other type of error-handling.
      //
      // If the type of exception is derived from the std::exception class, the
      // pointer passed-in will be valid. If the exception is of unknown type,
      // this pointer will be null.
      try {
         if (e)
            rsj::LogAndAlertError("Unhandled exception. " + juce::String(e->what()) + " "
                                  + source_filename + " line " + juce::String(lineNumber));
         else
            rsj::LogAndAlertError(
                "Unhandled exception. " + source_filename + " line " + juce::String(lineNumber));
      }
      catch (...) { // we'll terminate anyway
         std::terminate();
      }
      std::terminate(); // can't go on with the program
   }

   private : void DefaultProfileSave()
   {
      try {
         const auto file_name = rsj::AppDataFilePath(kDefaultsFile);
         const auto profile_file = juce::File(file_name.data());
         profile_.ToXmlFile(profile_file);
         rsj::Log("Default profile saved to " + profile_file.getFullPathName());
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

#pragma warning(suppress : 26447) // all exceptions suppressed by catch blocks
   void CerealSave() const noexcept
   { // scoped so archive gets flushed
      try {
#ifdef _WIN32
         fs::path p{rsj::AppDataFilePath(kSettingsFileX)};
#else
         const auto p = rsj::AppDataFilePath(kSettingsFileX);
#endif
         std::ofstream outfile(p, std::ios::trunc);
         if (outfile.is_open()) {
            // too large to construct on stack
            const auto oarchive = std::make_unique<cereal::XMLOutputArchive>(outfile);
            (*oarchive)(controls_model_);
#ifdef _WIN32
            rsj::Log("ControlsModel archive in Main saved to " + juce::String(p.c_str()));
#else
            rsj::Log("ControlsModel archive in Main saved to " + p);
#endif
         }
         else
            rsj::LogAndAlertError("Unable to save ControlsModel archive in Main.");
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      }
   }

#pragma warning(suppress : 26447) // all exceptions suppressed by catch blocks
   void CerealLoad()
   { // scoped so archive gets flushed
      try {
#ifdef _WIN32
         const fs::path px{rsj::AppDataFilePath(L"settings.xml")};
#else
         const auto px = rsj::AppDataFilePath("settings.xml");
#endif
         std::ifstream in_file(px);
         if (in_file.is_open() && !in_file.eof()) {
            // too large to construct on stack
            const auto iarchive = std::make_unique<cereal::XMLInputArchive>(in_file);
            (*iarchive)(controls_model_);
#ifdef _WIN32
            rsj::Log("ControlsModel archive in Main loaded from " + juce::String(px.c_str()));
#else
            rsj::Log("ControlsModel archive in Main loaded from " + px);
#endif
         }
         else { // see if old-style settings file is available
#ifdef _WIN32
            wchar_t path[MAX_PATH];
            GetModuleFileNameW(nullptr, static_cast<LPWSTR>(path), MAX_PATH);
            fs::path p{path};
            p = p.replace_filename(kSettingsFile);
#else
            const auto p = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
                               .getSiblingFile(kSettingsFile)
                               .getFullPathName()
                               .toStdString();
#endif
            std::ifstream infile(p, std::ios::binary);
            if (infile.is_open() && !infile.eof()) {
               // too large to construct on stack
               const auto iarchive = std::make_unique<cereal::BinaryInputArchive>(infile);
               (*iarchive)(controls_model_);
#ifdef _WIN32
               rsj::Log(
                   "Legacy ControlsModel archive loaded in Main from " + juce::String(p.c_str()));
#else
               rsj::Log("Legacy ControlsModel archive loaded in Main from " + p);
#endif
            }
         }
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      }
   }

   void SetAppLanguage() const
   {
      try {
         const auto lang{command_set_.GetLanguage()};

         // juce (as of July 2018) uses the following font defaults
         // taken from juce_mac_Fonts.mm and juce_wind32_Fonts.cpp
         // sans defaults do not support Asian languages
         //         MacOS            Windows
         // Sans    Lucida Grande    Verdana
         // Serif   Times New Roman  Times New Roman
         // Fixed   Menlo            Lucida Console

         // see https://docs.microsoft.com/en-us/typography/fonts/windows_10_font_list
         // avoiding fonts added in windows 10 to support people using earlier Windows versions
         // see https://docs.microsoft.com/en-us/windows/desktop/uxguide/vis-fonts
         if constexpr (MSWindows) {
            if (lang == "ko") {
               juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName(
                   "Malgun Gothic");
            }
            else if (lang == "zh_tw") {
               juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName(
                   "Microsoft JhengHei UI");
            }
            else if (lang == "zh_cn") {
               juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName(
                   "Microsoft YaHei UI");
            }
            else if (lang == "ja") {
               juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName(
                   "MS UI Gothic");
            }
         }
         else { // PingFang added in 10.11 El Capitan as new Chinese UI fonts
            if (lang == "ko") {
               juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName(
                   "Apple SD Gothic Neo");
            }
            else if (lang == "zh_cn") {
               juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName(
                   "PingFang SC");
            }
            else if (lang == "zh_tw") {
               juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName(
                   "PingFang TC");
            }
            else if (lang == "ja") {
               juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName(
                   "Hiragino Kaku Gothic Pro");
            }
         }
         rsj::Translate(lang);
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(typeid(this).name(), __func__, e);
         throw;
      }
   }

   // create logger first, makes sure that MIDI2LR directory is created for writing by other
   // modules log file created at %AppData%\MIDI2LR (Windows) or ~/Library/Logs/MIDI2LR (OSX)
   // need to own pointer created by createDefaultAppLogger
   std::unique_ptr<juce::FileLogger> logger_{
       juce::FileLogger::createDefaultAppLogger("MIDI2LR", "MIDI2LR.log", "", 32 * 1024)}; //-V112
   // forcing assignment to static early in construction
   [[maybe_unused, no_unique_address]] UpdateCurrentLogger dummy_ { logger_.get() };
   const CommandSet command_set_{};
   ControlsModel controls_model_{};
   Profile profile_{command_set_};
   std::shared_ptr<MidiSender> midi_sender_{std::make_shared<MidiSender>()};
   std::shared_ptr<MidiReceiver> midi_receiver_{std::make_shared<MidiReceiver>()};
   std::shared_ptr<LrIpcOut> lr_ipc_out_{
       std::make_shared<LrIpcOut>(controls_model_, profile_, midi_sender_, *midi_receiver_)};
   ProfileManager profile_manager_{controls_model_, profile_, lr_ipc_out_, *midi_receiver_};
   std::shared_ptr<LrIpcIn> lr_ipc_in_{
       std::make_shared<LrIpcIn>(controls_model_, profile_manager_, profile_, midi_sender_)};
   SettingsManager settings_manager_{profile_manager_, lr_ipc_out_};
   std::unique_ptr<MainWindow> main_window_{nullptr};
   // destroy after window that uses it
   juce::LookAndFeel_V3 look_feel_;
   // initialize this last as it needs window to exist
   VersionChecker version_checker_{settings_manager_};
};

//==============================================================================
// This macro generates the main() routine that launches the application.
#pragma warning(suppress : 26409 26425 28251)
START_JUCE_APPLICATION(MIDI2LRApplication)