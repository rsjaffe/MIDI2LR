/*
  ==============================================================================

	SettingsComponent.cpp
	Created: 24 Aug 2015 11:43:59am
	Author:  Parth, Jaffe

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe, derived from code
by Parth.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.  
  ==============================================================================
*/
#include "../JuceLibraryCode/JuceHeader.h"
#include "SettingsComponent.h"
#include "SettingsManager.h"

#define SETTINGS_LEFT 20
#define SETTING_WIDTH 400
#define SETTING_HEIGHT 300
//==============================================================================
SettingsComponent::SettingsComponent() : ResizableLayout(this), _pickupEnabled("Enable Pickup Mode"),
_pickupLabel("PickupLabel", ""),
_profileLocationButton("Choose Profile Folder"),
_profileLocationLabel("Profile Label", SettingsManager::getInstance().getProfileDirectory()),
m_autoHideGroup(),
m_pickupGroup(),
m_profileGroup(),
m_autoHideExplainLabel()
{

	// for layouts to work you must start at some size
	// place controls in a location that is initially correct.
	setSize(SETTING_WIDTH, SETTING_HEIGHT);

	
	m_pickupGroup.setText("Pick up");
	m_pickupGroup.setBounds(0, 0, SETTING_WIDTH, 100);
	addToLayout(&m_pickupGroup, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(m_pickupGroup);
	
	_pickupLabel.setFont(Font(12.f, Font::bold));
	_pickupLabel.setText("Disabling the pickup mode may be better for touchscreen interfaces and may solve issues with LR not picking up fast fader/knob movements", NotificationType::dontSendNotification);
	_pickupLabel.setBounds(SETTINGS_LEFT, 15, SETTING_WIDTH - 2*SETTINGS_LEFT, 50);
	addToLayout(&_pickupLabel, anchorMidLeft, anchorMidRight);
	_pickupLabel.setEditable(false);
	_pickupLabel.setColour(Label::textColourId, Colours::darkgrey);
	addAndMakeVisible(_pickupLabel);
	
	
	_pickupEnabled.addListener(this);
	_pickupEnabled.setToggleState(SettingsManager::getInstance().getPickupEnabled(), NotificationType::dontSendNotification);
	_pickupEnabled.setBounds(SETTINGS_LEFT, 60, SETTING_WIDTH - 2 * SETTINGS_LEFT, 32);
	addToLayout(&_pickupEnabled, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(_pickupEnabled);

	// ---------------------------- profile section -----------------------------------
	m_profileGroup.setText("Profile");
	m_profileGroup.setBounds(0, 100, SETTING_WIDTH, 100);
	addToLayout(&m_profileGroup, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(m_profileGroup);


	_profileLocationButton.addListener(this);
	_profileLocationButton.setBounds(SETTINGS_LEFT, 120, SETTING_WIDTH - 2*SETTINGS_LEFT, 25);
	addToLayout(&_profileLocationButton, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(_profileLocationButton);

	_profileLocationLabel.setEditable(false);
	_profileLocationLabel.setBounds(SETTINGS_LEFT, 145, SETTING_WIDTH - 2 * SETTINGS_LEFT, 30);
	addToLayout(&_profileLocationLabel, anchorMidLeft, anchorMidRight);
	_profileLocationLabel.setColour(Label::textColourId, Colours::darkgrey);
	addAndMakeVisible(_profileLocationLabel);



////// ----------------------- auto hide section ------------------------------------
	m_autoHideGroup.setText("Auto hide");
	m_autoHideGroup.setBounds(0, 200, SETTING_WIDTH, 100);
	addToLayout(&m_autoHideGroup, anchorMidLeft, anchorMidRight);
	addAndMakeVisible(m_autoHideGroup);
	

	m_autoHideExplainLabel.setFont(Font(12.f, Font::bold));
	m_autoHideExplainLabel.setText("Autohide the plugin window in x seconds, select 0 for disabling autohide", NotificationType::dontSendNotification);
	m_autoHideExplainLabel.setBounds(SETTINGS_LEFT, 215, SETTING_WIDTH - 2*SETTINGS_LEFT, 50);
	addToLayout(&m_autoHideExplainLabel, anchorMidLeft, anchorMidRight);
	m_autoHideExplainLabel.setEditable(false);
	m_autoHideExplainLabel.setFont(Font(12.f, Font::bold));
	m_autoHideExplainLabel.setColour(Label::textColourId, Colours::darkgrey);
	addAndMakeVisible(m_autoHideExplainLabel);

	m_autoHideSetting.setBounds(SETTINGS_LEFT, 245, SETTING_WIDTH - 2 * SETTINGS_LEFT, 50);
	m_autoHideSetting.setRange(0, 10, 1); 
	m_autoHideSetting.setValue(SettingsManager::getInstance().getAutoHideTime(), NotificationType::dontSendNotification);

	addToLayout(&m_autoHideSetting, anchorMidLeft, anchorMidRight);
	//add this as the lister for the data
	m_autoHideSetting.addListener(this);
	addAndMakeVisible(m_autoHideSetting);
	// turn it on
	activateLayout();
}

SettingsComponent::~SettingsComponent()
{

}

void SettingsComponent::paint(Graphics& g)
{
	g.fillAll(Colours::white);   // clear the background
}

void SettingsComponent::buttonClicked(Button* button)
{
	if (button == &_pickupEnabled)
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

void SettingsComponent::sliderValueChanged(Slider* slider)
{
	// NULL pointer check
	if (slider)
	{
		if (&m_autoHideSetting == slider)
		{
			//get the rounded setting
			int newSetting = (int)m_autoHideSetting.getValue();

			SettingsManager::getInstance().setAutoHideTime(newSetting);

		}

	}


}