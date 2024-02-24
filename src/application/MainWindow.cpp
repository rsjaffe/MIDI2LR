/*
 * This file is part of MIDI2LR. Copyright (C) 2015 by Rory Jaffe.
 *
 * MIDI2LR is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with MIDI2LR.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include "MainWindow.h"

#include <exception>

#include "SettingsManager.h"

MainWindow::MainWindow(const juce::String& name, const CommandSet& command_set, Profile& profile,
    ProfileManager& profile_manager, SettingsManager& settings_manager, LrIpcOut& lr_ipc_out,
    MidiReceiver& midi_receiver, MidiSender& midi_sender)
try : juce
   ::DocumentWindow {name, juce::Colours::lightgrey,
       juce::DocumentWindow::minimiseButton | juce::DocumentWindow::closeButton},
       window_content_ {std::make_unique<MainContentComponent>(command_set, profile,
           profile_manager, settings_manager, lr_ipc_out, midi_receiver, midi_sender)}
   {
      juce::TopLevelWindow::setUsingNativeTitleBar(true);
      juce::ResizableWindow::setContentNonOwned(window_content_.get(), true);
      juce::Component::centreWithSize(getWidth(), getHeight());
      juce::Component::setVisible(true);
      window_content_->Init();
      if (const auto hide_sec {settings_manager.GetAutoHideTime()}) { /* don't start timer if time
                                                                         is zero */
         juce::Timer::startTimer(1000 * hide_sec);
      }
   }

catch (const std::exception& e) {
   rsj::ExceptionResponse(e);
   throw;
}

void MainWindow::timerCallback()
{
   try {
      juce::Timer::stopTimer();
      if (!juce::ResizableWindow::isMinimised()) { juce::DocumentWindow::minimiseButtonPressed(); }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e);
      throw;
   }
}