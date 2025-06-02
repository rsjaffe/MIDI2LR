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

#include <cmath>
#include <exception>

#include <fmt/format.h>
#include <gsl/gsl>

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

namespace {
   void StandardLabelSettings(juce::Label& label_to_set)
   {
      label_to_set.setFont(juce::Font {juce::FontOptions(16.F, juce::Font::bold)});
      label_to_set.setEditable(false);
      label_to_set.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
   }
} // namespace

void SettingsComponent::AddComponent(juce::Component& component, int x, int y, int width,
    int height)
{
   component.setBounds(x, y, width, height);
   ResizableLayout::addToLayout(&component, anchorMidLeft, anchorMidRight);
   juce::Component::addAndMakeVisible(component);
}

void SettingsComponent::PickupClicked()
{
   const auto pickup_state {pickup_enabled_.getToggleState()};
   settings_manager_.SetPickupEnabled(pickup_state);
   rsj::Log(pickup_state ? "Pickup set to enabled." : "Pickup set to disabled.",
       std::source_location::current());
}

void SettingsComponent::ProfileClicked()
{
   juce::FileChooser chooser {juce::translate("Select Folder"),
       juce::File::getSpecialLocation(juce::File::userDocumentsDirectory), "", true};
   if (chooser.browseForDirectory()) {
      const auto profile_location {chooser.getResult().getFullPathName()};
      settings_manager_.SetProfileDirectory(profile_location);
      rsj::Log(fmt::format("Profile location set to {}.", profile_location.toStdString()),
          std::source_location::current());
      profile_location_label_.setText(profile_location,
          juce::NotificationType::dontSendNotification);
   }
}

void SettingsComponent::AutohideChanged()
{
   settings_manager_.SetAutoHideTime(gsl::narrow<int>(std::lrint(autohide_setting_.getValue())));
   rsj::Log(fmt::format("Autohide time set to {} seconds.", settings_manager_.GetAutoHideTime()),
       std::source_location::current());
}

void SettingsComponent::Init()
{
   try {
      juce::Component::setSize(kSettingsWidth, kSettingsHeight);

      /* pickup_group_ */
      pickup_group_.setText(juce::translate("Pick up"));
      AddComponent(pickup_group_, 0, 0, kSettingsWidth, 100);
      /* pickup_label_ */
      pickup_label_.setText(juce::translate("Disabling the pickup mode may be better for "
                                            "touchscreen interfaces and may solve issues with "
                                            "Lightroom not picking up fast fader/knob movements"),
          juce::NotificationType::dontSendNotification);
      StandardLabelSettings(pickup_label_);
      AddComponent(pickup_label_, kSettingsLeft, 15, kSettingsWidth - 2 * kSettingsLeft, 50);
      /* pickup_enabled_ */
      pickup_enabled_.setToggleState(settings_manager_.GetPickupEnabled(),
          juce::NotificationType::dontSendNotification);
      AddComponent(pickup_enabled_, kSettingsLeft, 60, kSettingsWidth - 2 * kSettingsLeft, 32);
#if __cpp_lib_bind_front >= 202'306L
      pickup_enabled_.onClick = std::bind_front<&SettingsComponent::PickupClicked>(this);
#else
      pickup_enabled_.onClick = std::bind_front(&SettingsComponent::PickupClicked, this);
#endif

      /* profile_group_ */
      profile_group_.setText(juce::translate("Profile"));
      AddComponent(profile_group_, 0, 100, kSettingsWidth, 100);
      /* profile_location_button_*/
      AddComponent(profile_location_button_, kSettingsLeft, 120, kSettingsWidth - 2 * kSettingsLeft,
          25);
#if __cpp_lib_bind_front >= 202'306L
      profile_location_button_.onClick = std::bind_front<&SettingsComponent::ProfileClicked>(this);
#else
      profile_location_button_.onClick = std::bind_front(&SettingsComponent::ProfileClicked, this);
#endif
      /* profile_location_label_ */
      profile_location_label_.setText(settings_manager_.GetProfileDirectory(),
          juce::NotificationType::dontSendNotification);
      AddComponent(profile_location_label_, kSettingsLeft, 145, kSettingsWidth - 2 * kSettingsLeft,
          30);

      /* autohide_group_ */
      autohide_group_.setText(juce::translate("Auto hide"));
      AddComponent(autohide_group_, 0, 200, kSettingsWidth, 100);
      /* autohide_explain_label_ */
      autohide_explain_label_.setText(juce::translate("Autohide the plugin window in x seconds, "
                                                      "select 0 for disabling autohide"),
          juce::NotificationType::dontSendNotification);
      StandardLabelSettings(autohide_explain_label_);
      AddComponent(autohide_explain_label_, kSettingsLeft, 215, kSettingsWidth - 2 * kSettingsLeft,
          50);
      /* autohide_setting_ */
      autohide_setting_.setRange(0., 10., 1.);
      autohide_setting_.setValue(settings_manager_.GetAutoHideTime(),
          juce::NotificationType::dontSendNotification);
      AddComponent(autohide_setting_, kSettingsLeft, 245, kSettingsWidth - 2 * kSettingsLeft, 50);
#if __cpp_lib_bind_front >= 202'306L
      autohide_setting_.onValueChange = std::bind_front<&SettingsComponent::AutohideChanged>(this);
#else
      autohide_setting_.onValueChange = std::bind_front(&SettingsComponent::AutohideChanged, this);
#endif

      /* turn it on */
      activateLayout();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void SettingsComponent::paint(juce::Graphics& g)
{                                   //-V2009 overridden method
   g.fillAll(juce::Colours::white); /* clear the background */
}