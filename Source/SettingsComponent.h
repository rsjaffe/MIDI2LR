/*
  ==============================================================================

	SettingsComponent.h
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
#ifndef SETTINGSCOMPONENT_H_INCLUDED
#define SETTINGSCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "ResizableLayout.h"

//==============================================================================
/*
*/
class SettingsComponent : public Component,
	public ButtonListener, ResizableLayout
{
public:
	SettingsComponent();
	~SettingsComponent();

	void paint(Graphics&);
	//void resized();

	// ButtonListener interface
	virtual void buttonClicked(Button* button) override;

private:
	ToggleButton _pickupEnabled;
	Label _pickupLabel;

	TextButton _profileLocationButton;
	Label _profileLocationLabel;
	GroupComponent m_autoHideGroup;
	GroupComponent m_pickupGroup;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsComponent)
};


#endif  // SETTINGSCOMPONENT_H_INCLUDED
