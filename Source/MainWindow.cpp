/*
  ==============================================================================

    MainWindow.cpp
    Created: 22 Nov 2015 8:28:57pm
    Author:  Jeffrey

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

#include "MainWindow.h"

/**********************************************************************************************//**
 * @fn  void MainWindow::timerCallback(void)
 *
 * @brief   Callback, called when the timer.
 *
 *
 * @date    3/22/2016
 **************************************************************************************************/

void MainWindow::timerCallback(void)
{
	bool decreasedValue = false;

	if (m_autoHideCounter > 0)
	{
		//decrement counter
		--m_autoHideCounter;
		decreasedValue = true;
	}
	
	//set the new timer text
	m_windowContent->SetTimerText(m_autoHideCounter);

	if (m_autoHideCounter == 0)
	{
		//first stop the timer so it will not be called again
		this->stopTimer();

		//check if the window is not already minimized
		if (!this->isMinimised())
		{
			if (decreasedValue)
			{
				this->minimiseButtonPressed();
			}
		}
	}
	

}

/**********************************************************************************************//**
 * @fn  void MainWindow::Init(CommandMap *commandMap, LR_IPC_IN *in, LR_IPC_OUT *out, MIDIProcessor *midiProcessor, ProfileManager *profileManager, SettingsManager *settingsManager, MIDISender *midiSender)
 *
 * @brief   S.
 *
 *
 * @date    3/22/2016
 *
 * @param [in,out]  commandMap      If non-null, the command map.
 * @param [in,out]  in              If non-null, the in.
 * @param [in,out]  out             If non-null, the out.
 * @param [in,out]  midiProcessor   If non-null, the MIDI processor.
 * @param [in,out]  profileManager  If non-null, manager for profile.
 * @param [in,out]  settingsManager If non-null, manager for settings.
 * @param [in,out]  midiSender      If non-null, the MIDI sender.
 **************************************************************************************************/

void MainWindow::Init(CommandMap *commandMap, LR_IPC_IN *in, LR_IPC_OUT *out, MIDIProcessor *midiProcessor, ProfileManager *profileManager, SettingsManager *settingsManager, MIDISender *midiSender)
{
	
	// get the auto time setting
	if (settingsManager)
	{
		m_autoHideCounter = settingsManager->getAutoHideTime();
	}
	else
	{
		m_autoHideCounter = 0;
	}
	
	//start timing
	this->startTimer(1000);	
	
	if (m_windowContent)
	{
		m_windowContent->Init(commandMap,in, out, midiProcessor, profileManager, settingsManager, midiSender);
	}
}