/*
  ==============================================================================

    MainWindow.cpp
    Created: 22 Nov 2015 8:28:57pm
    Author:  Jeffrey

  ==============================================================================
*/

#include "MainWindow.h"

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

void MainWindow::Init(void)
{
	
	// get the auto time setting
	m_autoHideCounter = SettingsManager::getInstance().getAutoHideTime();
	//start timing
	this->startTimer(1000);	

}