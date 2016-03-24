#pragma once
/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/
/*
  ==============================================================================
This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/
#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "CommandTable.h"
#include "CommandTableModel.h"
#include "MIDIProcessor.h"
#include "LR_IPC_OUT.h"
#include "ProfileManager.h"
#include "ResizableLayout.h"
#include "CommandMap.h"
#include "LR_IPC_In.h"
#include "LR_IPC_Out.h"
#include "SettingsManager.h"

/**********************************************************************************************//**
* @class   MainContentComponent
*
* @brief   This component lives inside our window, and this is where you should put all
    your controls and content.
*
* @author  Rory Jaffe
* @date    3/20/2016
**************************************************************************************************/

class MainContentComponent: public Component,
    public MIDICommandListener,
    public LRConnectionListener,
    public AsyncUpdater,
    public Timer,
    public ButtonListener,
    public ProfileChangeListener,
    public ResizableLayout
{
public:
    MainContentComponent();
    virtual ~MainContentComponent();

    void paint (Graphics&);

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

    // ProfileChangeListener interface
    virtual void profileChanged(XmlElement* elem, const String& filename) override;
    void SetTimerText(int timeValue);
	void Init(std::shared_ptr<CommandMap> commandMap, std::shared_ptr<LR_IPC_IN> in, 
        std::shared_ptr<LR_IPC_OUT> out, std::shared_ptr<MIDIProcessor> midiProcessor, 
        std::shared_ptr<ProfileManager>profileManager, std::shared_ptr<SettingsManager>settingsManager, 
        std::shared_ptr<MIDISender> midiSender);
protected:
    void SetLabelSettings(Label &lblToSet);
    
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
    CommandTable _commandTable;
    CommandTableModel _commandTableModel;
    Label _profileNameLabel;

    ScopedPointer<DialogWindow> _settingsDialog;
//  SystemTrayIconComponent _systemTrayComponent;

    String _lastCommand;
    int _rowToSelect;
    Label m_currentStatus;

    std::shared_ptr<CommandMap> m_commandMap;
    std::shared_ptr<LR_IPC_IN> m_lr_IPC_IN;
    std::shared_ptr<LR_IPC_OUT> m_lr_IPC_OUT;
    std::shared_ptr<SettingsManager> m_settingsManager;
    std::shared_ptr<MIDIProcessor> m_midiProcessor;
    std::shared_ptr<MIDISender> m_midiSender;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
