#pragma once
/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/
/*
  ==============================================================================
This file is part of MIDI2LR. Copyright 2015-2017 by Rory Jaffe.

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
#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"
#include "CommandTable.h" //class member
#include "CommandTableModel.h" //class member
#include "LR_IPC_Out.h" //base class
#include "MIDIProcessor.h" //base class
#include "MidiUtilities.h"
#include "ProfileManager.h" //base class
#include "ResizableLayout.h" //base class

class CommandMap;
class MIDISender;
class SettingsManager;

class MainContentComponent final:
  public juce::Component,
  public LRConnectionListener, //LR_IPC_OUT.h
  private juce::AsyncUpdater,
  private juce::Timer,
  private juce::ButtonListener,
  public ProfileChangeListener, //ProfileManager.h
  public ResizableLayout { //ResizableLayout.h
public:
  MainContentComponent();
  virtual ~MainContentComponent();
  void Init(CommandMap* command_map,
    std::weak_ptr<LR_IPC_OUT>&& out,
    std::shared_ptr<MIDIProcessor>& midi_processor,
    ProfileManager* profile_manager,
    SettingsManager* settings_manager,
    std::shared_ptr<MIDISender>& midi_sender);

  // MIDICommandListener interface
  virtual void handleMIDI(RSJ::Message);

  // LRConnectionListener interface
  virtual void connected() override;
  virtual void disconnected() override;

  // ProfileChangeListener interface
  virtual void profileChanged(juce::XmlElement* elem, const juce::String& file_name) override;
  void SetTimerText(int time_value);

protected:
  void SetLabelSettings(juce::Label& lblToSet);

private:
  void paint(juce::Graphics&) override;
  // Button interface
  virtual void buttonClicked(juce::Button* button) override;
  // AsyncUpdater interface
  virtual void handleAsyncUpdate() override;

  // Timer interface
  virtual void timerCallback() override;

  CommandTable command_table_{"Table", nullptr};
  CommandTableModel command_table_model_{};
  juce::DropShadowEffect title_shadow_;
  size_t row_to_select_{0};
  juce::Label command_label_{"Command", ""};
  juce::Label connection_label_{"Connection", "Not connected to LR"};
  juce::Label current_status_{"CurrentStatus", "no extra info"};
  juce::Label profile_name_label_{"ProfileNameLabel", ""};
  juce::Label title_label_{"Title", "MIDI2LR"};
  juce::Label version_label_{"Version", "Version " + juce::String{ ProjectInfo::versionString }};
  CommandMap* command_map_{nullptr};
  std::weak_ptr<LR_IPC_OUT> lr_ipc_out_;
  std::shared_ptr<MIDIProcessor> midi_processor_{nullptr};
  std::shared_ptr<MIDISender> midi_sender_{nullptr};
  SettingsManager* settings_manager_{nullptr};
  std::unique_ptr<DialogWindow> settings_dialog_;
  juce::String last_command_;
  juce::TextButton load_button_{"Load"};
  juce::TextButton remove_row_button_{"Clear ALL rows"};
  juce::TextButton rescan_button_{"Rescan MIDI devices"};
  juce::TextButton save_button_{"Save"};
  juce::TextButton settings_button_{"Settings"};

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};

#endif  // MAINCOMPONENT_H_INCLUDED
