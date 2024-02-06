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
#include "VersionChecker.h"

#include <atomic>
#include <exception>
#include <future>
#include <string>
#include <type_traits>
#include <utility>

#include <fmt/format.h>

#include <JuceHeader.h>

#include "Misc.h"
#include "SettingsManager.h"

namespace {
   [[nodiscard]] std::string IntToVersion(unsigned int vers)
   {
      static_assert(std::is_unsigned_v<decltype(vers)>, "Avoid sign extension");
      static_assert(sizeof vers >= 4, "At least 4 bytes"); //-V112
      const auto major {vers >> 24 & 0xFFU};
      const auto minor {vers >> 16 & 0xFFU};
      const auto rev {vers >> 8 & 0xFFU};
      const auto build {vers & 0xFFU};
      return fmt::format(FMT_STRING("{}.{}.{}.{}"), major, minor, rev, build);
   }
} // namespace

VersionChecker::VersionChecker(SettingsManager& settings_manager) noexcept
    : settings_manager_ {settings_manager}
{
}

void VersionChecker::Start()
{
   run_future_ = std::async(std::launch::async, [this] {
      rsj::LabelThread(MIDI2LR_UC_LITERAL("VersionChecker run thread"));
      MIDI2LR_FAST_FLOATS;
      Run();
   });
}

void VersionChecker::handleAsyncUpdate()
{
   const auto response {[this](const int result) {
      if (result) {
         if (juce::URL("https://github.com/rsjaffe/MIDI2LR/releases")
                 .launchInDefaultBrowser()) { /* successfully opened browser */
#ifndef MIDI2LR_BETA
            settings_manager_.SetLastVersionFound(new_version_);
#endif
         }
      }
#ifndef MIDI2LR_BETA
      else { /* user doesn't want it, don't show again */
         settings_manager_.SetLastVersionFound(new_version_);
      }
#endif
   }};
   /*                                                                       */
   try {
      if (thread_should_exit_.load(std::memory_order_acquire)) { return; }
      juce::NativeMessageBox::showYesNoBox(juce::AlertWindow::AlertIconType::QuestionIcon,
          fmt::format(juce::translate("A new version of {} is available.").toStdString(),
              "MIDI2LR"),
          juce::translate("Do you want to download the latest version?") + ' '
              + IntToVersion(new_version_),
          nullptr, juce::ModalCallbackFunction::create(response));
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
   }
}

namespace {
   int CheckVersion(const std::unique_ptr<juce::XmlElement>& version_xml_element)
   {
      int new_version;
      if (const auto os_specific_version =
              version_xml_element->getIntAttribute(MSWindows ? "vMSWindows" : "vMacOS")) {
         new_version = os_specific_version;
      }
      else {
         new_version = version_xml_element->getIntAttribute("vlatest");
      }
      return new_version;
   }

   void LogVersion(int new_version, int last_checked)
   {
      rsj::Log(fmt::format(FMT_STRING("Version available {}, version last checked {}, current "
                                      "version {}."),
          IntToVersion(new_version), IntToVersion(last_checked),
          IntToVersion(ProjectInfo::versionNumber)));
   }
} // namespace

void VersionChecker::Run() noexcept
{
   try {
      const juce::URL version_url {"https://rsjaffe.github.io/MIDI2LR/version.xml"};
      const auto version_xml_element {version_url.readEntireXmlStream()};
      if (version_xml_element && !thread_should_exit_.load(std::memory_order_acquire)) {
         auto last_checked {settings_manager_.GetLastVersionFound()};
         new_version_ = CheckVersion(version_xml_element);
         if (last_checked == 0) {
            last_checked = std::min(new_version_, ProjectInfo::versionNumber);
            settings_manager_.SetLastVersionFound(last_checked);
         }
         LogVersion(new_version_, last_checked);
         if (new_version_ > ProjectInfo::versionNumber && new_version_ != last_checked
             && !thread_should_exit_.load(std::memory_order_acquire)) {
            triggerAsyncUpdate();
         }
      }
      else {
         rsj::Log("Unable to download MIDI2LR/version.xml and parse into valid XML document.");
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
   }
}