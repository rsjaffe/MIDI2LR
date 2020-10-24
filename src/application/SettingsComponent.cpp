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
#include "SettingsComponent.h"

#include <exception>

#include <fmt/format.h>

#include "Misc.h"
#include "SettingsManager.h"

namespace {
   constexpr auto kSettingsLeft {20};
   constexpr auto kSettingsWidth {400};
   constexpr auto kSettingsHeight {300};
} // namespace

SettingsComponent::SettingsComponent(SettingsManager& settings_manager)
    : ResizableLayout {this}, settings_manager_ {settings_manager}
{
}

void SettingsComponent::Init()
{
   try {
      setSize(kSettingsWidth, kSettingsHeight);
      pickup_group_.setText(juce::translate("Pick up"));
      pickup_group_.setBounds(0, 0, kSettingsWidth, 100);
      addToLayout(&pickup_group_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(pickup_group_);
      pickup_label_.setFont(juce::Font {16.f, juce::Font::bold});
      pickup_label_.setText(juce::translate("Disabling the pickup mode may be better for "
                                            "touchscreen interfaces and may solve issues with "
                                            "Lightroom not picking up fast fader/knob movements"),
          juce::NotificationType::dontSendNotification);
      pickup_label_.setBounds(kSettingsLeft, 15, kSettingsWidth - 2 * kSettingsLeft, 50);
      addToLayout(&pickup_label_, anchorMidLeft, anchorMidRight);
      pickup_label_.setEditable(false);
      pickup_label_.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
      addAndMakeVisible(pickup_label_);

      pickup_enabled_.setToggleState(
          settings_manager_.GetPickupEnabled(), juce::NotificationType::dontSendNotification);
      pickup_enabled_.setBounds(kSettingsLeft, 60, kSettingsWidth - 2 * kSettingsLeft,
          32); //-V112
      addToLayout(&pickup_enabled_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(pickup_enabled_);
      pickup_enabled_.onClick = [this] {
         const auto pickup_state {pickup_enabled_.getToggleState()};
         settings_manager_.SetPickupEnabled(pickup_state);
         rsj::Log(pickup_state ? "Pickup set to enabled." : "Pickup set to disabled.");
      };

      /* profile */
      profile_group_.setText(juce::translate("Profile"));
      profile_group_.setBounds(0, 100, kSettingsWidth, 100);
      addToLayout(&profile_group_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(profile_group_);

      profile_location_button_.setBounds(
          kSettingsLeft, 120, kSettingsWidth - 2 * kSettingsLeft, 25);
      addToLayout(&profile_location_button_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(profile_location_button_);
      profile_location_button_.onClick = [this] {
         juce::FileChooser chooser {juce::translate("Select Folder"),
             juce::File::getSpecialLocation(juce::File::userDocumentsDirectory), "", true};
         if (chooser.browseForDirectory()) {
            const auto profile_location {chooser.getResult().getFullPathName()};
            settings_manager_.SetProfileDirectory(profile_location);
            rsj::Log(fmt::format(
                FMT_STRING("Profile location set to {}."), profile_location.toStdString()));
            profile_location_label_.setText(
                profile_location, juce::NotificationType::dontSendNotification);
         }
      };

      profile_location_label_.setEditable(false);
      profile_location_label_.setBounds(kSettingsLeft, 145, kSettingsWidth - 2 * kSettingsLeft, 30);
      addToLayout(&profile_location_label_, anchorMidLeft, anchorMidRight);
      profile_location_label_.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
      addAndMakeVisible(profile_location_label_);
      profile_location_label_.setText(
          settings_manager_.GetProfileDirectory(), juce::NotificationType::dontSendNotification);

      /* autohide */
      autohide_group_.setText(juce::translate("Auto hide"));
      autohide_group_.setBounds(0, 200, kSettingsWidth, 100);
      addToLayout(&autohide_group_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(autohide_group_);

      autohide_explain_label_.setFont(juce::Font {16.f, juce::Font::bold});
      autohide_explain_label_.setText(juce::translate("Autohide the plugin window in x seconds, "
                                                      "select 0 for disabling autohide"),
          juce::NotificationType::dontSendNotification);
      autohide_explain_label_.setBounds(kSettingsLeft, 215, kSettingsWidth - 2 * kSettingsLeft, 50);
      addToLayout(&autohide_explain_label_, anchorMidLeft, anchorMidRight);
      autohide_explain_label_.setEditable(false);
      autohide_explain_label_.setFont(juce::Font {16.f, juce::Font::bold});
      autohide_explain_label_.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
      addAndMakeVisible(autohide_explain_label_);

      autohide_setting_.setBounds(kSettingsLeft, 245, kSettingsWidth - 2 * kSettingsLeft, 50);
      autohide_setting_.setRange(0, 10, 1);
      autohide_setting_.setValue(
          settings_manager_.GetAutoHideTime(), juce::NotificationType::dontSendNotification);

      addToLayout(&autohide_setting_, anchorMidLeft, anchorMidRight);
      addAndMakeVisible(autohide_setting_);
      autohide_setting_.onValueChange = [this] {
         settings_manager_.SetAutoHideTime(rsj::RoundToInt(autohide_setting_.getValue()));
         rsj::Log(fmt::format(
             FMT_STRING("Autohide time set to {} seconds."), settings_manager_.GetAutoHideTime()));
      };
      /* turn it on */
      activateLayout();
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void SettingsComponent::paint(juce::Graphics& g)
{ //-V2009 overridden method
   try {
      g.fillAll(juce::Colours::white); /* clear the background */
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}