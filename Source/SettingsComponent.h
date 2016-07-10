#pragma once
/*
  ==============================================================================

    SettingsComponent.h

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
#ifndef SETTINGSCOMPONENT_H_INCLUDED
#define SETTINGSCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "ResizableLayout.h"
#include "SettingsManager.h"

class SettingsComponent final: public Component,
	private ButtonListener, private ResizableLayout, private Slider::Listener, private TextEditor::Listener {
public:
  SettingsComponent();
  ~SettingsComponent();
  void Init(std::shared_ptr<SettingsManager>& settings_manager);
private:
  void paint(Graphics&) override;
  virtual void buttonClicked(Button* button) override;
  virtual void sliderValueChanged(Slider* slider) override;
  virtual void textEditorTextChanged(TextEditor &editor) override;

  GroupComponent autohide_group_{};
  GroupComponent pickup_group_{};
  GroupComponent profile_group_{};
  GroupComponent controllers_group_{};
  Label autohide_explain_label_{};
  Label pickup_label_{"PickupLabel", ""};
  Label profile_location_label_{"Profile Label"};
  Label continuous_label_{"ContinuousLabel", ""};
  Label maxpitch_label_{"MaxPitchLabel", ""};
  Slider autohide_setting_;
  std::weak_ptr<SettingsManager> settings_manager_;
  TextButton profile_location_button_{"Choose Profile Folder"};
  TextButton apply_button_{"Apply"};
  ToggleButton pickup_enabled_{"Enable Pickup Mode"};
  ToggleButton continuous_enabled_{"Continuous Encoder Mode"};
  TextEditor pitch_max_value_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsComponent)
};

#endif  // SETTINGSCOMPONENT_H_INCLUDED
