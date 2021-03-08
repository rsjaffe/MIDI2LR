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
#include <future>
#include <memory>
#include <mutex>
#include <version>

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

#ifdef __cpp_lib_semaphore
#include <semaphore>
#else
#include <condition_variable>
#endif
#ifndef _WIN32
#include <AvailabilityMacros.h>
#if defined(MAC_OS_X_VERSION_10_15) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_15     \
    && defined(__cpp_lib_filesystem)
#define MIDI2LR_FILESYSTEM_AVAILABLE
#endif
#else
#ifdef __cpp_lib_filesystem
#define MIDI2LR_FILESYSTEM_AVAILABLE
#endif
#endif
#ifdef MIDI2LR_FILESYSTEM_AVAILABLE
#include <filesystem>
namespace fs = std::filesystem;
#endif

#ifdef _WIN32
#include <array>

#include <wil/result.h> /* including too early causes conflicts with other windows includes */
#endif

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
         return juce::Font(std::min(16.0f, static_cast<float>(button_height) * 0.7f));
      }
   };

   class SetLogger {
    public:
      SetLogger() noexcept
      {
         juce::Logger::setCurrentLogger(logger_.get());
#ifdef _WIN32
         try {
            wil::SetResultLoggingCallback([](wil::FailureInfo const& failure) noexcept {
               std::array<wchar_t, 2048> debug_string {};
               wil::GetFailureLogString(debug_string.data(), debug_string.size(), failure);
               rsj::Log(debug_string.data());
            });
         }
         catch (const std::exception& e) {
            MIDI2LR_E_RESPONSE;
         }
         catch (...) {
            rsj::Log("Unable to set up wil logger.");
         }
#endif
      }

      ~SetLogger()
      {
#ifdef _WIN32
         wil::SetResultLoggingCallback(nullptr);
#endif
         juce::Logger::setCurrentLogger(nullptr);
      }
      SetLogger(const SetLogger& s) = delete;
      SetLogger(SetLogger&& s) = default;
      SetLogger& operator=(const SetLogger& s) = delete;
      SetLogger& operator=(SetLogger&& s) = default;

    private:
      std::unique_ptr<juce::FileLogger> logger_ {juce::FileLogger::createDefaultAppLogger(
          "MIDI2LR", "MIDI2LR.log", "", 32 * 1024)}; //-V112
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

   bool moreThanOneInstanceAllowed() noexcept override { return false; }

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
            MIDI2LR_FAST_FLOATS;
            rsj::LabelThread(MIDI2LR_UC_LITERAL("Main MIDI2LR thread"));
            io_thread0_ = std::async(std::launch::async, [this] {
               rsj::LabelThread(MIDI2LR_UC_LITERAL("io_thread0_"));
               MIDI2LR_FAST_FLOATS;
               if constexpr (kNdebug)
                  io_context_.run();
               else
                  rsj::Log(
                      fmt::format(FMT_STRING("io_thread0_ ran {} handlers."), io_context_.run()));
            });
            io_thread1_ = std::async(std::launch::async, [this] {
               rsj::LabelThread(MIDI2LR_UC_LITERAL("io_thread1_"));
               MIDI2LR_FAST_FLOATS;
               if constexpr (kNdebug)
                  io_context_.run();
               else
                  rsj::Log(
                      fmt::format(FMT_STRING("io_thread1_ ran {} handlers."), io_context_.run()));
            });
            CCoptions::LinkToControlsModel(&controls_model_);
            PWoptions::LinkToControlsModel(&controls_model_);
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
#ifdef __cpp_lib_semaphore
               std::binary_semaphore ready_sem(1);
#else
               std::condition_variable cv;
               std::mutex m;
               bool ready {false};
#endif

               juce::MessageManager::callAsync([&]() {
                  try {
                     const auto result {juce::NativeMessageBox::showYesNoBox(
                         juce::AlertWindow::WarningIcon, juce::translate("MIDI2LR profiles"),
                         juce::translate("Profile changed. Do you want to save your changes? If "
                                         "you continue without saving, your changes will be "
                                         "lost."))};
                     if (result)
                        main_window_->SaveProfile();
                  }
                  catch (const std::exception& e) {
                     MIDI2LR_E_RESPONSE; /* and continue, so ready flag can be set */
                  }
                  catch (...) {
                  }
#ifdef __cpp_lib_semaphore
                  ready_sem.release();
               });
               ready_sem.acquire();
#else
                  {
                     std::unique_lock<std::mutex> lk {m};
                     ready = true;
                  }
                  cv.notify_one();
               });
               std::unique_lock<std::mutex> lk {m};
               cv.wait(lk, [&] { return ready; });
#endif
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
       const std::exception* e, const juce::String& source_filename, int line_number) override
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
                    std::uncaught_exceptions()));
         }
         else {
            const auto msgt {juce::translate("unhandled exception").toStdString()
                             + " {} line {}. Total uncaught {}."};
            rsj::LogAndAlertError(fmt::format(msgt, source_filename.toStdString(), line_number,
                                      std::uncaught_exceptions()),
                fmt::format(FMT_STRING("Unhandled exception {} line {}. Total uncaught {}."),
                    source_filename.toStdString(), line_number, std::uncaught_exceptions()));
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
      }
   }

   void SaveControlsModel() const
   {
      try {
#ifdef MIDI2LR_FILESYSTEM_AVAILABLE
         const fs::path p {rsj::AppDataFilePath(kSettingsFileX)};
#else
         const auto p {rsj::AppDataFilePath(kSettingsFileX)};
#endif
         std::ofstream outfile {p, std::ios::trunc};
         if (outfile.is_open()) {
#pragma warning(suppress : 26414) /* too large to construct on stack */
            const auto oarchive {std::make_unique<cereal::XMLOutputArchive>(outfile)};
            (*oarchive)(controls_model_);
#ifdef MIDI2LR_FILESYSTEM_AVAILABLE
            rsj::Log(
                fmt::format(FMT_STRING("ControlsModel archive in Main saved to {}."), p.string()));
#else
            rsj::Log(fmt::format(
                FMT_STRING(MIDI2LR_UC_LITERAL("ControlsModel archive in Main saved to {}.")), p));
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
#ifdef MIDI2LR_FILESYSTEM_AVAILABLE
         const fs::path px {rsj::AppDataFilePath(kSettingsFileX)};
#else
         const auto px {rsj::AppDataFilePath(kSettingsFileX)};
#endif
         std::ifstream in_file {px};
         if (in_file.is_open() && !in_file.eof()) {
#pragma warning(suppress : 26414) /* too large to construct on stack */
            const auto iarchive {std::make_unique<cereal::XMLInputArchive>(in_file)};
            (*iarchive)(controls_model_);
#ifdef MIDI2LR_FILESYSTEM_AVAILABLE
            rsj::Log(fmt::format(
                FMT_STRING("ControlsModel archive in Main loaded from {}."), px.string()));
#else
            rsj::Log(fmt::format(
                FMT_STRING(MIDI2LR_UC_LITERAL("ControlsModel archive in Main loaded from {}.")),
                px));
#endif
         }
      }
      catch (const std::exception& e) {
         MIDI2LR_E_RESPONSE;
         throw;
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
         else if (lang == "zh_TW" || lang == "zh_tw")
            font1_name = "NotoSansTC-Regular.otf";
         else if (lang == "zh_CN" || lang == "zh_cn")
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
      }
   }

   /* create logger first, makes sure that MIDI2LR directory is created for writing by other modules
    * log file created at %AppData%\MIDI2LR (Windows) or ~/Library/Logs/MIDI2LR (OSX) need to own
    * pointer created by createDefaultAppLogger */
   /* forcing assignment to static early in construction */
   [[maybe_unused]] const SetLogger dummy_ {};
   std::future<void> io_thread0_;
   std::future<void> io_thread1_;
   asio::io_context io_context_ {};
   [[maybe_unused]] asio::any_io_executor work_ {
       asio::require(io_context_.get_executor(), asio::execution::outstanding_work.tracked)};
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