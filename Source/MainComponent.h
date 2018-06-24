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
#include "CommandTable.h"      //class member
#include "CommandTableModel.h" //class member
#include "ResizableLayout.h"   //base class
class CommandMap;
class LrIpcOut;
class MidiProcessor;
class MidiSender;
class ProfileManager;
class SettingsManager;
namespace rsj {
   struct MidiMessage;
}

class MainContentComponent final : public juce::Component,
                                   juce::AsyncUpdater,
                                   juce::Timer,
                                   juce::Button::Listener,
                                   public ResizableLayout { // ResizableLayout.h
 public:
   MainContentComponent() noexcept;
   ~MainContentComponent() = default;
   MainContentComponent(const MainContentComponent& other) = delete;
   MainContentComponent(MainContentComponent&& other) = delete;
   MainContentComponent& operator=(const MainContentComponent& other) = delete;
   MainContentComponent& operator=(MainContentComponent&& other) = delete;
   void Init(CommandMap* command_map, std::weak_ptr<LrIpcOut>&& lr_ipc_out,
       std::shared_ptr<MidiProcessor> midi_processor, ProfileManager* profile_manager,
       SettingsManager* settings_manager, std::shared_ptr<MidiSender> midi_sender);

 private:
   void SetLabelSettings(juce::Label& label_to_set);
   void paint(juce::Graphics&) override;
   // Button interface
   void buttonClicked(juce::Button* button) override;
   // AsyncUpdater interface
   void handleAsyncUpdate() override;
   // Timer interface
   void timerCallback() override;
   // callbacks
   void LrIpcOutCallback(bool, bool);
   void MidiCmdCallback(rsj::MidiMessage);
   void ProfileChanged(juce::XmlElement* xml_element, const juce::String& file_name);

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
   std::unique_ptr<juce::DialogWindow> settings_dialog_;
   std::weak_ptr<LrIpcOut> lr_ipc_out_;
};

#endif // MAINCOMPONENT_H_INCLUDED
