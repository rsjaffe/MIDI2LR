/*
  ==============================================================================

    VersionChecker.h
    Created: 19 Aug 2015 1:21:14pm
    Author:  Parth

  ==============================================================================
*/

#ifndef VERSIONCHECKER_H_INCLUDED
#define VERSIONCHECKER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class VersionChecker : public Thread,
                       public AsyncUpdater
{
public:
    VersionChecker();

    // Thread interface
    virtual void run() override;

    // AsyncUpdater interface
    virtual void handleAsyncUpdate() override;

private:
    int _newVersion;
    ScopedPointer<DialogWindow> _dialog;
};



#endif  // VERSIONCHECKER_H_INCLUDED
