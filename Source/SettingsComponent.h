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

class SettingsComponent: public Component,
  public ButtonListener, private ResizableLayout, public Slider::Listener {
public:
  SettingsComponent();
  ~SettingsComponent();
  void Init(std::shared_ptr<SettingsManager>& settings_manager);

  void paint(Graphics&) override;
  //void resized();

  // ButtonListener interface
  virtual void buttonClicked(Button* button) override;
  virtual void sliderValueChanged(Slider* slider) override;

private:
  GroupComponent autohide_group_{};
  GroupComponent pickup_group_{};
  GroupComponent profile_group_{};
  Label autohide_explain_label_{};
  Label pickup_label_{"PickupLabel", ""};
  Label profile_location_label_{"Profile Label"};
  Slider autohide_setting_;
  std::weak_ptr<SettingsManager> settings_manager_;
  TextButton profile_location_button_{"Choose Profile Folder"};
  ToggleButton pickup_enabled_{"Enable Pickup Mode"};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsComponent)
};

#endif  // SETTINGSCOMPONENT_H_INCLUDED
