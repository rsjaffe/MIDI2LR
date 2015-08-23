/*
  ==============================================================================

    LR_IPC_OUT.h
    Created: 2 Aug 2015 12:27:47am
    Author:  Parth

  ==============================================================================
*/

#ifndef LR_IPC_OUT_H_INCLUDED
#define LR_IPC_OUT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "MIDIProcessor.h"

class LRConnectionListener
{
public:
    virtual void connected() = 0;
    virtual void disconnected() = 0;
};

class LR_IPC_OUT : public InterprocessConnection,
               public MIDICommandListener,
               public AsyncUpdater,
               public Timer
{
public:
    static LR_IPC_OUT& getInstance();
    void shutdown();

    static const int LR_OUT_PORT;

    // IPC interface
    virtual void connectionMade() override;
    virtual void connectionLost() override;
    virtual void messageReceived(const MemoryBlock& msg) override;

    void addListener(LRConnectionListener *listener);
    
    // MIDICommandListener interface
    virtual void handleMidiCC(int midiChannel, int controller, int value) override;
    virtual void handleMidiNote(int midiChannel, int note) override;
    
    // AsyncUpdater interface
    virtual void handleAsyncUpdate() override;

    // Timer callback
    virtual void timerCallback() override;
private:
    LR_IPC_OUT();

    LR_IPC_OUT(LR_IPC_OUT const&) = delete;
    void operator=(LR_IPC_OUT const&) = delete;

    Array<LRConnectionListener *> _listeners;
    int _valueToSend;
    String _commandToSend;
};


#endif  // LR_IPC_OUT_H_INCLUDED
