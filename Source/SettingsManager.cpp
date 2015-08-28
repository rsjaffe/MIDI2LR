/*
  ==============================================================================

    SettingsManager.cpp
    Created: 24 Aug 2015 7:11:29pm
    Author:  Parth

  ==============================================================================
*/

#include "SettingsManager.h"

SettingsManager::SettingsManager()
{
    PropertiesFile::Options opts;
    opts.applicationName = "MIDI2LR";
    opts.commonToAllUsers = false;
    opts.filenameSuffix = "xml";
    opts.osxLibrarySubFolder = "Application Support/MIDI2LR";
    opts.storageFormat = PropertiesFile::storeAsXML;

    _propertiesFile = new PropertiesFile(opts);

    LR_IPC_OUT::getInstance().addListener(this);
}

SettingsManager& SettingsManager::getInstance()
{
    static SettingsManager instance;
    return instance;
}

void SettingsManager::setPickupEnabled(bool enabled)
{
    _propertiesFile->setValue("pickup_enabled", enabled);
    _propertiesFile->saveIfNeeded();

    String command = String::formatted("Pickup %d\n", enabled);
    LR_IPC_OUT::getInstance().sendCommand(command);
}

bool SettingsManager::getPickupEnabled()
{
    return _propertiesFile->getBoolValue("pickup_enabled", true);
}

void SettingsManager::setMIDISendDelay(int delay)
{
    _propertiesFile->setValue("midi_delay", delay);
    _propertiesFile->saveIfNeeded();
}

int SettingsManager::getMIDISendDelay()
{
    return _propertiesFile->getIntValue("midi_delay", 20);
}

void SettingsManager::connected()
{
    String command = String::formatted("Pickup %d\n", getPickupEnabled());
    LR_IPC_OUT::getInstance().sendCommand(command);
}

void SettingsManager::disconnected()
{

}