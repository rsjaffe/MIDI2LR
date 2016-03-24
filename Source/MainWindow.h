#pragma once
/*
  ==============================================================================

    MainWindow.h

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

#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"
#include "SettingsManager.h"

/**********************************************************************************************//**
* @class   MainWindow
*
* @brief   This class implements the desktop window that contains an instance of
        our MainContentComponent class.
*
* @author  Rory Jaffe
* @date    3/20/2016
**************************************************************************************************/

    class MainWindow: public DocumentWindow, public Timer
    {
    public:
        MainWindow(String name): DocumentWindow(name,
            Colours::lightgrey,
            DocumentWindow::minimiseButton |
            DocumentWindow::closeButton) , Timer()
        {
            setUsingNativeTitleBar(true);
			m_windowContent = new MainContentComponent();
			
            setContentOwned(m_windowContent, true);

            centreWithSize(getWidth(), getHeight());
            setVisible(true);            
        }

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

		void Init(std::shared_ptr<CommandMap> commandMap, std::shared_ptr<LR_IPC_IN> in, std::shared_ptr<LR_IPC_OUT> out, std::shared_ptr<MIDIProcessor> midiProcessor, 
            std::shared_ptr<ProfileManager> profileManager, std::shared_ptr<SettingsManager> settingsManager, std::shared_ptr<MIDISender> midiSender);

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */
        
        // the timer callback function
        virtual void timerCallback();
        
    private:
		int m_autoHideCounter;
		MainContentComponent *m_windowContent;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };


#endif  // MAINWINDOW_H_INCLUDED
