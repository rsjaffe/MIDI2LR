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

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <future>
#include <memory>
#include <mutex>
#include <utility>

#include <asio/asio.hpp>
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
#include <ww898/utf_converters.hpp>
#endif

namespace fs = std::filesystem;

namespace {
   constexpr auto kShutDownString {"--LRSHUTDOWN"};
   constexpr auto kSettingsFileX {MIDI2LR_UC_LITERAL("settings.xml")};
   constexpr auto kDefaultsFile {MIDI2LR_UC_LITERAL("default.xml")};

   class LookAndFeelMIDI2LR final : public juce::LookAndFeel_V3 {
    public:
      LookAndFeelMIDI2LR() noexcept { juce::LookAndFeel::setDefaultLookAndFeel(this); }

      ~LookAndFeelMIDI2LR() { juce::LookAndFeel::setDefaultLookAndFeel(nullptr); }

      LookAndFeelMIDI2LR(const LookAndFeelMIDI2LR& s) = delete;
      LookAndFeelMIDI2LR(LookAndFeelMIDI2LR&& s) = delete;
      LookAndFeelMIDI2LR& operator=(const LookAndFeelMIDI2LR& s) = delete;
      LookAndFeelMIDI2LR& operator=(LookAndFeelMIDI2LR&& s) = delete;

      juce::Font getTextButtonFont(juce::TextButton&, const int button_height) override
      {
         return {std::min(16.0F, static_cast<float>(button_height) * 0.7F)};
      }
   };

   class SetLogger {
    public:
      SetLogger() noexcept
      {
         juce::Logger::setCurrentLogger(logger_.get());
#ifdef _WIN32
         try {
            wil::SetResultLoggingCallback([](const wil::FailureInfo& failure) noexcept {
               std::array<wchar_t, 2048> dbg {};
               if (SUCCEEDED(wil::GetFailureLogString(dbg.data(), dbg.size(), failure))) {
                  rsj::Log(dbg.data(), std::source_location::current());
               }
               else {
                  rsj::Log(L"Call to wil::GetFailureLogString failed.",
                      std::source_location::current());
               }
            });
         }
         catch (const std::exception& e) {
            rsj::ExceptionResponse(e, std::source_location::current());
         }
         catch (...) {
            rsj::Log("Unable to set up wil logger.", std::source_location::current());
         }
#endif
      }

      ~SetLogger()
      {
#ifdef _WIN32
         try {
            wil::SetResultLoggingCallback(nullptr);
         }
         catch (const std::exception& e) {
            rsj::ExceptionResponse(e, std::source_location::current());
         }
         catch (...) {
            rsj::Log("Unable to reset wil logging callback.", std::source_location::current());
         }
#endif
         juce::Logger::setCurrentLogger(nullptr);
      }

      SetLogger(const SetLogger& s) = delete;
      SetLogger(SetLogger&& s) = default;
      SetLogger& operator=(const SetLogger& s) = delete;
      SetLogger& operator=(SetLogger&& s) = default;

    private:
      std::unique_ptr<juce::FileLogger> logger_ {juce::FileLogger::createDefaultAppLogger("MIDI2LR",
          "MIDI2LR.log", "", 32LL * 1024LL)}; //-V112
   };

   [[noreturn]] void OnTerminate() noexcept
   {
      try {
         static std::mutex terminate_mutex;
         auto lock {std::scoped_lock(terminate_mutex)};
         if (const auto exc {std::current_exception()}) {
            /* we have an exception */
            try {
               /* throw to recognize the type */
               std::rethrow_exception(exc);
            }
            catch (const std::exception& e) {
               rsj::Log(fmt::format(FMT_STRING("Terminate called, exception {}."), e.what()),
                   std::source_location::current());
            }
            catch (...) {
               rsj::Log("Terminate called, unknown exception type.",
                   std::source_location::current());
            }
         }
         else {
            rsj::Log("Terminate called, no exception available.", std::source_location::current());
         }
      }
      catch (...) { //-V565 //-V5002  // NOLINT(bugprone-empty-catch)
      }
      std::_Exit(EXIT_FAILURE);
   }

   void LoadFont(const juce::String& font_name, bool is_primary) noexcept
   {
      juce::MemoryBlock font_data {};
      const auto font_file = juce::File::getSpecialLocation(juce::File::currentApplicationFile)
                                 .getSiblingFile(font_name);
      if (font_file.loadFileAsData(font_data)) {
         const auto typeface =
             juce::Typeface::createSystemTypefaceFor(font_data.getData(), font_data.getSize());
         if (is_primary) {
            juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(typeface);
         }
      }
      else {
         rsj::Log(fmt::format(FMT_STRING("Unable to load {} font file {}."),
                      is_primary ? "primary" : "bold", font_name.toStdString()),
             std::source_location::current());
      }
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

   bool moreThanOneInstanceAllowed() noexcept override { return false; }

   void initialise(const juce::String& command_line) override
   {
      /*Called when the application starts. This will be called once to let the application do
       * whatever initialization it needs, create its windows, etc. After the method returns, the
       * normal event - dispatch loop will be run, until the quit() method is called, at which
       * point the shutdown() method will be called to let the application clear up anything it
       * needs to delete. If during the initialise() method, the application decides not to
       * start-up after all, it can just call the quit() method and the event loop won't be
       * run. */
      try {
         if (command_line != kShutDownString) {
            InitializeThreads();
            LinkControlsModels();
            /* Need to start main window before ipc so it's already registered its callbacks and can
             * receive messages */
            SetupUi();
            StartServices();
         }
         else {
            quit();
         }
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(e, std::source_location::current());
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
      StopServices();
      DefaultProfileSave();
      SaveControlsModel();
   }

   void systemRequestedQuit() override
   {
      /*This is called when the application is being asked to quit: you can ignore this request
       * and let the application carry on running, or call quit() to allow the application to
       * close. */
      try {
         constinit static std::once_flag of; /* function might be called twice during LR shutdown */
         std::call_once(of, [this] {
            if (profile_.ProfileUnsaved() && main_window_) {
               if (juce::NativeMessageBox::showYesNoBox(juce::AlertWindow::WarningIcon,
                       juce::translate("MIDI2LR profiles"),
                       juce::translate("Profile changed. Do you want to save your changes? If "
                                       "you continue without saving, your changes will be "
                                       "lost."))) {
                  main_window_->SaveProfile();
               }
            }
            quit();
         });
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(e, std::source_location::current());
         throw;
      }
   }

   void anotherInstanceStarted(const juce::String& command_line) override
   {
      if (command_line == kShutDownString) { systemRequestedQuit(); }
   }

   [[noreturn]] void unhandledException(const std::exception* e,
       const juce::String& source_filename, int line_number) override
   {
      /* If any unhandled exceptions make it through to the message dispatch loop, this callback
       * will be triggered, in case you want to log them or do some other type of error-handling. If
       * the type of exception is derived from the std::exception class, the pointer passed-in will
       * be valid. If the exception is of unknown type, this pointer will be null. */
      try {
         if (e) {
            const auto msgt {juce::translate("unhandled exception").toStdString()
                             + " {}, {} line {}. Total uncaught {}."};
            rsj::LogAndAlertError(fmt::format(msgt, e->what(), source_filename.toStdString(),
                                      line_number, std::uncaught_exceptions()),
                fmt::format(FMT_STRING("Unhandled exception {}, {} line {}. Total uncaught {}."),
                    e->what(), source_filename.toStdString(), line_number,
                    std::uncaught_exceptions()),
                std::source_location::current());
         }
         else {
            const auto msgt {juce::translate("unhandled exception").toStdString()
                             + " {} line {}. Total uncaught {}."};
            rsj::LogAndAlertError(fmt::format(msgt, source_filename.toStdString(), line_number,
                                      std::uncaught_exceptions()),
                fmt::format(FMT_STRING("Unhandled exception {} line {}. Total uncaught {}."),
                    source_filename.toStdString(), line_number, std::uncaught_exceptions()),
                std::source_location::current());
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
   void InitializeThreads()
   {
      MIDI2LR_FAST_FLOATS;
      rsj::LabelThread(MIDI2LR_UC_LITERAL("Main MIDI2LR thread"));
      StartThread(io_thread0_, "io_thread0_");
      StartThread(io_thread1_, "io_thread1_");
   }

   void StartThread(std::future<void>& thread, const char* threadName)
   {
      thread = std::async(std::launch::async, [this, threadName] {
#ifdef _WIN32
         const auto uwr = ww898::utf::convz<wchar_t>(threadName);
         rsj::LabelThread(uwr.c_str());
#else
         rsj::LabelThread(threadName);
#endif
         MIDI2LR_FAST_FLOATS;
         if constexpr (kNdebug) { io_context_.run(); }
         else {
            rsj::Log(fmt::format(FMT_STRING("{} ran {} handlers."), threadName, io_context_.run()),
                std::source_location::current());
         }
      });
   }

   void LinkControlsModels()
   {
      CCoptions::LinkToControlsModel(&controls_model_);
      PWoptions::LinkToControlsModel(&controls_model_);
   }

   void SetupUi()
   {
      SetAppFont();
      LoadControlsModel();
      main_window_ = std::make_unique<MainWindow>(getApplicationName(), command_set_, profile_,
          profile_manager_, settings_manager_, lr_ipc_out_, midi_receiver_, midi_sender_);
   }

   void StartServices()
   {
      midi_receiver_.Start();
      midi_sender_.Start();
      lr_ipc_out_.Start();
      lr_ipc_in_.Start();
      version_checker_.Start();
   }

   void StopServices()
   {
      /*Primary goals: 1) remove callbacks in LR_IPC_Out and MIDIReceiver before the callee is
       * destroyed, 2) stop additional threads in VersionChecker, LR_IPC_In, LR_IPC_Out and
       * MIDIReceiver. Add to this list if new threads or callback lists are developed in this
       * app. */
      midi_receiver_.Stop();
      lr_ipc_in_.Stop();
      lr_ipc_out_.Stop();
      version_checker_.Stop();
      io_context_.stop();
   }

   void DefaultProfileSave() noexcept
   {
      try {
         const auto file_name {rsj::AppDataFilePath(kDefaultsFile)};
         const auto profile_file {juce::File(file_name.data())};
         profile_.ToXmlFile(profile_file);
         rsj::Log(fmt::format(FMT_STRING("Default profile saved to {}."),
                      profile_file.getFullPathName().toStdString()),
             std::source_location::current());
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(e, std::source_location::current());
      }
   }

   void SaveControlsModel() const noexcept
   {
      try {
         const fs::path p {rsj::AppDataFilePath(kSettingsFileX)};
         if (std::ofstream outfile {p, std::ios::trunc}; outfile.is_open()) {
#pragma warning(suppress : 26414) /* too large to construct on stack */
            const auto oarchive {std::make_unique<cereal::XMLOutputArchive>(outfile)};
            (*oarchive)(controls_model_);
            rsj::Log(fmt::format(FMT_STRING("ControlsModel archive in Main saved to {}."),
                         p.string()),
                std::source_location::current());
         }
         else {
            rsj::LogAndAlertError(juce::translate("Unable to save settings.xml"),
                "Unable to save settings.xml", std::source_location::current());
         }
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(e, std::source_location::current());
      }
   }

   void LoadControlsModel()
   {
      try {
         const fs::path px {rsj::AppDataFilePath(kSettingsFileX)};
         if (std::ifstream in_file {px}; in_file.is_open() && !in_file.eof()) {
#pragma warning(suppress : 26414) /* too large to construct on stack */
            const auto iarchive {std::make_unique<cereal::XMLInputArchive>(in_file)};
            (*iarchive)(controls_model_);
            rsj::Log(fmt::format(FMT_STRING("ControlsModel archive in Main loaded from {}."),
                         px.string()),
                std::source_location::current());
         }
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(e, std::source_location::current());
         throw;
      }
   }

   void SetAppFont() const noexcept
   {
      try {
         const auto& lang {rsj::ToLower(command_set_.GetLanguage())};
         const std::unordered_map<std::string, std::pair<std::string, std::string>> font_map {
             {     "ko",                                {"NotoSansKR-Regular.otf", ""}},
             {  "zh_tw",                                {"NotoSansTC-Regular.otf", ""}},
             {  "zh_cn",                                {"NotoSansSC-Regular.otf", ""}},
             {     "ja",                                {"NotoSansJP-Regular.otf", ""}},
             {"default", {"NotoSans-Regular-MIDI2LR.ttf", "NotoSans-Bold-MIDI2LR.ttf"}}
         };
         auto font_pair = font_map.find(lang);
         if (font_pair == font_map.end()) { font_pair = font_map.find("default"); }
         const auto& primary_font_name = font_pair->second.first;
         const auto& bold_font_name = font_pair->second.second;
         LoadFont(primary_font_name, true);
         if (!bold_font_name.empty()) { LoadFont(bold_font_name, false); }
      }
      catch (const std::exception& e) {
         rsj::ExceptionResponse(e, std::source_location::current());
      }
   }

   /* create logger first, makes sure that MIDI2LR directory is created for writing by other modules
    * log file created at %AppData%\MIDI2LR (Windows) or ~/Library/Logs/MIDI2LR (OSX) need to own
    * pointer created by createDefaultAppLogger */
   /* forcing assignment to static early in construction */
   [[maybe_unused]] const SetLogger dummy_ {};
   asio::io_context io_context_ {};
   std::future<void> io_thread0_;
   std::future<void> io_thread1_;
   [[maybe_unused]] asio::executor_work_guard<asio::io_context::executor_type> guard_ {
       asio::make_work_guard(io_context_)};
   Devices devices_ {};
   const CommandSet command_set_ {};
   ControlsModel controls_model_ {};
   Profile profile_ {command_set_};
   MidiSender midi_sender_ {devices_};
   MidiReceiver midi_receiver_ {devices_};
   LrIpcOut lr_ipc_out_ {
       command_set_, controls_model_, profile_, midi_sender_, midi_receiver_, io_context_};
   ProfileManager profile_manager_ {controls_model_, profile_, lr_ipc_out_, midi_receiver_};
   LrIpcIn lr_ipc_in_ {controls_model_, profile_manager_, profile_, midi_sender_, io_context_};
   SettingsManager settings_manager_ {profile_manager_, lr_ipc_out_};
   [[maybe_unused]] const LookAndFeelMIDI2LR dummy1_;
   std::unique_ptr<MainWindow> main_window_ {nullptr};
   VersionChecker version_checker_ {settings_manager_};
};

/* This macro generates the main() routine that launches the application. */
#pragma warning(suppress : 26409 26425 28251)
START_JUCE_APPLICATION(MIDI2LRApplication)