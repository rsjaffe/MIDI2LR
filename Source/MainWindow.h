#pragma once
/*
  ==============================================================================

    MainWindow.h

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/

#ifndef MAINWINDOW_H_INCLUDED
#define MAINWINDOW_H_INCLUDED

#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"
#include "SettingsManager.h"

class MainWindow final: private juce::DocumentWindow, private juce::Timer {
public:
  explicit MainWindow(juce::String name): juce::DocumentWindow{name,
    juce::Colours::lightgrey,
    juce::DocumentWindow::minimiseButton |
    juce::DocumentWindow::closeButton}, juce::Timer() {
    setUsingNativeTitleBar(true);
    window_content_ = new MainContentComponent{};

    setContentOwned(window_content_, true);

    centreWithSize(getWidth(), getHeight());
    setVisible(true);
  }

  void Init(std::shared_ptr<CommandMap>& command_map,
    std::weak_ptr<LR_IPC_IN>&& in, std::weak_ptr<LR_IPC_OUT>&& out,
    std::shared_ptr<MIDIProcessor>& midi_processor,
    std::shared_ptr<ProfileManager>& profile_manager,
    std::shared_ptr<SettingsManager>& settings_manager,
    std::shared_ptr<MIDISender>& midi_sender);

/* Note: Be careful if you override any DocumentWindow methods - the base
   class uses a lot of them, so by overriding you might break its functionality.
   It's best to do all your work in your content component instead, but if
   you really have to override any DocumentWindow methods, make sure your
   subclass also calls the superclass's method.
*/

private:
  void closeButtonPressed() override {
    // This is called when the user tries to close this window. Here, we'll just
    // ask the app to quit when this happens, but you can change this to do
    // whatever you need.
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
  }
  // the timer callback function
  virtual void timerCallback() override;
  int auto_hide_counter_{0};
  MainContentComponent *window_content_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};

#endif  // MAINWINDOW_H_INCLUDED
