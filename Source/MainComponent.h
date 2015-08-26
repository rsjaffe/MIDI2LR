/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "CommandTable.h"
#include "CommandTableModel.h"
#include "MIDIProcessor.h"
#include "LR_IPC_OUT.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent   : public Component,
                               public MIDICommandListener,
                               public LRConnectionListener,
                               public AsyncUpdater,
                               public Timer,
                               public ButtonListener
{
public:
    MainContentComponent();
    ~MainContentComponent();

    void paint (Graphics&);
    void resized();

    // MIDICommandListener interface
    virtual void handleMidiCC(int midiChannel, int controller, int value) override;
    virtual void handleMidiNote(int midiChannel, int note) override;

    // LRConnectionListener interface
    virtual void connected() override;
    virtual void disconnected() override;

    // AsyncUpdater interface
    virtual void handleAsyncUpdate() override;

    // Timer interface
    virtual void timerCallback() override;

    // Button interface
    virtual void buttonClicked(Button* button) override;

private:
    Label _titleLabel;
    DropShadowEffect _titleShadow;
    Label _commandLabel;
    Label _connectionLabel;
    TextButton _rescanButton;
    TextButton _removeRowButton;
    TextButton _saveButton;
    TextButton _loadButton;
    TextButton _settingsButton;
    Label _versionLabel;
    ScopedPointer<DialogWindow> _settingsDialog;

    String _lastCommand;
    int _rowToSelect;

    CommandTable _commandTable;
    CommandTableModel _commandTableModel;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
