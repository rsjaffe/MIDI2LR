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
                                         _pickupLabel("PickupLabel", "")
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
}

void SettingsComponent::buttonClicked(Button* button)
{
    SettingsManager::getInstance().setPickupEnabled(_pickupEnabled.getToggleState());
}
