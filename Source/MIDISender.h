/*
  ==============================================================================

    MIDISender.h
    Created: 22 Aug 2015 9:44:21pm
    Author:  Parth

  ==============================================================================
*/

#ifndef MIDISENDER_H_INCLUDED
#define MIDISENDER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class MIDISender
{
public:
    static MIDISender& getInstance();
    void sendCC(int midi_channel, int controller, int value);
    void rescanDevices();
private:
    MIDISender();
    ~MIDISender();

    MIDISender(MIDISender const&) = delete;
    void operator=(MIDISender const&) = delete;

    OwnedArray<MidiOutput> _outputDevices;

    void initDevices();
};


#endif  // MIDISENDER_H_INCLUDED
