/*
  ==============================================================================

    ProfileManager.cpp
    Created: 29 Aug 2015 10:27:13pm
    Author:  Parth

  ==============================================================================
*/

#include "ProfileManager.h"

ProfileManager& ProfileManager::getInstance()
{
    static ProfileManager instance;
    return instance;
}

ProfileManager::ProfileManager()
{

}

void ProfileManager::addListener(ProfileChangeListener *listener)
{
    _listeners.addIfNotAlreadyThere(listener);
}

void ProfileManager::setProfileDirectory(File& dir)
{
    _profileLocation = dir;

    Array<File> fileArray;
    dir.findChildFiles(fileArray, File::findFiles, false, "*.xml");

    _profiles.clear();
    for (auto file : fileArray)
        _profiles.add(file.getFileNameWithoutExtension());
}

StringArray& ProfileManager::getMenuItems()
{
    return _profiles;
}

void ProfileManager::switchToProfile(String& profile)
{
    String profileRef = profile + ".xml";
    File profileFile = _profileLocation.getChildFile(profileRef);

    if (profileFile.exists())
    {
        ScopedPointer<XmlElement> elem = XmlDocument::parse(profileFile);
        for (auto listener : _listeners)
            listener->profileChanged(elem);
    }
}

