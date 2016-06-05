/*
  ==============================================================================

    VersionChecker.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/
#include "VersionChecker.h"

VersionChecker::VersionChecker() noexcept : Thread{ "VersionChecker" }
{
}

void VersionChecker::Init(std::shared_ptr<SettingsManager>& settings_manager) noexcept
{
    settings_manager_ = settings_manager;
}

void VersionChecker::run()
{
    URL version_url{ "http://rsjaffe.github.io/MIDI2LR/version.xml" };
    unique_ptr<XmlElement> version_xml_element{ version_url.readEntireXmlStream() };
    int last_checked{ 0 };
    if (settings_manager_)
        last_checked = settings_manager_->getLastVersionFound();
    if (version_xml_element != nullptr && (version_xml_element->getIntAttribute("latest") > ProjectInfo::versionNumber) && (version_xml_element->getIntAttribute("latest") != last_checked))
    {
        new_version_ = version_xml_element->getIntAttribute("latest");
        if (settings_manager_)
            settings_manager_->setLastVersionFound(new_version_);
        triggerAsyncUpdate();
    }
}

void VersionChecker::handleAsyncUpdate()
{
    // show a dialog box indicating there is a newer version available
    DialogWindow::LaunchOptions dialog_options;
    dialog_options.dialogTitle = "New Version Available!";

    const auto major{ (new_version_ & 0xFF000000) >> 24 };
    const auto minor{ (new_version_ & 0xFF0000) >> 16 };
    const auto rev{ (new_version_ & 0xFF00) >> 8 };
    const auto build{ (new_version_ & 0xFF) };
    const auto version_string{ String::formatted("New version %d.%d.%d.%d available", major, minor, rev, build) };
    const URL download_url{ "https://github.com/rsjaffe/MIDI2LR/releases/latest" };

    dialog_options.content.setOwned(new HyperlinkButton{ version_string, download_url });
    dialog_options.content->setSize(300, 100);
    (static_cast<HyperlinkButton *>(dialog_options.content.get()))->setFont(Font{ 18.f }, false);
    dialog_options.escapeKeyTriggersCloseButton = true;
    dialog_.reset(dialog_options.create());
    dialog_->setVisible(true);
}