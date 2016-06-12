/*
  ==============================================================================

  MainComponent.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

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
#include "MIDISender.h"
#include "SettingsComponent.h"

constexpr auto kMainWidth = 400;
constexpr auto kMainHeight = 650;
constexpr auto kMainLeft = 20;
constexpr auto kSpaceBetweenButton = 10;

MainContentComponent::MainContentComponent(): ResizableLayout{this} {}

MainContentComponent::~MainContentComponent() {}

void MainContentComponent::Init(std::shared_ptr<CommandMap>& command_map,
  std::shared_ptr<LR_IPC_IN>& lr_ipc_in,
  std::shared_ptr<LR_IPC_OUT>& lr_ipc_out,
  std::shared_ptr<MIDIProcessor>& midi_processor,
  std::shared_ptr<ProfileManager>& profile_manager,
  std::shared_ptr<SettingsManager>& settings_manager,
  std::shared_ptr<MIDISender>& midi_sender) {
  //copy the pointers
  command_map_ = command_map;
  lr_ipc_in_ = lr_ipc_in;
  lr_ipc_out_ = lr_ipc_out;
  settings_manager_ = settings_manager;
  midi_processor_ = midi_processor;
  midi_sender_ = midi_sender;

  //call the function of the sub component.
  command_table_model_.Init(command_map);

  if (midi_processor) {
      // Add ourselves as a listener for MIDI commands
    midi_processor->addMIDICommandListener(this);
  }

  if (lr_ipc_out_) {
      // Add ourselves as a listener for LR_IPC_OUT events
    lr_ipc_out_->addListener(this);
  }

  if (profile_manager) {
      // Add ourselves as a listener for profile changes
    profile_manager->addListener(this);
  }

  //Set the component size
  setSize(kMainWidth, kMainHeight);

  // Main title
  title_label_.setFont(Font{36.f, Font::bold});
  title_label_.setEditable(false);
  title_label_.setColour(Label::textColourId, Colours::darkgrey);
  title_label_.setComponentEffect(&title_shadow_);
  title_label_.setBounds(kMainLeft, 10, kMainWidth - 2 * kMainLeft, 30);
  addToLayout(&title_label_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(title_label_);

  // Version label
  SetLabelSettings(version_label_);
  version_label_.setBounds(kMainLeft, 40, kMainWidth - 2 * kMainLeft, 10);
  addToLayout(&version_label_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(version_label_);

  // Connection status
  connection_label_.setFont(Font{12.f, Font::bold});
  connection_label_.setEditable(false);
  connection_label_.setColour(Label::backgroundColourId, Colours::red);
  connection_label_.setColour(Label::textColourId, Colours::black);
  connection_label_.setJustificationType(Justification::centred);
  connection_label_.setBounds(200, 15, kMainWidth - kMainLeft - 200, 20);
  addToLayout(&connection_label_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(connection_label_);

  //get the button width
  long button_width = (kMainWidth - 2 * kMainLeft - kSpaceBetweenButton * 2) / 3;

  // Load button
  load_button_.addListener(this);
  load_button_.setBounds(kMainLeft, 60, button_width, 20);
  addToLayout(&load_button_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(load_button_);

  // Save button
  save_button_.addListener(this);
  save_button_.setBounds(kMainLeft + button_width + kSpaceBetweenButton, 60,
    button_width, 20);
  addToLayout(&save_button_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(save_button_);

  // Settings button
  settings_button_.addListener(this);
  settings_button_.setBounds(kMainLeft + button_width * 2 + kSpaceBetweenButton * 2,
    60, button_width, 20);
  addToLayout(&settings_button_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(settings_button_);

  // Command Table
  command_table_.setModel(&command_table_model_);
  command_table_.setBounds(kMainLeft, 100, kMainWidth - kMainLeft * 2,
    kMainHeight - 210);
  addToLayout(&command_table_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(command_table_);

  long label_width = (kMainWidth - kMainLeft * 2) / 2;

  // Profile name label
  SetLabelSettings(profile_name_label_);
  profile_name_label_.setBounds(kMainLeft, kMainHeight - 100, label_width, 20);
  addToLayout(&profile_name_label_, anchorMidLeft, anchorMidRight);
  profile_name_label_.setJustificationType(Justification::centred);
  addAndMakeVisible(profile_name_label_);

  // Last MIDI command
  command_label_.setFont(Font{12.f, Font::bold});
  command_label_.setEditable(false);
  command_label_.setColour(Label::textColourId, Colours::darkgrey);
  command_label_.setBounds(kMainLeft + label_width, kMainHeight - 100,
    label_width, 20);
  addToLayout(&command_label_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(command_label_);

  // Remove row button
  remove_row_button_.addListener(this);
  remove_row_button_.setBounds(kMainLeft, kMainHeight - 75,
    kMainWidth - kMainLeft * 2, 20);
  addToLayout(&remove_row_button_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(remove_row_button_);

  // Rescan MIDI button
  rescan_button_.addListener(this);
  rescan_button_.setBounds(kMainLeft, kMainHeight - 50,
    kMainWidth - kMainLeft * 2, 20);
  addToLayout(&rescan_button_, anchorMidLeft, anchorMidRight);
  addAndMakeVisible(rescan_button_);

  // adding the current status label, used for counting down.
  current_status_.setBounds(kMainLeft, kMainHeight - 30,
    kMainWidth - kMainLeft * 2, 20);
  addToLayout(&current_status_, anchorMidLeft, anchorMidRight);
  current_status_.setJustificationType(Justification::centred);
  SetLabelSettings(current_status_);
  addAndMakeVisible(current_status_);

  if (settings_manager_) {
      // Try to load a default.xml if the user has not set a profile directory
    if (settings_manager_->getProfileDirectory().isEmpty()) {
      File default_profile =
        File::getSpecialLocation(File::currentExecutableFile).getSiblingFile("default.xml");
      std::unique_ptr<XmlElement> xml_element{XmlDocument::parse(default_profile)};
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

void MainContentComponent::paint(Graphics& g) {
  g.fillAll(Colours::white);
}

void MainContentComponent::handleMidiCC(int midi_channel, int controller, int value) {
    // Display the CC parameters and add/highlight row in table corresponding to the CC
  last_command_ = String::formatted("%d: CC%d [%d]", midi_channel, controller, value);
  command_table_model_.addRow(midi_channel, controller, true);
  row_to_select_ = command_table_model_.getRowForMessage(midi_channel, controller, true);
  triggerAsyncUpdate();
}

void MainContentComponent::handleMidiNote(int midi_channel, int note) {
    // Display the Note parameters and add/highlight row in table corresponding to the Note
  last_command_ = String::formatted("%d: Note [%d]", midi_channel, note);
  command_table_model_.addRow(midi_channel, note, false);
  row_to_select_ = command_table_model_.getRowForMessage(midi_channel, note, false);
  triggerAsyncUpdate();
}

void MainContentComponent::connected() {
  connection_label_.setText("Connected to LR", NotificationType::dontSendNotification);
  connection_label_.setColour(Label::backgroundColourId, Colours::greenyellow);
}

void MainContentComponent::disconnected() {
  connection_label_.setText("Not connected to LR", NotificationType::dontSendNotification);
  connection_label_.setColour(Label::backgroundColourId, Colours::red);
}

void MainContentComponent::buttonClicked(Button* button) {
  if (button == &rescan_button_) {
      // Re-enumerate MIDI IN and OUT devices

    if (midi_processor_) {
      midi_processor_->rescanDevices();
    }

    if (midi_sender_) {
      midi_sender_->rescanDevices();
    }
    // Send new CC parameters to MIDI Out devices
    if (lr_ipc_in_) {
      lr_ipc_in_->refreshMIDIOutput();
    }
  }
  else if (button == &remove_row_button_) {
    if (command_table_.getSelectedRow() != -1) {
      command_table_model_.removeRow(command_table_.getSelectedRow());
      command_table_.updateContent();
    }
  }
  else if (button == &save_button_) {
    File profile_directory;

    if (settings_manager_) {
      profile_directory = settings_manager_->getProfileDirectory();
    }

    if (!profile_directory.exists()) {
      profile_directory = File::getCurrentWorkingDirectory();
    }

    WildcardFileFilter wildcard_filter{"*.xml", String::empty, "MIDI2LR profiles"};
    FileBrowserComponent browser{FileBrowserComponent::canSelectFiles |
      FileBrowserComponent::saveMode |
        FileBrowserComponent::warnAboutOverwriting, profile_directory,
      &wildcard_filter, nullptr};
    FileChooserDialogBox dialog_box{"Save profile",
        "Enter filename to save profile",
        browser,
        true,
        Colours::lightgrey};
    if (dialog_box.show()) {
      File selected_file = browser.getSelectedFile(0).withFileExtension("xml");

      if (command_map_) {
        command_map_->toXMLDocument(selected_file);
      }
    }
  }
  else if (button == &load_button_) {
    File profile_directory;

    if (settings_manager_) {
      profile_directory = settings_manager_->getProfileDirectory();
    }

    if (!profile_directory.exists()) {
      profile_directory = File::getCurrentWorkingDirectory();
    }

    WildcardFileFilter wildcard_filter{"*.xml", String::empty, "MIDI2LR profiles"};
    FileBrowserComponent browser{FileBrowserComponent::canSelectFiles |
      FileBrowserComponent::openMode, profile_directory,
      &wildcard_filter, nullptr};
    FileChooserDialogBox dialog_box{"Open profile", "Select a profile to open",
      browser, true, Colours::lightgrey};

    if (dialog_box.show()) {
      std::unique_ptr<XmlElement> xml_element{XmlDocument::parse(browser.getSelectedFile(0))};
      if (xml_element) {
        auto new_profile = browser.getSelectedFile(0);
        auto command = String{"ChangedToFullPath "} +new_profile.getFullPathName() + "\n";

        if (lr_ipc_out_) {
          lr_ipc_out_->sendCommand(command);
        }
        profile_name_label_.setText(new_profile.getFileName(),
          NotificationType::dontSendNotification);
        command_table_model_.buildFromXml(xml_element.get());
        command_table_.updateContent();
        command_table_.repaint();
      }
    }
  }
  else if (button == &settings_button_) {
    DialogWindow::LaunchOptions dialog_options;
    dialog_options.dialogTitle = "Settings";
    //create new object
    auto *component = new SettingsComponent{};
    component->Init(settings_manager_);
    dialog_options.content.setOwned(component);
    dialog_options.content->setSize(400, 300);
    dialog_options.escapeKeyTriggersCloseButton = true;
    dialog_options.useNativeTitleBar = false;
    settings_dialog_.reset(dialog_options.create());
    settings_dialog_->setVisible(true);
  }
}

void MainContentComponent::profileChanged(XmlElement* xml_element, const String& file_name) {
  command_table_model_.buildFromXml(xml_element);
  command_table_.updateContent();
  command_table_.repaint();
  profile_name_label_.setText(file_name, NotificationType::dontSendNotification);
//  _systemTrayComponent.showInfoBubble(filename, "Profile loaded");

    // Send new CC parameters to MIDI Out devices
  if (lr_ipc_in_) {
    lr_ipc_in_->refreshMIDIOutput();
  }
}

void MainContentComponent::SetTimerText(int time_value) {
  if (time_value > 0) {
    current_status_.setText(String::formatted("Hiding in %i Sec.", time_value),
      NotificationType::dontSendNotification);
  }
  else {
    current_status_.setText("", NotificationType::dontSendNotification);
  }
}

void MainContentComponent::SetLabelSettings(Label &label_to_set) {
  label_to_set.setFont(Font{12.f, Font::bold});
  label_to_set.setEditable(false);
  label_to_set.setColour(Label::textColourId, Colours::darkgrey);
}

void MainContentComponent::handleAsyncUpdate() {
    // Update the last command label and set its colour to green
  command_label_.setText(last_command_, NotificationType::dontSendNotification);
  command_label_.setColour(Label::backgroundColourId, Colours::greenyellow);
  startTimer(1000);

  // Update the command table to add and/or select row corresponding to midi command
  command_table_.updateContent();
  command_table_.selectRow(row_to_select_);
}

void MainContentComponent::timerCallback() {
    // reset the command label's background to white
  command_label_.setColour(Label::backgroundColourId, Colours::white);
  stopTimer();
}
