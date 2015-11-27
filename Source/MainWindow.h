/*
  ==============================================================================

    MainWindow.h
    Created: 22 Nov 2015 8:28:57pm
    Author:  Jeffrey

  ==============================================================================
*/

#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"
#include "SettingsManager.h"
//==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainContentComponent class.
    */
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

		void Init(void);

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
