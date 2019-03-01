#pragma once
/*
  ==============================================================================

    SettingsComponent.h

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
#ifndef MIDI2LR_SETTINGSCOMPONENT_H_INCLUDED
#define MIDI2LR_SETTINGSCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "ResizableLayout.h"
class SettingsManager;

class SettingsComponent final : public juce::Component,
                                juce::ButtonListener,
                                ResizableLayout,
                                juce::Slider::Listener {
 public:
   explicit SettingsComponent(SettingsManager& settings_manager);
   ~SettingsComponent() = default;
   SettingsComponent(const SettingsComponent& other) = delete;
   SettingsComponent(SettingsComponent&& other) = delete;
   SettingsComponent& operator=(const SettingsComponent& other) = delete;
   SettingsComponent& operator=(SettingsComponent&& other) = delete;
   void Init();

 private:
   void paint(juce::Graphics&) override;
   void buttonClicked(juce::Button* button) override;
   void sliderValueChanged(juce::Slider* slider) override;

   juce::GroupComponent autohide_group_{};
   juce::GroupComponent pickup_group_{};
   juce::GroupComponent profile_group_{};
   juce::Label autohide_explain_label_{};
   juce::Label pickup_label_{"PickupLabel", ""};
   juce::Label profile_location_label_{"Profile Label"};
   juce::Slider autohide_setting_;
   juce::TextButton profile_location_button_{TRANS("Choose Profile Folder")};
   juce::ToggleButton pickup_enabled_{TRANS("Enable Pickup Mode")};
   SettingsManager& settings_manager_;
};

#endif // SETTINGSCOMPONENT_H_INCLUDED
