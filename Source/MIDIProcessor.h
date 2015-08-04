/*
  ==============================================================================

    MIDIProcessor.h
    Created: 31 Jul 2015 11:56:19pm
    Author:  Parth

  ==============================================================================
*/

#ifndef MIDIPROCESSOR_H_INCLUDED
#define MIDIPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class MIDICommandListener
{
public:
    virtual void handleMidiCC(int midiChannel, int controller, int value) = 0;
    virtual void handleMidiNote(int midiChannel, int note) = 0;
};

class MIDIProcessor : public MidiInputCallback
{
public:
    static MIDIProcessor& getInstance();

    void handleIncomingMidiMessage(MidiInput*, const MidiMessage&) override;
    void addMIDICommandListener(MIDICommandListener*);
private:
    MIDIProcessor();
    ~MIDIProcessor();

    Array<MIDICommandListener *> _listeners;
    OwnedArray<MidiInput> _devices;
};


#endif  // MIDIPROCESSOR_H_INCLUDED
