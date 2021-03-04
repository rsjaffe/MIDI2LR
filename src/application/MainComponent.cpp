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
#include "MainComponent.h"

#include <exception>
#include <functional>
#include <string>

#include <fmt/format.h>
#include <gsl/gsl>

#include "LR_IPC_Out.h"
#include "MIDIReceiver.h"
#include "MIDISender.h"
#include "MidiUtilities.h"
#include "Misc.h"
#include "Profile.h"
#include "ProfileManager.h"
#include "SettingsComponent.h"
#include "SettingsManager.h"

namespace {
   constexpr int kMainWidth {560}; /* equals CommandTable columns total width plus 60 */
   constexpr int kMainHeight {700};
   constexpr int kMainLeft {20};
   constexpr int kBottomSectionHeight {185};
   constexpr int kSpaceBetweenButton {10};
   constexpr int kStandardHeight {20};
   constexpr int kFullWidth {kMainWidth - kMainLeft * 2};
   constexpr int kButtonWidth {(kFullWidth - kSpaceBetweenButton * 2) / 3};
   constexpr int kButtonXIncrement {kButtonWidth + kSpaceBetweenButton};
   constexpr int kConnectionLabelWidth = {kMainWidth - kMainLeft - 200};
   constexpr int kTopButtonY {60};
   constexpr int kCommandTableY {100};
   constexpr int kFirstButtonX {kMainLeft};
   constexpr int kSecondButtonX {kMainLeft + kButtonXIncrement};
   constexpr int kThirdButtonX {kMainLeft + kButtonXIncrement * 2};
   constexpr int kCommandTableHeight {kMainHeight - kBottomSectionHeight};
   constexpr int kLabelWidth {kFullWidth / 2};
   constexpr int kProfileNameY {kMainHeight - kBottomSectionHeight + 110};
   constexpr int kCommandLabelX {kMainLeft + kLabelWidth};
   constexpr int kCommandLabelY {kProfileNameY};
   constexpr int kBottomButtonY {kMainHeight - kBottomSectionHeight + 135};
   constexpr int kBottomButtonY2 {kMainHeight - kBottomSectionHeight + 160};
   constexpr auto kDefaultsFile {MIDI2LR_UC_LITERAL("default.xml")};
} // namespace

MainContentComponent::MainContentComponent(const CommandSet& command_set, Profile& profile,
    ProfileManager& profile_manager, SettingsManager& settings_manager, LrIpcOut& lr_ipc_out,
    MidiReceiver& midi_receiver, MidiSender& midi_sender)
try : ResizableLayout {
   this
}
, command_table_model_(command_set, profile), lr_ipc_out_ {lr_ipc_out},
    midi_receiver_ {midi_receiver}, midi_sender_ {midi_sender}, profile_(profile),
    profile_manager_(profile_manager), settings_manager_(settings_manager)
{
   setSize(kMainWidth, kMainHeight);
}
catch (const std::exception& e)
{
   MIDI2LR_E_RESPONSE;
   throw;
}

MainContentComponent::~MainContentComponent()
{
   settings_manager_.SetDefaultProfile(profile_name_label_.getText());
}

void MainContentComponent::Init()
{
   try {
      midi_receiver_.AddCallback(this, &MainContentComponent::MidiCmdCallback);
      lr_ipc_out_.AddCallback(this, &MainContentComponent::LrIpcOutCallback);
      profile_manager_.AddCallback(this, &MainContentComponent::ProfileChanged);

      /* Main title */
      StandardLabelSettings(title_label_);
      title_label_.setFont(juce::Font {36.f, juce::Font::bold});
      title_label_.setComponentEffect(&title_shadow_);
      title_label_.setBounds(kMainLeft, 10, kFullWidth, 30);
      addToLayout(&title_label_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(title_label_);

      /* Version label */
      StandardLabelSettings(version_label_);
      version_label_.setBounds(kMainLeft, 40, kFullWidth, 10);
      addToLayout(&version_label_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(version_label_);

      /* Connection status */
      StandardLabelSettings(connection_label_);
      connection_label_.setColour(juce::Label::textColourId, juce::Colours::black);
      connection_label_.setColour(juce::Label::backgroundColourId, juce::Colours::red);
      connection_label_.setJustificationType(juce::Justification::centred);
      connection_label_.setBounds(200, 15, kConnectionLabelWidth, kStandardHeight);
      addToLayout(&connection_label_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(connection_label_);

      /* Load button */
      load_button_.setBounds(kFirstButtonX, kTopButtonY, kButtonWidth, kStandardHeight);
      addToLayout(&load_button_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(load_button_);
      load_button_.onClick = [this] {
         if (profile_.ProfileUnsaved()) {
            const auto result {juce::NativeMessageBox::showYesNoBox(juce::AlertWindow::WarningIcon,
                juce::translate("MIDI2LR profiles"),
                juce::translate("Profile changed. Do you want to save your changes? If you "
                                "continue without saving, your changes will be lost."))};
            if (result)
               SaveProfile();
         }
         juce::File profile_directory {settings_manager_.GetProfileDirectory()};
         const auto directory_saved {profile_directory.exists()};
         if (!directory_saved) [[unlikely]]
            profile_directory = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
         juce::FileChooser chooser {
             juce::translate("Open profile"), profile_directory, "*.xml", true};
         if (chooser.browseForFileToOpen()) {
            if (const auto parsed {juce::parseXML(chooser.getResult())}) {
               const auto new_profile {chooser.getResult()};
               lr_ipc_out_.SendCommand(fmt::format(FMT_STRING("ChangedToFullPath {}\n"),
                   new_profile.getFullPathName().toStdString()));
               profile_name_label_.setText(
                   new_profile.getFileName(), juce::NotificationType::dontSendNotification);
               profile_.FromXml(parsed.get());
               command_table_.updateContent();
               command_table_.repaint();
               if (!directory_saved) [[unlikely]] /* haven't saved a directory yet */
                  settings_manager_.SetProfileDirectory(
                      new_profile.getParentDirectory().getFullPathName());
            }
            else {
               rsj::Log(fmt::format(FMT_STRING("Unable to load profile {}."),
                   chooser.getResult().getFullPathName().toStdString()));
            }
         }
      };

      /* Save button */
      save_button_.setBounds(kSecondButtonX, kTopButtonY, kButtonWidth, kStandardHeight);
      addToLayout(&save_button_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(save_button_);
      save_button_.onClick = [this] { SaveProfile(); };

      /* Settings button */
      settings_button_.setBounds(kThirdButtonX, kTopButtonY, kButtonWidth, kStandardHeight);
      addToLayout(&settings_button_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(settings_button_);
      settings_button_.onClick = [this] {
         juce::DialogWindow::LaunchOptions dialog_options;
         dialog_options.dialogTitle = juce::translate("Settings");
         /* create new object */
         auto component {std::make_unique<SettingsComponent>(settings_manager_)};
         component->Init();
         dialog_options.content.setOwned(component.release());
         dialog_options.content->setSize(400, 300);
         settings_dialog_.reset(dialog_options.create());
         settings_dialog_->setVisible(true);
      };

      /* Command Table */
      command_table_.setModel(&command_table_model_);
      command_table_.setBounds(kMainLeft, kCommandTableY, kFullWidth, kCommandTableHeight);
      addToLayout(&command_table_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(command_table_);

      /* Profile name label */
      StandardLabelSettings(profile_name_label_);
      profile_name_label_.setColour(juce::Label::textColourId, juce::Colours::black);
      profile_name_label_.setBounds(kMainLeft, kProfileNameY, kLabelWidth, kStandardHeight);
      profile_name_label_.setJustificationType(juce::Justification::centred);
      addToLayout(&profile_name_label_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(profile_name_label_);

      /* Last MIDI command */
      StandardLabelSettings(command_label_);
      command_label_.setBounds(kCommandLabelX, kCommandLabelY, kLabelWidth, kStandardHeight);
      addToLayout(&command_label_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(command_label_);

      /* Remove row button */
      remove_row_button_.setBounds(kFirstButtonX, kBottomButtonY, kButtonWidth, kStandardHeight);
      addToLayout(&remove_row_button_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(remove_row_button_);
      remove_row_button_.onClick = [this] {
         if (command_table_.getNumRows() > 0) {
            profile_.RemoveAllRows();
            command_table_.updateContent();
         }
      };

      /* Rescan MIDI button */
      rescan_button_.setBounds(kSecondButtonX, kBottomButtonY, kButtonWidth, kStandardHeight);
      addToLayout(&rescan_button_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(rescan_button_);
      rescan_button_.onClick = [this] {
         /* Re-enumerate MIDI IN and OUT devices */
         midi_receiver_.RescanDevices();
         midi_sender_.RescanDevices();
         /* Send new CC parameters to MIDI Out devices */
         lr_ipc_out_.SendCommand("FullRefresh 1\n");
      };

      /* Disconnect button */
      disconnect_button_.setBounds(kThirdButtonX, kBottomButtonY, kButtonWidth, kStandardHeight);
      disconnect_button_.setClickingTogglesState(true);
      addToLayout(&disconnect_button_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(disconnect_button_);
      disconnect_button_.onClick = [this] {
         if (disconnect_button_.getToggleState()) {
            lr_ipc_out_.SendingStop();
            rsj::Log("Sending halted.");
         }
         else {
            lr_ipc_out_.SendingRestart();
            rsj::Log("Sending restarted.");
         }
      };

      /* Delete unassigned rows */
      remove_unassigned_button_.setBounds(
          kFirstButtonX, kBottomButtonY2, kButtonWidth, kStandardHeight);
      addToLayout(&remove_unassigned_button_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(remove_unassigned_button_);
      remove_unassigned_button_.onClick = [this] {
         profile_.RemoveUnassignedMessages();
         command_table_.updateContent();
      };

      /* Try to load a default.xml if the user has not set a profile directory */
      if (settings_manager_.GetProfileDirectory().isEmpty()) {
         const auto filename {rsj::AppDataFilePath(kDefaultsFile)};
         const auto default_profile {juce::File(filename.data())};
         if (const auto parsed {juce::parseXML(default_profile)}) {
            profile_.FromXml(parsed.get());
            command_table_.updateContent();
         }
      }
      else {
         const auto last_prof {settings_manager_.GetDefaultProfile()};
         if (last_prof != juce::String())
            profile_manager_.SwitchToProfile(last_prof);
         else
            /* otherwise use the last profile from the profile directory */
            profile_manager_.SwitchToProfile(0);
      }

      /* turn it on */
      activateLayout();
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void MainContentComponent::SaveProfile() const
{
   juce::File profile_directory {settings_manager_.GetProfileDirectory()};
   if (!profile_directory.exists())
      profile_directory = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
   juce::FileChooser chooser {juce::translate("Save profile"), profile_directory, "*.xml", true};
   if (chooser.browseForFileToSave(true)) {
      const auto selected_file {chooser.getResult().withFileExtension("xml")};
      profile_.ToXmlFile(selected_file);
   }
}

void MainContentComponent::paint(juce::Graphics& g)
{ //-V2009 overridden method
   try {
      g.fillAll(juce::Colours::white);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void MainContentComponent::MidiCmdCallback(const rsj::MidiMessage& mm)
{
   try {
      /* Display the MIDI parameters and add/highlight row in table corresponding to the message msg
       * is 1-based for channel, which display expects */
      const rsj::MidiMessageId msg {mm};
      last_command_ = fmt::format(FMT_STRING("{}: {}{} [{}]"), msg.channel, mm.message_type_byte,
          msg.control_number, mm.value);
      profile_.AddRowUnmapped(msg);
      row_to_select_ = gsl::narrow_cast<size_t>(profile_.GetRowForMessage(msg));
      triggerAsyncUpdate();
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void MainContentComponent::LrIpcOutCallback(const bool connected, const bool sending_blocked)
{
   try {
      const juce::MessageManagerLock mm_lock; /* as not called in message loop */
      if (connected) {
         if (sending_blocked) {
            connection_label_.setText(
                juce::translate("Sending halted"), juce::NotificationType::dontSendNotification);
            connection_label_.setColour(juce::Label::backgroundColourId, juce::Colours::yellow);
         }
         else {
            connection_label_.setText(juce::translate("Connected to Lightroom"),
                juce::NotificationType::dontSendNotification);
            connection_label_.setColour(
                juce::Label::backgroundColourId, juce::Colours::greenyellow);
         }
      }
      else {
         connection_label_.setText(juce::translate("Not connected to Lightroom"),
             juce::NotificationType::dontSendNotification);
         connection_label_.setColour(juce::Label::backgroundColourId, juce::Colours::red);
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

#pragma warning(suppress : 26461) /* must not change function signature, used as callback */
void MainContentComponent::ProfileChanged(
    juce::XmlElement* xml_element, const juce::String& file_name)
{ //-V2009 overridden method
   try {
      {
         const juce::MessageManagerLock mm_lock;
         profile_.FromXml(xml_element);
         command_table_.updateContent();
         command_table_.repaint();
         profile_name_label_.setText(file_name, juce::NotificationType::dontSendNotification);
      }
      /* Send new CC parameters to MIDI Out devices */
      lr_ipc_out_.SendCommand("FullRefresh 1\n");
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void MainContentComponent::StandardLabelSettings(juce::Label& label_to_set)
{
   try {
      label_to_set.setFont(juce::Font {16.f, juce::Font::bold});
      label_to_set.setEditable(false);
      label_to_set.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void MainContentComponent::handleAsyncUpdate()
{
   try {
      /* Update the last command label and set its color to green */
      command_label_.setText(last_command_, juce::NotificationType::dontSendNotification);
      command_label_.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow);
      startTimer(1000);
      /* Update the command table to add and/or select row corresponding to midi command */
      command_table_.updateContent();
      command_table_.selectRow(gsl::narrow_cast<int>(row_to_select_));
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void MainContentComponent::timerCallback()
{
   try {
      /* reset the command label's background to white */
      command_label_.setColour(juce::Label::backgroundColourId, juce::Colours::white);
      juce::Timer::stopTimer();
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}