#ifndef MIDI2LR_MAINWINDOW_H_INCLUDED
#define MIDI2LR_MAINWINDOW_H_INCLUDED
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
#include <memory>

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "MainComponent.h"

class CommandSet;
class LrIpcOut;
class MidiReceiver;
class MidiSender;
class Profile;
class ProfileManager;
class SettingsManager;

class MainWindow final : juce::DocumentWindow, juce::Timer {
 public:
   MainWindow(const juce::String& name, const CommandSet& command_set, Profile& profile,
       ProfileManager& profile_manager, SettingsManager& settings_manager, LrIpcOut& lr_ipc_out,
       MidiReceiver& midi_receiver, MidiSender& midi_sender);
   ~MainWindow() = default; // NOLINT(modernize-use-override)
   MainWindow(const MainWindow& other) = delete;
   MainWindow(MainWindow&& other) = delete;
   MainWindow& operator=(const MainWindow& other) = delete;
   MainWindow& operator=(MainWindow&& other) = delete;
   void SaveProfile() const { window_content_->SaveProfile(); }
   /* Note: Be careful if you override any DocumentWindow methods - the base class uses a lot of
    * them, so by overriding you might break its functionality. It's best to do all your work in
    * your content component instead, but if you really have to override any DocumentWindow methods,
    * make sure your subclass also calls the superclass's method. */
   [[nodiscard]] juce::String GetProfileName() const { return window_content_->GetProfileName(); }

 private:
   void closeButtonPressed() override
   {
      juce::JUCEApplication::getInstance()->systemRequestedQuit();
   }
   void timerCallback() override;

   std::unique_ptr<MainContentComponent> window_content_ {};
};

#endif
