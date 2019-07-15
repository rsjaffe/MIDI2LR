/*
  ==============================================================================

    VersionChecker.cpp

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
#include "VersionChecker.h"

#include <exception>
#include <sstream>

#include "Misc.h"
#include "SettingsManager.h"

namespace {
   [[nodiscard]] std::string IntToVersion(int vers)
   {
      const auto major{(vers & 0xFF000000) >> 24};
      const auto minor{(vers & 0xFF0000) >> 16};
      const auto rev{(vers & 0xFF00) >> 8};
      const auto build{(vers & 0xFF)};
      std::ostringstream version_string;
      version_string << major << '.' << minor << '.' << rev << '.' << build;
      return version_string.str();
   }
} // namespace

VersionChecker::VersionChecker(SettingsManager& settings_manager)
    : juce::Thread{"VersionChecker"}, settings_manager_{settings_manager}
{
}

void VersionChecker::StopRunning()
{
   if (!juce::Thread::stopThread(1000))
      rsj::Log("stopThread failed in VersionChecker destructor");
}

void VersionChecker::handleAsyncUpdate()
{
   try {
      // show a dialog box indicating there is a newer version available
      juce::DialogWindow::LaunchOptions dialog_options;
      dialog_options.dialogTitle = juce::translate("New Version Available!");
      const juce::URL download_url{"https://github.com/rsjaffe/MIDI2LR/releases/latest"};
      auto button =
          std::make_unique<juce::HyperlinkButton>(IntToVersion(new_version_), download_url);
      button->setFont(juce::Font{18.f}, false);
      dialog_options.content.setOwned(button.release());
      dialog_options.content->setSize(600, 100);
      dialog_.reset(dialog_options.create());
      dialog_->setVisible(true);
      settings_manager_.SetLastVersionFound(new_version_); // user has been notified
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void VersionChecker::run()
{
   try {
      rsj::LabelThread(L"VersionChecker run thread");
      const juce::URL version_url{"https://rsjaffe.github.io/MIDI2LR/version.xml"};
      const std::unique_ptr<juce::XmlElement> version_xml_element{
          version_url.readEntireXmlStream()};
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
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}