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

#include "Misc.h"
#include "SettingsManager.h"

namespace {
   [[nodiscard]] std::string IntToVersion(unsigned int vers)
   {
      static_assert(std::is_unsigned_v<decltype(vers)>, "Avoid sign extenstion");
      const auto major{vers >> 24 & 0xFFu};
      const auto minor{vers >> 16 & 0xFFu};
      const auto rev{vers >> 8 & 0xFFu};
      const auto build{vers & 0xFFu};
      return fmt::format("{}.{}.{}.{}", major, minor, rev, build);
   }
} // namespace

VersionChecker::VersionChecker(SettingsManager& settings_manager)
    : juce::Thread{"VersionChecker"}, settings_manager_{settings_manager}
{
}

void VersionChecker::Stop()
{
   if (!juce::Thread::stopThread(1000))
      rsj::Log("stopThread failed in VersionChecker destructor.");
}

void VersionChecker::handleAsyncUpdate()
{
   try {
      if (juce::Thread::threadShouldExit())
         return;
      juce::NativeMessageBox::showYesNoBox(juce::AlertWindow::AlertIconType::QuestionIcon,
          juce::translate("A new version of MIDI2LR is available."),
          juce::translate("Download new version of MIDI2LR") + ' ' + IntToVersion(new_version_),
          nullptr, juce::ModalCallbackFunction::create([this](int result) {
             if (result) {
                const auto git{juce::URL("https://github.com/rsjaffe/MIDI2LR/releases")};
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
      rsj::ExceptionResponse(typeid(this).name(), MIDI2LR_FUNC, e);
      throw;
   }
}

void VersionChecker::run()
{
   try {
      rsj::LabelThread(L"VersionChecker run thread");
      const juce::URL version_url{"https://rsjaffe.github.io/MIDI2LR/version.xml"};
      const auto version_xml_element{version_url.readEntireXmlStream()};
      if (version_xml_element && !juce::Thread::threadShouldExit()) {
         auto last_checked = settings_manager_.GetLastVersionFound();
         new_version_ = version_xml_element->getIntAttribute("latest");
         if (last_checked == 0) {
            last_checked = std::min(new_version_, ProjectInfo::versionNumber);
            settings_manager_.SetLastVersionFound(last_checked);
         }
         rsj::Log(fmt::format("Version available {}, version last checked {}, current version {}.",
             IntToVersion(new_version_), IntToVersion(last_checked),
             IntToVersion(ProjectInfo::versionNumber)));
         if (new_version_ > ProjectInfo::versionNumber && new_version_ != last_checked
             && !juce::Thread::threadShouldExit()) {
            triggerAsyncUpdate();
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), MIDI2LR_FUNC, e);
      throw;
   }
}