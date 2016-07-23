/*
  ==============================================================================

    MainWindow.cpp

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

#include "MainWindow.h"
#include <utility>

void MainWindow::Init(std::shared_ptr<CommandMap>& command_map,
  std::weak_ptr<LR_IPC_IN>&& lr_ipc_in,
  std::weak_ptr<LR_IPC_OUT>&& lr_ipc_out,
  std::shared_ptr<MIDIProcessor>& midi_processor,
  std::shared_ptr<ProfileManager>& profile_manager,
  std::shared_ptr<SettingsManager>& settings_manager,
  std::shared_ptr<MIDISender>& midi_sender) {
  // get the auto time setting
  if (settings_manager) {
    auto_hide_counter_ = settings_manager->getAutoHideTime();
  }
  else {
    auto_hide_counter_ = 0;
  }

  //start timing
  juce::Timer::startTimer(1000);

  if (window_content_) {
    window_content_->Init(command_map, std::move(lr_ipc_in), std::move(lr_ipc_out),
      midi_processor, profile_manager, settings_manager, midi_sender);
  }
}

void MainWindow::timerCallback(void) {
  auto decreased_value = false;

  if (auto_hide_counter_ > 0) {
    //decrement counter
    --auto_hide_counter_;
    decreased_value = true;
  }

  //set the new timer text
  window_content_->SetTimerText(auto_hide_counter_);

  if (auto_hide_counter_ == 0) {
    //first stop the timer so it will not be called again
    juce::Timer::stopTimer();

    //check if the window is not already minimized
    if (!juce::ResizableWindow::isMinimised()) {
      if (decreased_value) {
        juce::DocumentWindow::minimiseButtonPressed();
      }
    }
  }
}