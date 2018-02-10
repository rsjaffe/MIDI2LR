#pragma once
/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/
/*
  ==============================================================================
This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/
#ifndef MIDI2LR_MAINCOMPONENT_H_INCLUDED
#define MIDI2LR_MAINCOMPONENT_H_INCLUDED

#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"
#include "CommandTable.h" //class member
#include "CommandTableModel.h" //class member
#include "ResizableLayout.h" //base class
class CommandMap;
class LrIpcOut;
class MidiProcessor;
class MidiSender;
class ProfileManager;
class SettingsManager;
namespace rsj {
    struct MidiMessage;
}

class MainContentComponent final:
    public juce::Component,
    private juce::AsyncUpdater,
    private juce::Timer,
    private juce::ButtonListener,
    public ResizableLayout { //ResizableLayout.h
public:
    MainContentComponent();
    virtual ~MainContentComponent();
    MainContentComponent(const MainContentComponent&) = delete;
    MainContentComponent& operator=(const MainContentComponent&) = delete;
    void Init(CommandMap* const command_map,
        std::weak_ptr<LrIpcOut>&& out,
        std::shared_ptr<MidiProcessor>& midi_processor,
        ProfileManager* const profile_manager,
        SettingsManager* const settings_manager,
        std::shared_ptr<MidiSender>& midi_sender);

    void ProfileChanged(juce::XmlElement* elem, const juce::String& file_name);

protected:
    void SetLabelSettings(juce::Label& lbl_to_set);

private:
    void paint(juce::Graphics&) override;
    // Button interface
    void buttonClicked(juce::Button* button) override;
    // AsyncUpdater interface
    void handleAsyncUpdate() override;

    // Timer interface
    void timerCallback() override;

    void LrIpcOutCallback(bool, bool);
    void MidiCmdCallback(rsj::MidiMessage);

    CommandMap* command_map_{nullptr};
    CommandTable command_table_{"Table", nullptr};
    CommandTableModel command_table_model_{};
    juce::DropShadowEffect title_shadow_;
    juce::Label command_label_{"Command", ""};
    juce::Label connection_label_{"Connection", "Not connected to LR"};
    juce::Label profile_name_label_{"ProfileNameLabel", ""};
    juce::Label title_label_{"Title", "MIDI2LR"};
    juce::Label version_label_{"Version", "Version " + juce::String{ProjectInfo::versionString}};
    juce::String last_command_;
    juce::TextButton disconnect_button_{"Halt sending to Lightroom"};
    juce::TextButton load_button_{"Load"};
    juce::TextButton remove_row_button_{"Clear ALL rows"};
    juce::TextButton rescan_button_{"Rescan MIDI devices"};
    juce::TextButton save_button_{"Save"};
    juce::TextButton settings_button_{"Settings"};
    SettingsManager* settings_manager_{nullptr};
    size_t row_to_select_{0};
    std::shared_ptr<MidiProcessor> midi_processor_{nullptr};
    std::shared_ptr<MidiSender> midi_sender_{nullptr};
    std::unique_ptr<DialogWindow> settings_dialog_;
    std::weak_ptr<LrIpcOut> lr_ipc_out_;
};

#endif  // MAINCOMPONENT_H_INCLUDED
