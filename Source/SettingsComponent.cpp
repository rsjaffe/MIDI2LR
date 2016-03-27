/*
  ==============================================================================

	SettingsComponent.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

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

constexpr auto SettingsLeft = 20;
constexpr auto SettingsWidth = 400;
constexpr auto SettingsHeight = 300;

/**********************************************************************************************//**
 * @fn  SettingsComponent::SettingsComponent()
 *
 * @brief   Default constructor.
 *
 * @date    3/22/2016
 **************************************************************************************************/

SettingsComponent::SettingsComponent() : ResizableLayout(this), _pickupEnabled("Enable Pickup Mode"),
_pickupLabel("PickupLabel", ""),
_profileLocationButton("Choose Profile Folder"),
_profileLocationLabel("Profile Label"),
m_autoHideGroup(),
m_pickupGroup(),
m_profileGroup(),
m_autoHideExplainLabel(),
m_settingsManager(nullptr)
{

	
}

/**********************************************************************************************//**
 * @fn  SettingsComponent::~SettingsComponent()
 *
 * @brief   Destructor.
 *
 * @date    3/22/2016
 **************************************************************************************************/

SettingsComponent::~SettingsComponent()
{

}

/**********************************************************************************************//**
 * @fn  void SettingsComponent::paint(Graphics& g)
 *
 * @brief   Paints the given g.
 *
 * @date    3/22/2016
 *
 * @param [in,out]  g   The Graphics to process.
 **************************************************************************************************/

void SettingsComponent::paint(Graphics& g)
{
	g.fillAll(Colours::white);   // clear the background
}

/**********************************************************************************************//**
 * @fn  void SettingsComponent::buttonClicked(Button* button)
 *
 * @brief   Button clicked.
 *
 * @date    3/22/2016
 *
 * @param [in,out]  button  If non-null, the button.
 **************************************************************************************************/

void SettingsComponent::buttonClicked(Button* button)
{
	if (button == &_pickupEnabled)
	{
		if (m_settingsManager)
		{
			m_settingsManager->setPickupEnabled(_pickupEnabled.getToggleState());
		}		
	}
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
			auto profileLoc = browser.getSelectedFile(0).getFullPathName();
			if (m_settingsManager)
			{
				m_settingsManager->setProfileDirectory(profileLoc);
			}			
			_profileLocationLabel.setText(profileLoc, NotificationType::dontSendNotification);
		}
	}
}

/**********************************************************************************************//**
 * @fn  void SettingsComponent::sliderValueChanged(Slider* slider)
 *
 * @brief   Slider value changed.
 *
 * @date    3/22/2016
 *
 * @param [in,out]  slider  If non-null, the slider.
 **************************************************************************************************/

void SettingsComponent::sliderValueChanged(Slider* slider)
{
	// NULL pointer check
	if (slider)
	{
		if (&m_autoHideSetting == slider)
		{
			//get the rounded setting
			int newSetting = (int)m_autoHideSetting.getValue();

			if (m_settingsManager)
			{
				m_settingsManager->setAutoHideTime(newSetting);
			}			
		}
	}

}

/**********************************************************************************************//**
 * @fn  void SettingsComponent::Init(std::shared_ptr<SettingsManager>& settingsManager)
 *
 * @brief   Inits the given settings manager.
 *
 * @date    3/22/2016
 *
 * @param [in,out]  settingsManager If non-null, manager for settings.
 **************************************************************************************************/

void SettingsComponent::Init(std::shared_ptr<SettingsManager>& settingsManager)
{
	//copy the pointer
	m_settingsManager = settingsManager;

	// for layouts to work you must start at some size
	// place controls in a location that is initially correct.
	setSize(SettingsWidth, SettingsHeight);

	if (m_settingsManager)
	{
		m_pickupGroup.setText("Pick up");
		m_pickupGroup.setBounds(0, 0, SettingsWidth, 100);
		addToLayout(&m_pickupGroup, anchorMidLeft, anchorMidRight);
		addAndMakeVisible(m_pickupGroup);

		_pickupLabel.setFont(Font(12.f, Font::bold));
		_pickupLabel.setText("Disabling the pickup mode may be better for touchscreen interfaces and may solve issues with LR not picking up fast fader/knob movements", NotificationType::dontSendNotification);
		_pickupLabel.setBounds(SettingsLeft, 15, SettingsWidth - 2 * SettingsLeft, 50);
		addToLayout(&_pickupLabel, anchorMidLeft, anchorMidRight);
		_pickupLabel.setEditable(false);
		_pickupLabel.setColour(Label::textColourId, Colours::darkgrey);
		addAndMakeVisible(_pickupLabel);


		_pickupEnabled.addListener(this);
		_pickupEnabled.setToggleState(m_settingsManager->getPickupEnabled(), NotificationType::dontSendNotification);
		_pickupEnabled.setBounds(SettingsLeft, 60, SettingsWidth - 2 * SettingsLeft, 32);
		addToLayout(&_pickupEnabled, anchorMidLeft, anchorMidRight);
		addAndMakeVisible(_pickupEnabled);

		// ---------------------------- profile section -----------------------------------
		m_profileGroup.setText("Profile");
		m_profileGroup.setBounds(0, 100, SettingsWidth, 100);
		addToLayout(&m_profileGroup, anchorMidLeft, anchorMidRight);
		addAndMakeVisible(m_profileGroup);


		_profileLocationButton.addListener(this);
		_profileLocationButton.setBounds(SettingsLeft, 120, SettingsWidth - 2 * SettingsLeft, 25);
		addToLayout(&_profileLocationButton, anchorMidLeft, anchorMidRight);
		addAndMakeVisible(_profileLocationButton);

		_profileLocationLabel.setEditable(false);
		_profileLocationLabel.setBounds(SettingsLeft, 145, SettingsWidth - 2 * SettingsLeft, 30);
		addToLayout(&_profileLocationLabel, anchorMidLeft, anchorMidRight);
		_profileLocationLabel.setColour(Label::textColourId, Colours::darkgrey);
		addAndMakeVisible(_profileLocationLabel);
		_profileLocationLabel.setText(m_settingsManager->getProfileDirectory(), NotificationType::dontSendNotification);
		

		////// ----------------------- auto hide section ------------------------------------
		m_autoHideGroup.setText("Auto hide");
		m_autoHideGroup.setBounds(0, 200, SettingsWidth, 100);
		addToLayout(&m_autoHideGroup, anchorMidLeft, anchorMidRight);
		addAndMakeVisible(m_autoHideGroup);


		m_autoHideExplainLabel.setFont(Font(12.f, Font::bold));
		m_autoHideExplainLabel.setText("Autohide the plugin window in x seconds, select 0 for disabling autohide", NotificationType::dontSendNotification);
		m_autoHideExplainLabel.setBounds(SettingsLeft, 215, SettingsWidth - 2 * SettingsLeft, 50);
		addToLayout(&m_autoHideExplainLabel, anchorMidLeft, anchorMidRight);
		m_autoHideExplainLabel.setEditable(false);
		m_autoHideExplainLabel.setFont(Font(12.f, Font::bold));
		m_autoHideExplainLabel.setColour(Label::textColourId, Colours::darkgrey);
		addAndMakeVisible(m_autoHideExplainLabel);

		m_autoHideSetting.setBounds(SettingsLeft, 245, SettingsWidth - 2 * SettingsLeft, 50);
		m_autoHideSetting.setRange(0, 10, 1);
		m_autoHideSetting.setValue(m_settingsManager->getAutoHideTime(), NotificationType::dontSendNotification);

		addToLayout(&m_autoHideSetting, anchorMidLeft, anchorMidRight);
		//add this as the lister for the data
		m_autoHideSetting.addListener(this);
		addAndMakeVisible(m_autoHideSetting);
		// turn it on
		activateLayout();
	}
}