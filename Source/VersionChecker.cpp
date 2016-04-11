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

/**********************************************************************************************//**
 * @fn  VersionChecker::VersionChecker() noexcept
 *
 * @brief   Default constructor.
 *
 *
 **************************************************************************************************/

VersionChecker::VersionChecker() noexcept : Thread{ "VersionChecker" }
{

}

/**********************************************************************************************//**
 * @fn  void VersionChecker::Init(std::shared_ptr<SettingsManager>& settingsManager)
 *
 * @brief   Inits the given settings manager.
 *
 *
 *
 * @param [in,out]  settingsManager If non-null, manager for settings.
 **************************************************************************************************/

void VersionChecker::Init(std::shared_ptr<SettingsManager>& settingsManager) noexcept
{
    m_settingsManager = settingsManager;
}

/**********************************************************************************************//**
 * @fn  void VersionChecker::run()
 *
 * @brief   Runs this object.
 *
 *
 *
 **************************************************************************************************/

void VersionChecker::run()
{
    URL versionURL{ "http://rsjaffe.github.io/MIDI2LR/version.xml" };
    unique_ptr<XmlElement> versionElem{ versionURL.readEntireXmlStream() };
    int lastchecked{ 0 };
    if (m_settingsManager)
        lastchecked = m_settingsManager->getLastVersionFound();
    if (versionElem != nullptr && (versionElem->getIntAttribute("latest") > ProjectInfo::versionNumber) && (versionElem->getIntAttribute("latest") != lastchecked))
    {
        _newVersion = versionElem->getIntAttribute("latest");
        if (m_settingsManager)
            m_settingsManager->setLastVersionFound(_newVersion);
        triggerAsyncUpdate();
    }
}

/**********************************************************************************************//**
 * @fn  void VersionChecker::handleAsyncUpdate()
 *
 * @brief   Handles the asynchronous update.
 *
 *
 **************************************************************************************************/

void VersionChecker::handleAsyncUpdate()
{
    // show a dialog box indicating there is a newer version available
    DialogWindow::LaunchOptions dwOpt;
    dwOpt.dialogTitle = "New Version Available!";

    const auto major{ (_newVersion & 0xF000000) >> 24 };
    const auto minor{ (_newVersion & 0x00F0000) >> 16 };
    const auto rev{ (_newVersion & 0x0000F00) >> 8 };
    const auto build{ (_newVersion & 0x000000F) };
    const auto versionString{ String::formatted("New version %d.%d.%d.%d available", major, minor, rev, build) };
    const URL downloadURL{ "https://github.com/rsjaffe/MIDI2LR/releases/latest" };

    dwOpt.content.setOwned(new HyperlinkButton{ versionString, downloadURL });
    dwOpt.content->setSize(300, 100);
    (static_cast<HyperlinkButton *>(dwOpt.content.get()))->setFont(Font{ 18.f }, false);
    dwOpt.escapeKeyTriggersCloseButton = true;
    _dialog.reset(dwOpt.create());
    _dialog->setVisible(true);
}
