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

#include <algorithm>
#include <exception>
#include <fstream>
#include <memory>
#include <mutex>
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

#include <cereal/archives/xml.hpp>
#include <fmt/format.h>

#include <JuceHeader.h>

#include "CCoptions.h"
#include "CommandSet.h"
#include "ControlsModel.h"
#include "Devices.h"
#include "LR_IPC_In.h"
#include "LR_IPC_Out.h"
#include "MIDIReceiver.h"
#include "MIDISender.h"
#include "MainWindow.h"
#include "Misc.h"
#include "PWoptions.h"
#include "Profile.h"
#include "ProfileManager.h"
#include "SettingsManager.h"
#include "VersionChecker.h"
#ifdef _WIN32
#include <array>

#include <wil/result.h> /* including too early causes conflicts with other windows includes */
#endif

namespace {
   class LookAndFeelMIDI2LR final : public juce::LookAndFeel_V3 {
    public:
      juce::Font getTextButtonFont(juce::TextButton&, const int button_height) override
      {
         return juce::Font(std::min(16.0f, static_cast<float>(button_height) * 0.7f));
      }
   };

   constexpr auto kShutDownString {"--LRSHUTDOWN"};
   constexpr auto kSettingsFileX {"settings.xml"};
   constexpr auto kDefaultsFile {"default.xml"};

   class UpdateCurrentLogger {
    public:
      explicit UpdateCurrentLogger(juce::Logger* new_logger) noexcept
      {
         juce::Logger::setCurrentLogger(new_logger);
#ifdef _WIN32
         wil::SetResultLoggingCallback([](wil::FailureInfo const& failure) noexcept {
            std::array<wchar_t, 2048> debug_string {};
            wil::GetFailureLogString(debug_string.data(), debug_string.size(), failure);
            rsj::Log(debug_string.data());
         });
#endif
      }
   };

   [[noreturn]] void OnTerminate() noexcept
   {
      static std::mutex terminate_mutex;
      try {
         auto lock {std::scoped_lock(terminate_mutex)};
         if (const auto exc {std::current_exception()}) {
            /* we have an exception */
            try {
               /* throw to recognize the type */
               std::rethrow_exception(exc);
            }
            catch (const std::exception& e) {
               rsj::Log(fmt::format(FMT_STRING("Terminate called, exception {}."), e.what()));
            }
            catch (...) {
               rsj::Log("Terminate called, unknown exception type.");
            }
         }
         else
            rsj::Log("Terminate called, no exception available.");
      }
      catch (...) { //-V565
      }
      std::_Exit(EXIT_FAILURE);
   }
/* global to install prior to program start order of initialization unimportant for this global
 * object */
#pragma warning(suppress : 26426)
   [[maybe_unused]] const auto kInstalled {std::set_terminate(&OnTerminate)};
} // namespace

class MIDI2LRApplication final : public juce::JUCEApplication {
 public:
   // ReSharper disable once CppConstValueFunctionReturnType
   const juce::String getApplicationName() override // NOLINT(readability-const-return-type)
   {
      return ProjectInfo::projectName;
   }

   // ReSharper disable once CppConstValueFunctionReturnType
   const juce::String getApplicationVersion() override // NOLINT(readability-const-return-type)
   {
      return ProjectInfo::versionString;
   }

   bool moreThanOneInstanceAllowed() noexcept override
   {
      return false;
   }

   void initialise(const juce::String& command_line) override
   {
      try {
         /*Called when the application starts. This will be called once to let the application do
          * whatever initialization it needs, create its windows, etc. After the method returns, the
          * normal event - dispatch loop will be run, until the quit() method is called, at which
          * point the shutdown() method will be called to let the application clear up anything it
          * needs to delete. If during the initialise() method, the application decides not to
          * start-up after all, it can just call the quit() method and the event loop won't be
          * run. */
         if (command_line != kShutDownString) {
            _mm_setcsr(_mm_getcsr() | 0x8040);
            CCoptions::LinkToControlsModel(&controls_model_);
            PWoptions::LinkToControlsModel(&controls_model_);
            juce::LookAndFeel::setDefaultLookAndFeel(&look_feel_);
            /* set language and load appropriate fonts and files */
            SetAppFont();
            LoadControlsModel();
            /* need to start main window before ipc so it's already registered its callbacks and can
             * receive messages */
            main_window_ =
                std::make_unique<MainWindow>(getApplicationName(), command_set_, profile_,
                    profile_manager_, settings_manager_, lr_ipc_out_, midi_receiver_, midi_sender_);
            midi_receiver_.Start();
            midi_sender_.Start();
            lr_ipc_out_.Start();
            lr_ipc_in_.Start();
            /* Check for latest version */
            version_checker_.Start();
         }
         else {
            quit();
         }
      }
      catch (const std::exception& e) {
         MIDI2LR_E_RESPONSE;
         throw;
      }
   }

   void shutdown() override
   {
      /*Called to allow the application to clear up before exiting. After JUCEApplication::quit()
       * has been called, the event-dispatch loop will terminate, and this method will get called
       * to allow the application to sort itself out. Be careful that nothing happens in this method
       * that might rely on messages being sent, or any kind of window activity, because the message
       * loop is no longer running at this point. */

      /*Primary goals: 1) remove callbacks in LR_IPC_Out and MIDIReceiver before the callee is
       * destroyed, 2) stop additional threads in VersionChecker, LR_IPC_In, LR_IPC_Out and
       * MIDIReceiver. Add to this list if new threads or callback lists are developed in this
       * app. */
      midi_receiver_.Stop();
      lr_ipc_in_.Stop();
      lr_ipc_out_.Stop();
      version_checker_.Stop();
      DefaultProfileSave();
      SaveControlsModel();
      settings_manager_.SetDefaultProfile(main_window_->GetProfileName());
      /* (delete our window) */
      main_window_.reset();
      juce::Logger::setCurrentLogger(nullptr);
   }

   void systemRequestedQuit() override
   {
      /*This is called when the application is being asked to quit: you can ignore this request
       * and let the application carry on running, or call quit() to allow the application to
       * close. */
      try {
         static std::once_flag of; /* function might be called twice during LR shutdown */
         std::call_once(of, [this] {
            if (profile_.ProfileUnsaved() && main_window_) {
               const juce::MessageManagerLock mmLock; /* this may be unnecessary */
               const auto result {juce::NativeMessageBox::showYesNoBox(
                   juce::AlertWindow::WarningIcon, juce::translate("MIDI2LR profiles"),
                   juce::translate("Profile changed. Do you want to save your changes? If you "
                                   "continue without saving, your changes will be lost."))};
               if (result)
                  main_window_->SaveProfile();
            }
            quit();
         });
      }
      catch (const std::exception& e) {
         MIDI2LR_E_RESPONSE;
         throw;
      }
   }

   void anotherInstanceStarted(const juce::String& command_line) override
   {
      if (command_line == kShutDownString)
         systemRequestedQuit();
   }

   [[noreturn]] void unhandledException(
       const std::exception* e, const juce::String& source_filename, int lineNumber) override
   {
      /* If any unhandled exceptions make it through to the message dispatch loop, this callback
       * will be triggered, in case you want to log them or do some other type of error-handling. If
       * the type of exception is derived from the std::exception class, the pointer passed-in will
       * be valid. If the exception is of unknown type, this pointer will be null. */
      try {
         if (e) {
            constexpr auto msge {"Unhandled exception {}, {} line {}. Total uncaught {}."};
            const auto msgt {juce::translate("unhandled exception").toStdString()
                             + " {}, {} line {}. Total uncaught {}."};
            rsj::LogAndAlertError(fmt::format(msgt, e->what(), source_filename.toStdString(),
                                      lineNumber, std::uncaught_exceptions()),
                fmt::format(msge, e->what(), source_filename.toStdString(), lineNumber,
                    std::uncaught_exceptions()));
         }
         else {
            constexpr auto msge {"Unhandled exception {} line {}. Total uncaught {}."};
            const auto msgt {juce::translate("unhandled exception").toStdString()
                             + " {} line {}. Total uncaught {}."};
            rsj::LogAndAlertError(fmt::format(msgt, source_filename.toStdString(), lineNumber,
                                      std::uncaught_exceptions()),
                fmt::format(
                    msge, source_filename.toStdString(), lineNumber, std::uncaught_exceptions()));
         }
      }
      catch (...) {
         /* we'll terminate anyway */
         std::terminate();
      }
      /* can't go on with the program */
      std::terminate();
   }

 private:
   void DefaultProfileSave()
   {
      try {
         const auto file_name {rsj::AppDataFilePath(kDefaultsFile)};
         const auto profile_file {juce::File(file_name.data())};
         profile_.ToXmlFile(profile_file);
         rsj::Log(fmt::format(FMT_STRING("Default profile saved to {}."),
             profile_file.getFullPathName().toStdString()));
      }
      catch (const std::exception& e) {
         MIDI2LR_E_RESPONSE;
         throw;
      }
   }

   void SaveControlsModel() const
   {
      try {
#ifdef FILESYSTEM_AVAILABLE_MIDI2LR
         const fs::path p {rsj::AppDataFilePath(kSettingsFileX)};
#else
         const auto p {rsj::AppDataFilePath(kSettingsFileX)};
#endif
         std::ofstream outfile {p, std::ios::trunc};
         if (outfile.is_open()) {
#pragma warning(suppress : 26414) /* too large to construct on stack */
            const auto oarchive {std::make_unique<cereal::XMLOutputArchive>(outfile)};
            (*oarchive)(controls_model_);
#ifdef FILESYSTEM_AVAILABLE_MIDI2LR
            rsj::Log(
                fmt::format(FMT_STRING("ControlsModel archive in Main saved to {}."), p.string()));
#else
            rsj::Log(fmt::format(FMT_STRING("ControlsModel archive in Main saved to {}."), p));
#endif
         }
         else
            rsj::LogAndAlertError(
                juce::translate("Unable to save settings.xml"), "Unable to save settings.xml");
      }
      catch (const std::exception& e) {
         MIDI2LR_E_RESPONSE;
      }
   }

   void LoadControlsModel()
   {
      try {
#ifdef FILESYSTEM_AVAILABLE_MIDI2LR
         const fs::path px {rsj::AppDataFilePath(kSettingsFileX)};
#else
         const auto px {rsj::AppDataFilePath(kSettingsFileX)};
#endif
         std::ifstream in_file {px};
         if (in_file.is_open() && !in_file.eof()) {
#pragma warning(suppress : 26414) /* too large to construct on stack */
            const auto iarchive {std::make_unique<cereal::XMLInputArchive>(in_file)};
            (*iarchive)(controls_model_);
#ifdef FILESYSTEM_AVAILABLE_MIDI2LR
            rsj::Log(fmt::format(
                FMT_STRING("ControlsModel archive in Main loaded from {}."), px.string()));
#else
            rsj::Log(fmt::format(FMT_STRING("ControlsModel archive in Main loaded from {}."), px));
#endif
         }
      }
      catch (const std::exception& e) {
         MIDI2LR_E_RESPONSE;
      }
   }

   void SetAppFont() const
   {
      /* juce (as of July 2018) uses the following font defaults taken from juce_mac_Fonts.mm and
       * juce_wind32_Fonts.cpp. Sans defaults do not support Asian languages.
       * SEE:https://docs.microsoft.com/en-us/typography/fonts/windows_10_font_list avoiding fonts
       * added in windows 10 to support people using earlier Windows versions
       * SEE:https://docs.microsoft.com/en-us/windows/desktop/uxguide/vis-fonts
       * SEE:https://docs.microsoft.com/en-us/windows/uwp/design/globalizing/loc-international-fonts
       * but Meiryo UI doesn't work well on my computer, so using MS UI Gothic instead for ja.
       * PingFang added in 10.11 El Capitan as new Chinese UI fonts
       *
       * Juce defaults:
       *                MacOS            Windows
       *        Sans    Lucida Grande    Verdana
       *        Serif   Times New Roman  Times New Roman
       *        Fixed   Menlo            Lucida Console
       */
      try {
         const auto& lang {command_set_.GetLanguage()};
         juce::String font1_name;
         juce::String font2_name;
         if (lang == "ko")
            font1_name = "NotoSansKR-Regular.otf";
         else if (lang == "zh_tw")
            font1_name = "NotoSansTC-Regular.otf";
         else if (lang == "zh_cn")
            font1_name = "NotoSansSC-Regular.otf";
         else if (lang == "ja")
            font1_name = "NotoSansJP-Regular.otf";
         else {
            font1_name = "NotoSans-Regular-Plus-Thai.ttf";
            font2_name = "NotoSans-Bold-plus-Thai.ttf";
         }
         juce::MemoryBlock font_data {};
         auto font_file = juce::File::getSpecialLocation(juce::File::currentApplicationFile)
                              .getSiblingFile(font1_name);
         if (font_file.loadFileAsData(font_data))
            juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(
                juce::Typeface::createSystemTypefaceFor(font_data.getData(), font_data.getSize()));
         else
            rsj::Log(fmt::format(
                FMT_STRING("Unable to load primary font file {}."), font1_name.toStdString()));
         if (font2_name.isNotEmpty()) {
            font_data.reset();
            font_file = juce::File::getSpecialLocation(juce::File::currentApplicationFile)
                            .getSiblingFile(font2_name);
            if (font_file.loadFileAsData(font_data))
               juce::Typeface::createSystemTypefaceFor(font_data.getData(), font_data.getSize());
            else
               rsj::Log(fmt::format(
                   FMT_STRING("Unable to load bold font file {}."), font2_name.toStdString()));
         }
      }
      catch (const std::exception& e) {
         MIDI2LR_E_RESPONSE;
         throw;
      }
   }

   /* create logger first, makes sure that MIDI2LR directory is created for writing by other modules
    * log file created at %AppData%\MIDI2LR (Windows) or ~/Library/Logs/MIDI2LR (OSX) need to own
    * pointer created by createDefaultAppLogger */
   std::unique_ptr<juce::FileLogger> logger_ {
       juce::FileLogger::createDefaultAppLogger("MIDI2LR", "MIDI2LR.log", "", 32 * 1024)}; //-V112
   /* forcing assignment to static early in construction */
   [[maybe_unused, no_unique_address]] UpdateCurrentLogger dummy_ {logger_.get()};
   Devices devices_ {};
   const CommandSet command_set_ {};
   ControlsModel controls_model_ {};
   Profile profile_ {command_set_};
   MidiSender midi_sender_ {devices_};
   MidiReceiver midi_receiver_ {devices_};
   LrIpcOut lr_ipc_out_ {controls_model_, profile_, midi_sender_, midi_receiver_};
   ProfileManager profile_manager_ {controls_model_, profile_, lr_ipc_out_, midi_receiver_};
   LrIpcIn lr_ipc_in_ {controls_model_, profile_manager_, profile_, midi_sender_};
   SettingsManager settings_manager_ {profile_manager_, lr_ipc_out_};
   std::unique_ptr<MainWindow> main_window_ {nullptr};
   /* destroy after window that uses it */
   LookAndFeelMIDI2LR look_feel_;
   /* initialize this last as it needs window to exist */
   VersionChecker version_checker_ {settings_manager_};
};

/* This macro generates the main() routine that launches the application. */
#pragma warning(suppress : 26409 26425 28251)
START_JUCE_APPLICATION(MIDI2LRApplication)