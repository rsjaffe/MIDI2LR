/*
  ==============================================================================

    SettingsComponent.h
    Created: 24 Aug 2015 11:43:59am
    Author:  Parth

  ==============================================================================
*/

#ifndef SETTINGSCOMPONENT_H_INCLUDED
#define SETTINGSCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class SettingsComponent    : public Component,
                             public ButtonListener,
                             public SliderListener
{
public:
    SettingsComponent();
    ~SettingsComponent();

    void paint (Graphics&);
    void resized();

    // ButtonListener interface
    virtual void buttonClicked(Button* button) override;

    // SliderListener interface
    virtual void sliderValueChanged(Slider *slider) override;
private:
    ToggleButton _pickupEnabled;
    Label _pickupLabel;
    
    Slider _midiDelaySlider;
    Label _midiDelayLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsComponent)
};


#endif  // SETTINGSCOMPONENT_H_INCLUDED
