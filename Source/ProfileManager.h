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
#include "MIDIProcessor.h"

class ProfileChangeListener
{
public:
    virtual void profileChanged(XmlElement* elem, const String& filename) = 0;
};

class ProfileManager : public MIDICommandListener,
                       public AsyncUpdater
{
public:
    static ProfileManager& getInstance();
    void addListener(ProfileChangeListener *listener);
    void setProfileDirectory(File& dir);
    StringArray& getMenuItems();
    void switchToProfile(int profileIdx);
    void switchToProfile(const String& profile);
    void switchToNextProfile();
    void switchToPreviousProfile();

    // MIDICommandListener interface
    virtual void handleMidiCC(int midiChannel, int controller, int value) override;
    virtual void handleMidiNote(int midiChannel, int note) override;

    // AsyncUpdate interface
    virtual void handleAsyncUpdate() override;

private:
    enum class SWITCH_STATE
    {
        NONE,
        PREV,
        NEXT,
    };

    ProfileManager();

    ProfileManager(ProfileManager const&) = delete;
    void operator=(ProfileManager const&) = delete;

    File _profileLocation;
    StringArray _profiles;
    Array<ProfileChangeListener *> _listeners;
    int _currentProfileIdx;
    SWITCH_STATE _switchState;
};


#endif  // PROFILEMANAGER_H_INCLUDED
