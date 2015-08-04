/*
  ==============================================================================

    LR_IPC.h
    Created: 2 Aug 2015 12:27:47am
    Author:  Parth

  ==============================================================================
*/

#ifndef LR_IPC_H_INCLUDED
#define LR_IPC_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "MIDIProcessor.h"

class LR_IPC : public InterprocessConnection,
               public MIDICommandListener,
               public AsyncUpdater,
               public Timer
{
public:
    static LR_IPC& getInstance();
    void shutdown();

    static const int LR_PORT;

    virtual void connectionMade() override;
    virtual void connectionLost() override;
    virtual void messageReceived(const MemoryBlock& msg) override;
    
    // MIDICommandListener interface
    virtual void handleMidiCC(int midiChannel, int controller, int value) override;
    virtual void handleMidiNote(int midiChannel, int note) override;
    
    // AsyncUpdater interface
    virtual void handleAsyncUpdate() override;

    // Timer callback
    virtual void timerCallback() override;
private:
    LR_IPC();

    LR_IPC(LR_IPC const&) = delete;
    void operator=(LR_IPC const&) = delete;

    int _valueToSend;
    String _commandToSend;
};


#endif  // LR_IPC_H_INCLUDED
