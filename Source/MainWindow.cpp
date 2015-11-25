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
    
	if (m_autoHideCounter > 0)
	{
		//decrement counter
		--m_autoHideCounter;
	}
	
	//set the new timer text
	m_windowContent.SetTimerText(m_autoHideCounter);

	if (m_autoHideCounter == 0)
	{
		//first stop the timer so it will not be called again
		this->stopTimer();

		//check if the window is not already minimized
		if (!this->isMinimised())
		{
			this->minimiseButtonPressed();
		}
	}
	

}