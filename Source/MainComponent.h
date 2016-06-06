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

    void paint(Graphics&) override;

    // MIDICommandListener interface
    virtual void handleMidiCC(int midi_channel, int controller, int value) override;
    virtual void handleMidiNote(int midi_channel, int note) override;

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
    virtual void profileChanged(XmlElement* elem, const String& file_name) override;
    void SetTimerText(int time_value);
    void Init(std::shared_ptr<CommandMap>& command_map, std::shared_ptr<LR_IPC_IN>& in,
        std::shared_ptr<LR_IPC_OUT>& out, std::shared_ptr<MIDIProcessor>& midi_processor,
        std::shared_ptr<ProfileManager>& profile_manager, std::shared_ptr<SettingsManager>& settings_manager,
        std::shared_ptr<MIDISender>& midi_sender);
protected:
    void SetLabelSettings(Label &lblToSet);

private:
    Label title_label_{ "Title", "MIDI2LR" };
    DropShadowEffect title_shadow_;
    Label command_label_{ "Command", "" };
    Label connection_label_{ "Connection", "Not connected to LR" };
    TextButton rescan_button_{ "Rescan MIDI devices" };
    TextButton remove_row_button_{ "Remove selected row" };
    TextButton save_button_{ "Save" };
    TextButton load_button_{ "Load" };
    TextButton settings_button_{ "Settings" };
    Label version_label_{ "Version", "Version " + String{ ProjectInfo::versionString } };
    CommandTable command_table_{ "Table", nullptr };
    CommandTableModel command_table_model_{};
    Label profile_name_label_{ "ProfileNameLabel", "" };
    std::unique_ptr<DialogWindow> settings_dialog_;
    String last_command_;
    int row_to_select_;
    Label current_status_{ "CurrentStatus", "no extra info" };
    std::shared_ptr<CommandMap> command_map_{ nullptr };
    std::shared_ptr<LR_IPC_IN> lr_ipc_in_{ nullptr };
    std::shared_ptr<LR_IPC_OUT> lr_ipc_out_{ nullptr };
    std::shared_ptr<SettingsManager> settings_manager_{ nullptr };
    std::shared_ptr<MIDIProcessor> midi_processor_{ nullptr };
    std::shared_ptr<MIDISender> midi_sender_{ nullptr };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};

#endif  // MAINCOMPONENT_H_INCLUDED
