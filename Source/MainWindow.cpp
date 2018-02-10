// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

MainWindow::MainWindow(juce::String name): juce::DocumentWindow{name,
juce::Colours::lightgrey,
juce::DocumentWindow::minimiseButton |
juce::DocumentWindow::closeButton}
{
    juce::TopLevelWindow::setUsingNativeTitleBar(true);
    window_content_ = new MainContentComponent{};
    juce::ResizableWindow::setContentOwned(window_content_, true);
    juce::Component::centreWithSize(getWidth(), getHeight());
    juce::Component::setVisible(true);
}

void MainWindow::Init(CommandMap* const command_map,
    std::weak_ptr<LrIpcOut>&& lr_ipc_out,
    std::shared_ptr<MidiProcessor>& midi_processor,
    ProfileManager* const profile_manager,
    SettingsManager* const settings_manager,
    std::shared_ptr<MidiSender>& midi_sender)
{
    if (window_content_)
        window_content_->Init(command_map, std::move(lr_ipc_out),
            midi_processor, profile_manager, settings_manager, midi_sender);
}