#ifndef MIDI2LR_MAINCOMPONENT_H_INCLUDED
#define MIDI2LR_MAINCOMPONENT_H_INCLUDED
/*
 * This file is part of MIDI2LR. Copyright (C) 2015 by Rory Jaffe.
 *
 * MIDI2LR is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with MIDI2LR.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include <memory>

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "CommandTable.h"
#include "CommandTableModel.h"
#include "falco/ResizableLayout.h"
class CommandSet;
class LrIpcOut;
class MidiReceiver;
class MidiSender;
class Profile;
class ProfileManager;
class SettingsManager;

namespace rsj {
   struct MidiMessage;
} // namespace rsj

class MainContentComponent final :
    public juce::Component,
    juce::AsyncUpdater,
    juce::Timer,
    public ResizableLayout {
 public:
   MainContentComponent(const CommandSet& command_set, Profile& profile,
       ProfileManager& profile_manager, SettingsManager& settings_manager, LrIpcOut& lr_ipc_out,
       MidiReceiver& midi_receiver, MidiSender& midi_sender);
   ~MainContentComponent(); // NOLINT(modernize-use-override)
   MainContentComponent(const MainContentComponent& other) = delete;
   MainContentComponent(MainContentComponent&& other) = delete;
   MainContentComponent& operator=(const MainContentComponent& other) = delete;
   MainContentComponent& operator=(MainContentComponent&& other) = delete;
   void Init();
   void SaveProfile() const;

 private:
   void handleAsyncUpdate() override;
   void LrIpcOutCallback(bool, bool);
   void MidiCmdCallback(rsj::MidiMessage mm);
   void paint(juce::Graphics&) override;
   void ProfileChanged(juce::XmlElement* xml_element, const juce::String& file_name);
   void StandardLabelSettings(juce::Label& label_to_set);
   void timerCallback() override;

   CommandTable command_table_ {"Table", nullptr};
   CommandTableModel command_table_model_;
   juce::DropShadowEffect title_shadow_;
   juce::Label command_label_ {"Command", ""};
   juce::Label connection_label_ {"Connection", juce::translate("Not connected to Lightroom")};
   juce::Label profile_name_label_ {"ProfileNameLabel", ""};
   juce::Label title_label_ {"Title", "MIDI2LR"};
   juce::Label version_label_ {
       "Version", juce::translate("Version ") + juce::String {ProjectInfo::versionString}};
   juce::String last_command_;
   juce::TextButton disconnect_button_ {juce::translate("Halt sending to Lightroom")};
   juce::TextButton load_button_ {juce::translate("Load")};
   juce::TextButton remove_row_button_ {juce::translate("Clear ALL rows")};
   juce::TextButton remove_unassigned_button_ {juce::translate("Remove unassigned rows")};
   juce::TextButton rescan_button_ {juce::translate("Rescan MIDI devices")};
   juce::TextButton save_button_ {juce::translate("Save")};
   juce::TextButton settings_button_ {juce::translate("Settings")};
   LrIpcOut& lr_ipc_out_;
   MidiReceiver& midi_receiver_;
   MidiSender& midi_sender_;
   Profile& profile_;
   ProfileManager& profile_manager_;
   SettingsManager& settings_manager_;
   size_t row_to_select_ {0};
   std::unique_ptr<juce::DialogWindow> settings_dialog_;
};

#endif
