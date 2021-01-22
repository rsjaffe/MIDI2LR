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

#include <algorithm>
#include <exception>
#include <type_traits>

#include <fmt/format.h>

#include <JuceHeader.h>

#include "Misc.h"
#include "SettingsManager.h"

namespace {
   [[nodiscard]] std::string IntToVersion(unsigned int vers)
   {
      static_assert(std::is_unsigned_v<decltype(vers)>, "Avoid sign extension");
      const auto major {vers >> 24 & 0xFFu};
      const auto minor {vers >> 16 & 0xFFu};
      const auto rev {vers >> 8 & 0xFFu};
      const auto build {vers & 0xFFu};
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

void VersionChecker::Stop() noexcept { thread_should_exit_.store(true, std::memory_order_release); }

void VersionChecker::handleAsyncUpdate()
{
   try {
      if (thread_should_exit_.load(std::memory_order_acquire))
         return;
      juce::NativeMessageBox::showYesNoBox(juce::AlertWindow::AlertIconType::QuestionIcon,
          fmt::format(
              juce::translate("A new version of {} is available.").toStdString(), "MIDI2LR"),
          juce::translate("Do you want to download the latest version?") + ' '
              + IntToVersion(new_version_),
          nullptr, juce::ModalCallbackFunction::create([this](const int result) {
             if (result) {
                const auto git {juce::URL("https://github.com/rsjaffe/MIDI2LR/releases")};
                if (git.launchInDefaultBrowser())
                   /* successfully opened browser */
                   settings_manager_.SetLastVersionFound(new_version_);
             }
             else
                /* user doesn't want it, don't show again */
                settings_manager_.SetLastVersionFound(new_version_);
          }));
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
   }
}

void VersionChecker::Run()
{
   try {
      const juce::URL version_url {"https://rsjaffe.github.io/MIDI2LR/version.xml"};
      const auto version_xml_element {version_url.readEntireXmlStream()};
      if (version_xml_element && !thread_should_exit_.load(std::memory_order_acquire)) {
         auto last_checked {settings_manager_.GetLastVersionFound()};
         if (const auto os_specific_version =
                 version_xml_element->getIntAttribute(MSWindows ? "vMSWindows" : "vMacOS"))
            new_version_ = os_specific_version;
         else
            new_version_ = version_xml_element->getIntAttribute("vlatest");
         if (last_checked == 0) {
            last_checked = std::min(new_version_, ProjectInfo::versionNumber);
            settings_manager_.SetLastVersionFound(last_checked);
         }
         rsj::Log(fmt::format(
             FMT_STRING("Version available {}, version last checked {}, current version {}."),
             IntToVersion(new_version_), IntToVersion(last_checked),
             IntToVersion(ProjectInfo::versionNumber)));
         if (new_version_ > ProjectInfo::versionNumber && new_version_ != last_checked
             && !thread_should_exit_.load(std::memory_order_acquire)) {
            triggerAsyncUpdate();
         }
      }
      else
         rsj::Log("Unable to download MIDI2LR/version.xml and parse into valid XML document.");
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
   }
}