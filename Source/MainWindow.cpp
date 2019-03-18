/*
  ==============================================================================

    MainWindow.cpp

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

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
#include "MainWindow.h"

#include <utility>

#include "MainComponent.h"
#include "SettingsManager.h"

MainWindow::MainWindow(const juce::String& name, Profile& profile, ProfileManager& profile_manager,
    SettingsManager& settings_manager, std::weak_ptr<LrIpcOut>&& lr_ipc_out,
    std::shared_ptr<MidiReceiver> midi_receiver, std::shared_ptr<MidiSender> midi_sender) try : juce
   ::DocumentWindow{name, juce::Colours::lightgrey,
       juce::DocumentWindow::minimiseButton | juce::DocumentWindow::closeButton}
   {
      juce::TopLevelWindow::setUsingNativeTitleBar(true);
#pragma warning(suppress : 26409 24623)
      window_content_ = new MainContentComponent(profile, profile_manager, settings_manager);
      juce::ResizableWindow::setContentOwned(window_content_, true);
      juce::Component::centreWithSize(getWidth(), getHeight());
      juce::Component::setVisible(true);
      window_content_->Init(
          std::move(lr_ipc_out), std::move(midi_receiver), std::move(midi_sender));
      // get the auto time setting
      const auto hide_sec = settings_manager.GetAutoHideTime();
      // start timing
      if (hide_sec) // don't start timer if time is zero!
         juce::Timer::startTimer(1000 * hide_sec);
   }
catch (const std::exception& e) {
   rsj::ExceptionResponse(typeid(this).name(), __func__, e);
   throw;
}

void MainWindow::timerCallback()
{
   try {
      juce::Timer::stopTimer();
      if (!juce::ResizableWindow::isMinimised())
         juce::DocumentWindow::minimiseButtonPressed();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void MainWindow::SaveProfile()
{
   try {
      if (window_content_)
         window_content_->SaveProfile();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}