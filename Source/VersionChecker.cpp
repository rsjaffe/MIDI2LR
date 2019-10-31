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

#include <exception>
#include <sstream>
#include <type_traits>

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
      std::ostringstream version_string;
      version_string << major << '.' << minor << '.' << rev << '.' << build;
      return version_string.str();
   }
} // namespace

VersionChecker::VersionChecker(SettingsManager& settings_manager)
    : juce::Thread{"VersionChecker"}, settings_manager_{settings_manager}
{
}

void VersionChecker::Stop()
{
   if (!juce::Thread::stopThread(1000))
      rsj::Log("stopThread failed in VersionChecker destructor");
}

void VersionChecker::handleAsyncUpdate()
{
   try {
      if (threadShouldExit())
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
                /* user doesn't want it */
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
      if (version_xml_element && !threadShouldExit()) {
         const auto last_checked = settings_manager_.GetLastVersionFound();
         new_version_ = version_xml_element->getIntAttribute("latest");
         rsj::Log("Version available " + IntToVersion(new_version_) + ", version last checked "
                  + IntToVersion(last_checked) + ", current version "
                  + IntToVersion(ProjectInfo::versionNumber) + '.');
         if (new_version_ > ProjectInfo::versionNumber && new_version_ != last_checked
             && !threadShouldExit()) {
            triggerAsyncUpdate();
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), MIDI2LR_FUNC, e);
      throw;
   }
}