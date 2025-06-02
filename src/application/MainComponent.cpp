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
   constexpr int kConnectionLabelWidth {kMainWidth - kMainLeft - 200};
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

try
    : ResizableLayout{this}, command_table_model_(command_set, profile), lr_ipc_out_{lr_ipc_out},
      midi_receiver_{midi_receiver}, midi_sender_{midi_sender}, profile_(profile),
      profile_manager_(profile_manager), settings_manager_(settings_manager) {
   setSize(kMainWidth, kMainHeight);
}

catch (const std::exception& e) {
   rsj::ExceptionResponse(e, std::source_location::current());
   throw;
}

MainContentComponent::~MainContentComponent()
{
   settings_manager_.SetDefaultProfile(profile_name_label_.getText());
   juce::Timer::stopTimer();
}

namespace {
   void StandardLabelSettings(juce::Label& label_to_set)
   {
      label_to_set.setFont(juce::Font {juce::FontOptions(16.F, juce::Font::bold)});
      label_to_set.setEditable(false);
      label_to_set.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
   }
} // namespace

void MainContentComponent::LoadClicked()
{
   try {
      if (profile_.ProfileUnsaved()) {
         if (juce::NativeMessageBox::showYesNoBox(juce::AlertWindow::WarningIcon,
                 juce::translate("MIDI2LR profiles"),
                 juce::translate("Profile changed. Do you want to save your changes? If you "
                                 "continue without saving, your changes will be lost."))) {
            SaveProfile();
         }
      }
      juce::File profile_directory {settings_manager_.GetProfileDirectory()};
      const auto directory_saved {profile_directory.exists()};
      if (!directory_saved) [[unlikely]] {
         profile_directory = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
      }
      juce::FileChooser chooser {juce::translate("Open profile"), profile_directory, "*.xml", true};
      if (chooser.browseForFileToOpen()) {
         if (const auto parsed {juce::parseXML(chooser.getResult())}) {
            const auto new_profile {chooser.getResult()};
            lr_ipc_out_.SendCommand(fmt::format("ChangedToFullPath {}\n",
                new_profile.getFullPathName().toStdString()));
            profile_name_label_.setText(new_profile.getFileName(),
                juce::NotificationType::dontSendNotification);
            profile_.FromXml(parsed.get());
            command_table_.updateContent();
            command_table_.repaint();
            if (!directory_saved) [[unlikely]] { /* haven't saved a directory yet */
               settings_manager_.SetProfileDirectory(
                   new_profile.getParentDirectory().getFullPathName());
            }
         }
         else {
            rsj::Log(fmt::format("Unable to load profile {}.",
                         chooser.getResult().getFullPathName().toStdString()),
                std::source_location::current());
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void MainContentComponent::SettingsClicked()
{
   try {
      juce::DialogWindow::LaunchOptions dialog_options;
      dialog_options.dialogTitle = juce::translate("Settings");
      /* create new object */
      auto component {std::make_unique<SettingsComponent>(settings_manager_)};
      component->Init();
      dialog_options.content.setOwned(component.release());
      dialog_options.content->setSize(400, 300);
      settings_dialog_.reset(dialog_options.create());
      settings_dialog_->setVisible(true);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void MainContentComponent::RemoveAllRowsClicked()
{
   if (command_table_.getNumRows() > 0) {
      profile_.RemoveAllRows();
      command_table_.updateContent();
   }
}

void MainContentComponent::RescanClicked()
{
   /* Re-enumerate MIDI IN and OUT devices */
   midi_receiver_.RescanDevices();
   midi_sender_.RescanDevices();
   /* Send new CC parameters to MIDI Out devices */
   lr_ipc_out_.SendCommand("FullRefresh 1\n");
}

void MainContentComponent::DisconnectClicked()
{
   if (disconnect_button_.getToggleState()) {
      lr_ipc_out_.SendingStop();
      rsj::Log("Sending halted.", std::source_location::current());
   }
   else {
      lr_ipc_out_.SendingRestart();
      rsj::Log("Sending restarted.", std::source_location::current());
   }
}

void MainContentComponent::RemoveUnassignedClicked()
{
   profile_.RemoveUnassignedMessages();
   command_table_.updateContent();
}

void MainContentComponent::AddComponent(juce::Component& component, int x, int y, int width,
    int height)
{
   component.setBounds(x, y, width, height);
   ResizableLayout::addToLayout(&component, anchorMidLeft, anchorMidRight);
   juce::Component::addAndMakeVisible(component);
}

void MainContentComponent::Init()
{
   try {
      midi_receiver_.AddCallback(this, &MainContentComponent::MidiCmdCallback);
      lr_ipc_out_.AddCallback(this, &MainContentComponent::LrIpcOutCallback);
      profile_manager_.AddCallback(this, &MainContentComponent::ProfileChanged);

      /* Main title */
      StandardLabelSettings(title_label_);
      title_label_.setFont(juce::Font {juce::FontOptions(36.F, juce::Font::bold)});
      title_label_.setComponentEffect(&title_shadow_);
      AddComponent(title_label_, kMainLeft, 10, kFullWidth, 30);

      /* Version label */
      StandardLabelSettings(version_label_);
      AddComponent(version_label_, kMainLeft, 40, kFullWidth, 10);

      /* Connection status */
      StandardLabelSettings(connection_label_);
      connection_label_.setColour(juce::Label::textColourId, juce::Colours::black);
      connection_label_.setColour(juce::Label::backgroundColourId, juce::Colours::red);
      connection_label_.setJustificationType(juce::Justification::centred);
      AddComponent(connection_label_, 200, 15, kConnectionLabelWidth, kStandardHeight);

      /* Load button */
      AddComponent(load_button_, kFirstButtonX, kTopButtonY, kButtonWidth, kStandardHeight);
#if __cpp_lib_bind_front >= 202'306L
      load_button_.onClick = std::bind_front<&MainContentComponent::LoadClicked>(this);
#else
      load_button_.onClick = std::bind_front(&MainContentComponent::LoadClicked, this);
#endif

      /* Save button */
      AddComponent(save_button_, kSecondButtonX, kTopButtonY, kButtonWidth, kStandardHeight);
#if __cpp_lib_bind_front >= 202'306L
      save_button_.onClick = std::bind_front<&MainContentComponent::SaveProfile>(this);
#else
      save_button_.onClick = std::bind_front(&MainContentComponent::SaveProfile, this);
#endif

      /* Settings button */
      AddComponent(settings_button_, kThirdButtonX, kTopButtonY, kButtonWidth, kStandardHeight);
#if __cpp_lib_bind_front >= 202'306L
      settings_button_.onClick = std::bind_front<&MainContentComponent::SettingsClicked>(this);
#else
      settings_button_.onClick = std::bind_front(&MainContentComponent::SettingsClicked, this);
#endif

      /* Command Table */
      command_table_.setModel(&command_table_model_);
      AddComponent(command_table_, kMainLeft, kCommandTableY, kFullWidth, kCommandTableHeight);

      /* Profile name label */
      StandardLabelSettings(profile_name_label_);
      profile_name_label_.setColour(juce::Label::textColourId, juce::Colours::black);
      profile_name_label_.setJustificationType(juce::Justification::centred);
      AddComponent(profile_name_label_, kMainLeft, kProfileNameY, kLabelWidth, kStandardHeight);

      /* Last MIDI command */
      StandardLabelSettings(command_label_);
      AddComponent(command_label_, kCommandLabelX, kCommandLabelY, kLabelWidth, kStandardHeight);

      /* Remove row button */
      AddComponent(remove_allrows_button_, kFirstButtonX, kBottomButtonY, kButtonWidth,
          kStandardHeight);
#if __cpp_lib_bind_front >= 202'306L
      remove_allrows_button_.onClick =
          std::bind_front<&MainContentComponent::RemoveAllRowsClicked>(this);
#else
      remove_allrows_button_.onClick =
          std::bind_front(&MainContentComponent::RemoveAllRowsClicked, this);
#endif

      /* Rescan MIDI button */
      AddComponent(rescan_button_, kSecondButtonX, kBottomButtonY, kButtonWidth, kStandardHeight);
#if __cpp_lib_bind_front >= 202'306L
      rescan_button_.onClick = std::bind_front<&MainContentComponent::RescanClicked>(this);
#else
      rescan_button_.onClick = std::bind_front(&MainContentComponent::RescanClicked, this);
#endif

      /* Disconnect button */
      disconnect_button_.setClickingTogglesState(true);
      AddComponent(disconnect_button_, kThirdButtonX, kBottomButtonY, kButtonWidth,
          kStandardHeight);
#if __cpp_lib_bind_front >= 202'306L
      disconnect_button_.onClick = std::bind_front<&MainContentComponent::DisconnectClicked>(this);
#else
      disconnect_button_.onClick = std::bind_front(&MainContentComponent::DisconnectClicked, this);
#endif

      /* Delete unassigned rows */
      AddComponent(remove_unassigned_button_, kFirstButtonX, kBottomButtonY2, kButtonWidth,
          kStandardHeight);
#if __cpp_lib_bind_front >= 202'306L
      remove_unassigned_button_.onClick =
          std::bind_front<&MainContentComponent::RemoveUnassignedClicked>(this);
#else
      remove_unassigned_button_.onClick =
          std::bind_front(&MainContentComponent::RemoveUnassignedClicked, this);
#endif

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
         if (last_prof != juce::String()) { profile_manager_.SwitchToProfile(last_prof); }
         else { /* otherwise use the last profile from the profile directory */
            profile_manager_.SwitchToProfile(0);
         }
      }

      /* turn it on */
      ResizableLayout::activateLayout();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void MainContentComponent::SaveProfile() const
{
   juce::File profile_directory {settings_manager_.GetProfileDirectory()};
   if (!profile_directory.exists()) {
      profile_directory = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
   }
   juce::FileChooser chooser {juce::translate("Save profile"), profile_directory, "*.xml", true};
   if (chooser.browseForFileToSave(true)) {
      const auto selected_file {chooser.getResult().withFileExtension("xml")};
      profile_.ToXmlFile(selected_file);
   }
}

void MainContentComponent::paint(juce::Graphics& g)
{ //-V2009 overridden method
   g.fillAll(juce::Colours::white);
}

void MainContentComponent::MidiCmdCallback(rsj::MidiMessage mm)
{
   try {
      /* Display the MIDI parameters and add/highlight row in table corresponding to the message.
       * msg is 1-based for channel, which display expects */
      const rsj::MidiMessageId msg {mm};
      last_command_ = fmt::format("{}: {}{} [{}]", msg.channel, mm.message_type_byte,
          msg.control_number, mm.value);
      profile_.InsertUnassigned(msg);
      row_to_select_ = gsl::narrow_cast<size_t>(profile_.GetRowForMessage(msg));
      triggerAsyncUpdate();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void MainContentComponent::UpdateConnectionLabel(const char* text, const juce::Colour colour)
{
   connection_label_.setText(juce::translate(text), juce::NotificationType::dontSendNotification);
   connection_label_.setColour(juce::Label::backgroundColourId, colour);
}

namespace {
   const std::map<std::pair<bool, bool>, std::pair<const char*, juce::Colour>> kConnectionStatus {
       { {true, false}, {"Connected to Lightroom", juce::Colours::greenyellow}},
       {  {true, true},              {"Sending halted", juce::Colours::yellow}},
       {{false, false},     {"Not connected to Lightroom", juce::Colours::red}},
       { {false, true},     {"Not connected to Lightroom", juce::Colours::red}}
   };
} // namespace

void MainContentComponent::LrIpcOutCallback(const bool connected, const bool sending_blocked)
{
   const juce::MessageManagerLock mm_lock; /* as not called in message loop */
   const auto& [text, color] {kConnectionStatus.at({connected, sending_blocked})};
   UpdateConnectionLabel(text, color);
}

#pragma warning(suppress : 26461) /* must not change function signature, used as callback */

void MainContentComponent::ProfileChanged(juce::XmlElement* xml_element,
    const juce::String& file_name) //-V3536
{                                  //-V2009 overridden method
   try {
      {
         const juce::MessageManagerLock mm_lock;
         profile_.FromXml(xml_element);
         command_table_.updateContent();
         profile_name_label_.setText(file_name, juce::NotificationType::dontSendNotification);
      }
      /* Send new CC parameters to MIDI Out devices */
      lr_ipc_out_.SendCommand("FullRefresh 1\n");
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void MainContentComponent::handleAsyncUpdate()
{
   try {
      /* Update the last command label and set its color to green */
      command_label_.setText(last_command_, juce::NotificationType::dontSendNotification);
      command_label_.setColour(juce::Label::backgroundColourId, juce::Colours::greenyellow);
      juce::Timer::startTimer(1000);
      /* Update the command table to add and/or select row corresponding to midi command */
      command_table_.updateContent();
      command_table_.selectRow(gsl::narrow_cast<int>(row_to_select_));
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void MainContentComponent::timerCallback()
{
   /* reset the command label's background to white */
   command_label_.setColour(juce::Label::backgroundColourId, juce::Colours::white);
   juce::Timer::stopTimer();
}