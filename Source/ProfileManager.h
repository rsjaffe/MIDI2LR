/*
  ==============================================================================

    ProfileManager.h
    Created: 29 Aug 2015 10:27:13pm
    Author:  Parth

  ==============================================================================
*/

#ifndef PROFILEMANAGER_H_INCLUDED
#define PROFILEMANAGER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class ProfileChangeListener
{
public:
    virtual void profileChanged(XmlElement*) = 0;
};

class ProfileManager
{
public:
    ProfileManager& getInstance();
    void addListener(ProfileChangeListener *listener);
    void setProfileDirectory(File& dir);
    StringArray& getMenuItems();
    void switchToProfile(String& profile);

private:
    File _profileLocation;
    StringArray _profiles;
    Array<ProfileChangeListener *> _listeners;
};


#endif  // PROFILEMANAGER_H_INCLUDED
