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

// checks http://thegouger.github.io/MIDI2LR/version.xml to see if we are running the latest version or not
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
