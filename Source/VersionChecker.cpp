/*
  ==============================================================================

    VersionChecker.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

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
#include <utility>

VersionChecker::VersionChecker() noexcept : juce::Thread{"VersionChecker"} {}

VersionChecker::~VersionChecker() {
  stopThread(100);
}

void VersionChecker::Init(std::weak_ptr<SettingsManager>&& settings_manager) noexcept {
  settings_manager_ = std::move(settings_manager);
}

void VersionChecker::run() {
  const juce::URL version_url{"http://rsjaffe.github.io/MIDI2LR/version.xml"};
  const std::unique_ptr<juce::XmlElement> version_xml_element{version_url.readEntireXmlStream()};

  if (version_xml_element != nullptr) {
    int last_checked = 0;
    if (const auto smp = settings_manager_.lock())
      last_checked = smp->getLastVersionFound();
    new_version_ = version_xml_element->getIntAttribute("latest");
    if (new_version_ > ProjectInfo::versionNumber && new_version_ != last_checked) {
      if (const auto smp = settings_manager_.lock())
        smp->setLastVersionFound(new_version_);
      triggerAsyncUpdate();
    }
  }
}

void VersionChecker::handleAsyncUpdate() {
    // show a dialog box indicating there is a newer version available
  juce::DialogWindow::LaunchOptions dialog_options;
  dialog_options.dialogTitle = "New Version Available!";
  const auto major{(new_version_ & 0xFF000000) >> 24};
  const auto minor{(new_version_ & 0xFF0000) >> 16};
  const auto rev{(new_version_ & 0xFF00) >> 8};
  const auto build{(new_version_ & 0xFF)};
  const auto version_string{juce::String::formatted("New version %d.%d.%d.%d available",
    major, minor, rev, build)};
  const juce::URL download_url{"https://github.com/rsjaffe/MIDI2LR/releases/latest"};
  dialog_options.content.setOwned(new juce::HyperlinkButton{version_string, download_url});
  dialog_options.content->setSize(300, 100);
  (static_cast<juce::HyperlinkButton *>(dialog_options.content.get()))->
    setFont(juce::Font{18.f}, false);
  dialog_options.escapeKeyTriggersCloseButton = true;
  dialog_.reset(dialog_options.create());
  dialog_->setVisible(true);
}