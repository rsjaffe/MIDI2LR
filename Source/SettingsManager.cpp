/*
  ==============================================================================

    SettingsManager.cpp

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
#include "SettingsManager.h"
#include "ProfileManager.h"

#ifndef _WIN32
//missing make_unique (C++14) in XCode
namespace std
{
    template<typename T, typename... Args>
    unique_ptr<T> make_unique(Args&&... args)
    {
        return unique_ptr<T>{new T{ forward<Args>(args)... }};
    }
}
#endif

//constexpr and auto don't work in XCode
const juce::String AutoHideSection{ "autohide" };

SettingsManager::SettingsManager(): m_lr_IPC_OUT{ nullptr }, m_profileManager{ nullptr }
{
    PropertiesFile::Options opts;
    opts.applicationName = "MIDI2LR";
    opts.commonToAllUsers = false;
    opts.filenameSuffix = "xml";
    opts.osxLibrarySubFolder = "Application Support/MIDI2LR";
    opts.storageFormat = PropertiesFile::storeAsXML;

    _propertiesFile = std::make_unique<PropertiesFile>(opts);

}

void SettingsManager::setPickupEnabled(bool enabled)
{
    _propertiesFile->setValue("pickup_enabled", enabled);
    _propertiesFile->saveIfNeeded();

    auto command = String::formatted("Pickup %d\n", enabled);

    if (m_lr_IPC_OUT)
    {
        m_lr_IPC_OUT->sendCommand(command);
    }

}

bool SettingsManager::getPickupEnabled() const noexcept
{
    return _propertiesFile->getBoolValue("pickup_enabled", true);
}

String SettingsManager::getProfileDirectory() const noexcept
{
    return _propertiesFile->getValue("profile_directory");
}

void SettingsManager::setProfileDirectory(const String& profileDirStr)
{
    _propertiesFile->setValue("profile_directory", profileDirStr);
    _propertiesFile->saveIfNeeded();

    if (m_profileManager)
    {
        File profileDir{ profileDirStr };
        m_profileManager->setProfileDirectory(profileDir);
    }
}

void SettingsManager::connected()
{
    auto command = String::formatted("Pickup %d\n", getPickupEnabled());

    if (m_lr_IPC_OUT)
    {
        m_lr_IPC_OUT->sendCommand(command);
    }

}

void SettingsManager::disconnected()
{

}

int SettingsManager::getAutoHideTime() const noexcept
{
    return _propertiesFile->getIntValue(AutoHideSection, 0);

}

void SettingsManager::setAutoHideTime(int newTime)
{
    _propertiesFile->setValue(AutoHideSection, newTime);
    _propertiesFile->saveIfNeeded();

}

int SettingsManager::getLastVersionFound() const noexcept
{
    return _propertiesFile->getIntValue("LastVersionFound", 0);
}

void SettingsManager::setLastVersionFound(int newversion)
{
    _propertiesFile->setValue("LastVersionFound", newversion);
    _propertiesFile->saveIfNeeded();
}

void SettingsManager::Init(std::shared_ptr<LR_IPC_OUT>& lr_IPC_OUT, std::shared_ptr<ProfileManager>& profileManager)
{
    m_lr_IPC_OUT = lr_IPC_OUT;

    if (m_lr_IPC_OUT)
    {
        // add ourselves as a listener to LR_IPC_OUT so that we can send plugin settings on connection
        m_lr_IPC_OUT->addListener(this);
    }

    m_profileManager = profileManager;

    if (m_profileManager)
    {
        // set the profile directory
        File profileDir{ getProfileDirectory() };
        profileManager->setProfileDirectory(profileDir);
    }

}