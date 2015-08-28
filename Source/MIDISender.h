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
#include "CommandMap.h"
#include <unordered_map>

class MIDISender : public Timer
{
public:
    static MIDISender& getInstance();
    void queueCCForSending(int midi_channel, int controller, int value);
    
    void rescanDevices();

    // Timer interface
    virtual void timerCallback() override;
private:
    MIDISender();
    ~MIDISender();

    MIDISender(MIDISender const&) = delete;
    void operator=(MIDISender const&) = delete;

    OwnedArray<MidiOutput> _outputDevices;
    std::unordered_map<MIDI_Message, int> _ccMap;

    void initDevices();
    void sendCC(int midi_channel, int controller, int value);
};


#endif  // MIDISENDER_H_INCLUDED
