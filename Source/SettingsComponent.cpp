/*
  ==============================================================================

    SettingsComponent.cpp
    Created: 24 Aug 2015 11:43:59am
    Author:  Parth

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SettingsComponent.h"
#include "SettingsManager.h"

//==============================================================================
SettingsComponent::SettingsComponent() : _pickupEnabled("Enable Pickup Mode"),
                                         _pickupLabel("PickupLabel", ""),
                                         _profileLocationButton("Choose Profile Folder"),
                                         _profileLocationLabel("Profile Label", SettingsManager::getInstance().getProfileDirectory())
{

    _pickupEnabled.addListener(this);
    _pickupEnabled.setToggleState(SettingsManager::getInstance().getPickupEnabled(), NotificationType::dontSendNotification);
    addAndMakeVisible(_pickupEnabled);

    _pickupLabel.setFont(Font(12.f, Font::bold));
    _pickupLabel.setText("Disabling the pickup mode may be better for touchscreen interfaces and may solve issues with LR not picking up fast fader/knob movements",
                         NotificationType::dontSendNotification);

    _pickupLabel.setEditable(false);
    _pickupLabel.setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(_pickupLabel);

    _profileLocationButton.addListener(this);
    addAndMakeVisible(_profileLocationButton);

    _profileLocationLabel.setEditable(false);
    _profileLocationLabel.setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(_profileLocationLabel);
}

SettingsComponent::~SettingsComponent()
{

}

void SettingsComponent::paint (Graphics& g)
{
    g.fillAll (Colours::white);   // clear the background
}

void SettingsComponent::resized()
{
    _pickupLabel.setBoundsRelative(.1f, .1f, .8f, .3f);
    _pickupEnabled.setBoundsRelative(.1f, .35f, .5f, .2f);

    _profileLocationButton.setBoundsRelative(.1f, .6f, .5f, .1f);
    _profileLocationLabel.setBoundsRelative(.1f, .7f, .8f, .2f);
}

void SettingsComponent::buttonClicked(Button* button)
{
  if(button == &_pickupEnabled)
    SettingsManager::getInstance().setPickupEnabled(_pickupEnabled.getToggleState());
  else if (button == &_profileLocationButton)
  {
    FileBrowserComponent browser(FileBrowserComponent::canSelectDirectories | FileBrowserComponent::openMode,
      File::getCurrentWorkingDirectory(),
      nullptr,
      nullptr);
    FileChooserDialogBox dialogBox("Select Profile Folder",
      "Select a folder containing MIDI2LR Profiles",
      browser,
      true,
      Colours::lightgrey);

    if (dialogBox.show())
    {
      String profileLoc = browser.getSelectedFile(0).getFullPathName();
      SettingsManager::getInstance().setProfileDirectory(profileLoc);
      _profileLocationLabel.setText(profileLoc, NotificationType::dontSendNotification);
    }
  }
}
