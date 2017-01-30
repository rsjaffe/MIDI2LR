// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
  ==============================================================================

  MainComponent.cpp

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
#include "MainComponent.h"
#include <string>
#include <utility>
#include "MIDISender.h"
#include "SettingsComponent.h"

namespace {
  constexpr int kMainWidth = 400;
  constexpr int kMainHeight = 650;
  constexpr int kMainLeft = 20;
  constexpr int kSpaceBetweenButton = 10;
  constexpr int kStandardHeight = 20;
  constexpr int kFullWidth = kMainWidth - kMainLeft * 2;
  constexpr int kButtonWidth = (kFullWidth - kSpaceBetweenButton * 2) / 3;
  constexpr int kButtonXIncrement = kButtonWidth + kSpaceBetweenButton;
  constexpr int kConnectionLabelWidth = kMainWidth - kMainLeft - 200;
  constexpr int kFirstButtonX = kMainLeft;
  constexpr int kSecondButtonX = kMainLeft + kButtonXIncrement;
  constexpr int kThirdButtonX = kMainLeft + kButtonXIncrement * 2;
  constexpr int kCommandTableHeight = kMainHeight - 210;
  constexpr int kLabelWidth = kFullWidth / 2;
  constexpr int kProfileNameY = kMainHeight - 100;
  constexpr int kCommandLabelX = kMainLeft + kLabelWidth;
  constexpr int kCommandLabelY = kMainHeight - 100;
  constexpr int kRemoveRowY = kMainHeight - 75;
  constexpr int kRescanY = kMainHeight - 50;
  constexpr int kCurrentStatusY = kMainHeight - 30;
}

MainContentComponent::MainContentComponent(): ResizableLayout{this} {}

MainContentComponent::~MainContentComponent() {}

void MainContentComponent::Init(std::shared_ptr<CommandMap>& command_map,
  std::weak_ptr<LR_IPC_IN>&& lr_ipc_in,
  std::weak_ptr<LR_IPC_OUT>&& lr_ipc_out,
  std::shared_ptr<MIDIProcessor>& midi_processor, //-V2009
  std::shared_ptr<ProfileManager>& profile_manager,
  std::shared_ptr<SettingsManager>& settings_manager,
  std::shared_ptr<MIDISender>& midi_sender) {
  //copy the pointers
  command_map_ = command_map;
  lr_ipc_in_ = std::move(lr_ipc_in);
  lr_ipc_out_ = std::move(lr_ipc_out);
  settings_manager_ = settings_manager;
  midi_processor_ = midi_processor;
  midi_sender_ = midi_sender;

  //call the function of the sub component.
  command_table_model_.Init(command_map);

  if (midi_processor) {
      // Add ourselves as a listener for MIDI commands
    midi_processor->addMIDICommandListener(this);
  }

  if (const auto ptr = lr_ipc_out_.lock()) {
      // Add ourselves as a listener for LR_IPC_OUT events
    ptr->addListener(this);
  }

  if (profile_manager) {
      // Add ourselves as a listener for profile changes
    profile_manager->addListener(this);
  }

  //Set the component size
  setSize(kMainWidth, kMainHeight);

  // Main title
  title_label_.setFont(juce::Font{36.f, juce::Font::bold});
  title_label_.setEditable(false);
  title_label_.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
  title_label_.setComponentEffect(&title_shadow_);
  title_label_.setBounds(kMainLeft, 10, kFullWidth, 30);
  addToLayout(&title_label_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(title_label_);

  // Version label
  SetLabelSettings(version_label_);
  version_label_.setBounds(kMainLeft, 40, kFullWidth, 10);
  addToLayout(&version_label_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(version_label_);

  // Connection status
  connection_label_.setFont(juce::Font{12.f, juce::Font::bold});
  connection_label_.setEditable(false);
  connection_label_.setColour(juce::Label::backgroundColourId, juce::Colours::red);
  connection_label_.setColour(juce::Label::textColourId, juce::Colours::black);
  connection_label_.setJustificationType(juce::Justification::centred);
  connection_label_.setBounds(200, 15, kConnectionLabelWidth, kStandardHeight);
  addToLayout(&connection_label_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(connection_label_);

  // Load button
  load_button_.addListener(this);
  load_button_.setBounds(kFirstButtonX, 60, kButtonWidth, kStandardHeight);
  addToLayout(&load_button_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(load_button_);

  // Save button
  save_button_.addListener(this);
  save_button_.setBounds(kSecondButtonX, 60, kButtonWidth, kStandardHeight);
  addToLayout(&save_button_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(save_button_);

  // Settings button
  settings_button_.addListener(this);
  settings_button_.setBounds(kThirdButtonX, 60, kButtonWidth, kStandardHeight);
  addToLayout(&settings_button_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(settings_button_);

  // Command Table
  command_table_.setModel(&command_table_model_);
  command_table_.setBounds(kMainLeft, 100, kFullWidth, kCommandTableHeight);
  addToLayout(&command_table_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(command_table_);

  // Profile name label
  SetLabelSettings(profile_name_label_);
  profile_name_label_.setBounds(kMainLeft, kProfileNameY, kLabelWidth, kStandardHeight);
  addToLayout(&profile_name_label_, anchorMidLeft, anchorMidRight);
  profile_name_label_.setJustificationType(juce::Justification::centred);
  addAndMakeVisible(profile_name_label_);

  // Last MIDI command
  command_label_.setFont(juce::Font{12.f, juce::Font::bold});
  command_label_.setEditable(false);
  command_label_.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
  command_label_.setBounds(kCommandLabelX, kCommandLabelY, kLabelWidth, kStandardHeight);
  addToLayout(&command_label_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(command_label_);

  // Remove row button
  remove_row_button_.addListener(this);
  remove_row_button_.setBounds(kMainLeft, kRemoveRowY, kFullWidth, kStandardHeight);
  addToLayout(&remove_row_button_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(remove_row_button_);

  // Rescan MIDI button
  rescan_button_.addListener(this);
  rescan_button_.setBounds(kMainLeft, kRescanY, kFullWidth, kStandardHeight);
  addToLayout(&rescan_button_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(rescan_button_);

  // adding the current status label, used for counting down.
  current_status_.setBounds(kMainLeft, kCurrentStatusY, kFullWidth, kStandardHeight);
  addToLayout(&current_status_, anchorMidLeft, anchorMidRight);
  current_status_.setJustificationType(juce::Justification::centred);
  SetLabelSettings(current_status_);
  addAndMakeVisible(current_status_);

  if (settings_manager_) {
      // Try to load a default.xml if the user has not set a profile directory
    if (settings_manager_->getProfileDirectory().isEmpty()) {
      juce::File default_profile =
        juce::File::getSpecialLocation(juce::File::currentExecutableFile).getSiblingFile("default.xml");
      std::unique_ptr<juce::XmlElement> xml_element{juce::XmlDocument::parse(default_profile)};
      if (xml_element) {
        command_table_model_.buildFromXml(xml_element.get());
        command_table_.updateContent();
      }
    }
    else if (profile_manager) {
        // otherwise use the last profile from the profile directory
      profile_manager->switchToProfile(0);
    }
  }
  // turn it on
  activateLayout();
}

void MainContentComponent::paint(juce::Graphics& g) { //-V2009 overridden method
  g.fillAll(juce::Colours::white);
}

void MainContentComponent::handleMIDI(RSJ::Message mm) {
    // Display the CC parameters and add/highlight row in table corresponding to the CC
  MessageType mt;
  switch (mm.MessageType) {//this is needed because mapping uses custom structure
    case RSJ::kCCFlag:
      mt = CC;
      break;
    case RSJ::kNoteOnFlag:
      mt = NOTE;
      break;
    case RSJ::kPWFlag:
      mt = PITCHBEND;
      break;
    default: //shouldn't receive any messages note categorized above
      assert(0);
      mt = CC;
  }
  mm.Channel++; //used to 1-based channel numbers
  last_command_ = juce::String::formatted("%d: CC%d [%d]", mm.Channel, mm.Number, mm.Value);
  command_table_model_.addRow(mm.Channel, mm.Number, mt);
  row_to_select_ = static_cast<size_t>(command_table_model_.getRowForMessage(mm.Channel, mm.Number, mt));
  triggerAsyncUpdate();
}

void MainContentComponent::connected() {
  connection_label_.setText("Connected to LR", juce::NotificationType::dontSendNotification);
  connection_label_.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow);
}

void MainContentComponent::disconnected() {
  connection_label_.setText("Not connected to LR", juce::NotificationType::dontSendNotification);
  connection_label_.setColour(juce::Label::backgroundColourId, juce::Colours::red);
}

void MainContentComponent::buttonClicked(juce::Button* button) { //-V2009 overridden method
  if (button == &rescan_button_) {
      // Re-enumerate MIDI IN and OUT devices

    if (midi_processor_) {
      midi_processor_->RescanDevices();
    }

    if (midi_sender_) {
      midi_sender_->RescanDevices();
    }
    // Send new CC parameters to MIDI Out devices
    if (const auto ptr = lr_ipc_out_.lock()) {
      ptr->sendCommand("FullRefresh 1\n");
    }
  }
  else if (button == &remove_row_button_) {
    if (command_table_.getNumRows() > 0) {
      command_table_model_.removeAllRows();
      //command_table_model_.removeRow(static_cast<size_t>(command_table_.getSelectedRow()));
      command_table_.updateContent();
    }
  }
  else if (button == &save_button_) {
    juce::File profile_directory;

    if (settings_manager_) {
      profile_directory = settings_manager_->getProfileDirectory();
    }

    if (!profile_directory.exists()) {
      profile_directory = juce::File::getCurrentWorkingDirectory();
    }

    juce::WildcardFileFilter wildcard_filter{"*.xml", juce::String::empty, "MIDI2LR profiles"};
    juce::FileBrowserComponent browser{juce::FileBrowserComponent::canSelectFiles |
      juce::FileBrowserComponent::saveMode |
      juce::FileBrowserComponent::warnAboutOverwriting, profile_directory,
      &wildcard_filter, nullptr};
    juce::FileChooserDialogBox dialog_box{"Save profile",
        "Enter filename to save profile",
        browser,
        true,
      juce::Colours::lightgrey};
    if (dialog_box.show()) {
      juce::File selected_file = browser.getSelectedFile(0).withFileExtension("xml");

      if (command_map_) {
        command_map_->toXMLDocument(selected_file);
      }
    }
  }
  else if (button == &load_button_) {
    juce::File profile_directory;

    if (settings_manager_) {
      profile_directory = settings_manager_->getProfileDirectory();
    }

    if (!profile_directory.exists()) {
      profile_directory = juce::File::getCurrentWorkingDirectory();
    }

    juce::WildcardFileFilter wildcard_filter{"*.xml", juce::String::empty, "MIDI2LR profiles"};
    juce::FileBrowserComponent browser{juce::FileBrowserComponent::canSelectFiles |
      juce::FileBrowserComponent::openMode, profile_directory,
      &wildcard_filter, nullptr};
    juce::FileChooserDialogBox dialog_box{"Open profile", "Select a profile to open",
      browser, true, juce::Colours::lightgrey};

    if (dialog_box.show()) {
      std::unique_ptr<juce::XmlElement> xml_element{juce::XmlDocument::parse(browser.getSelectedFile(0))};
      if (xml_element) {
        const auto new_profile = browser.getSelectedFile(0);
        const std::string command = "ChangedToFullPath " + new_profile.getFullPathName().toStdString() + '\n';

        if (const auto ptr = lr_ipc_out_.lock()) {
          ptr->sendCommand(command);
        }
        profile_name_label_.setText(new_profile.getFileName(),
          juce::NotificationType::dontSendNotification);
        command_table_model_.buildFromXml(xml_element.get());
        command_table_.updateContent();
        command_table_.repaint();
      }
    }
  }
  else if (button == &settings_button_) {
    juce::DialogWindow::LaunchOptions dialog_options;
    dialog_options.dialogTitle = "Settings";
    //create new object
    auto* const component = new SettingsComponent{};
    component->Init(settings_manager_);
    dialog_options.content.setOwned(component);
    dialog_options.content->setSize(400, 300);
    dialog_options.escapeKeyTriggersCloseButton = true;
    dialog_options.useNativeTitleBar = false;
    settings_dialog_.reset(dialog_options.create());
    settings_dialog_->setVisible(true);
  }
}

void MainContentComponent::profileChanged(juce::XmlElement* xml_element, const juce::String& file_name) { //-V2009 overridden method
  command_table_model_.buildFromXml(xml_element);
  command_table_.updateContent();
  command_table_.repaint();
  profile_name_label_.setText(file_name, NotificationType::dontSendNotification);
//  _systemTrayComponent.showInfoBubble(filename, "Profile loaded");

    // Send new CC parameters to MIDI Out devices
  if (const auto ptr = lr_ipc_out_.lock()) {
    ptr->sendCommand("FullRefresh 1\n");
  }
}

void MainContentComponent::SetTimerText(int time_value) {
  if (time_value > 0) {
    current_status_.setText(juce::String::formatted("Hiding in %i Sec.", time_value),
      juce::NotificationType::dontSendNotification);
  }
  else {
    current_status_.setText("", juce::NotificationType::dontSendNotification);
  }
}

void MainContentComponent::SetLabelSettings(juce::Label& label_to_set) {
  label_to_set.setFont(juce::Font{12.f, juce::Font::bold});
  label_to_set.setEditable(false);
  label_to_set.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
}

void MainContentComponent::handleAsyncUpdate() {
    // Update the last command label and set its colour to green
  command_label_.setText(last_command_, juce::NotificationType::dontSendNotification);
  command_label_.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow);
  startTimer(1000);

  // Update the command table to add and/or select row corresponding to midi command
  command_table_.updateContent();
  command_table_.selectRow(static_cast<int>(row_to_select_));
}

void MainContentComponent::timerCallback() {
    // reset the command label's background to white
  command_label_.setColour(juce::Label::backgroundColourId, juce::Colours::white);
  juce::Timer::stopTimer();
}