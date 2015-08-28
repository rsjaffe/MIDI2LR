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
                                         _midiDelaySlider(),
                                         _midiDelayLabel("MIDIDelayLabel", "Sets the delay between sending MIDI messages \
                                                          which may be necessary for motorized sliders to update correctly (default 20ms). \
                                                          MIDI2LR will need to be restarted for this change to take effect.")
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

    _midiDelaySlider.setRange(1.0, 50.0, 1.0);
    _midiDelaySlider.setSliderStyle(Slider::LinearHorizontal);
    _midiDelaySlider.setTextBoxStyle(Slider::TextBoxRight, false, 40, 20);
    _midiDelaySlider.setValue(SettingsManager::getInstance().getMIDISendDelay(), NotificationType::dontSendNotification);
    _midiDelaySlider.setChangeNotificationOnlyOnRelease(true);
    _midiDelaySlider.setTextValueSuffix("ms");
    _midiDelaySlider.addListener(this);
    addAndMakeVisible(_midiDelaySlider);

    _midiDelayLabel.setFont(Font(12.f, Font::bold));
    _midiDelayLabel.setEditable(false);
    _midiDelayLabel.setColour(Label::textColourId, Colours::darkgrey);
    addAndMakeVisible(_midiDelayLabel);
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
    _pickupLabel.setBoundsRelative(.1f, .1f, .9f, .3f);
    _pickupEnabled.setBoundsRelative(.1f, .3f, .5f, .2f);

    _midiDelayLabel.setBoundsRelative(.1f, .5f, .9f, .3f);
    _midiDelaySlider.setBoundsRelative(.1f, .7f, .5f, .3f);
}

void SettingsComponent::buttonClicked(Button* button)
{
    if (button == &_pickupEnabled)
        SettingsManager::getInstance().setPickupEnabled(_pickupEnabled.getToggleState());
}

void SettingsComponent::sliderValueChanged(Slider* slider)
{
    if (slider == &_midiDelaySlider)
        SettingsManager::getInstance().setMIDISendDelay((int)slider->getValue());
}
