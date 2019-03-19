#ifndef MIDI2LR_MAINWINDOW_H_INCLUDED
#define MIDI2LR_MAINWINDOW_H_INCLUDED
/*
  ==============================================================================

    MainWindow.h

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
#include <memory>

#include <JuceLibraryCode/JuceHeader.h>
class LrIpcOut;
class MainContentComponent;
class MidiReceiver;
class MidiSender;
class Profile;
class ProfileManager;
class SettingsManager;

class MainWindow final : juce::DocumentWindow, juce::Timer {
 public:
   MainWindow(const juce::String& name, Profile& profile, ProfileManager& profile_manager,
       SettingsManager& settings_manager, std::weak_ptr<LrIpcOut>&& lr_ipc_out,
       std::shared_ptr<MidiReceiver> midi_receiver, std::shared_ptr<MidiSender> midi_sender);
   ~MainWindow() = default;
   MainWindow(const MainWindow& other) = delete;
   MainWindow(MainWindow&& other) = delete;
   MainWindow& operator=(const MainWindow& other) = delete;
   MainWindow& operator=(MainWindow&& other) = delete;
   void SaveProfile() const;
   /* Note: Be careful if you override any DocumentWindow methods - the base
      class uses a lot of them, so by overriding you might break its functionality.
      It's best to do all your work in your content component instead, but if
      you really have to override any DocumentWindow methods, make sure your
      subclass also calls the superclass's method.
   */

 private:
   void closeButtonPressed() override
   {
      // This is called when the user tries to close this window. Here, we'll just
      // ask the app to quit when this happens, but you can change this to do
      // whatever you need.
      juce::JUCEApplication::getInstance()->systemRequestedQuit();
   }
   MainContentComponent* window_content_;
   // the timer callback function
   void timerCallback() override;
};

#endif // MAINWINDOW_H_INCLUDED
