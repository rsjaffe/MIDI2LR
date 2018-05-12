#pragma once
/*
  ==============================================================================

    VersionChecker.h

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
#ifndef MIDI2LR_VERSIONCHECKER_H_INCLUDED
#define MIDI2LR_VERSIONCHECKER_H_INCLUDED

#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"
class SettingsManager;

class VersionChecker final : public juce::Thread, juce::AsyncUpdater {
 public:
   explicit VersionChecker(SettingsManager* settings_manager) noexcept;
   ~VersionChecker();
   VersionChecker(const VersionChecker& other) = delete;
   VersionChecker(VersionChecker&& other) = delete;
   VersionChecker& operator=(const VersionChecker& other) = delete;
   VersionChecker& operator=(VersionChecker&& other) = delete;

 private:
   // Thread interface
   void run() override;

   // AsyncUpdater interface
   void handleAsyncUpdate() override;

   int new_version_{0};
   SettingsManager* const settings_manager_;
   std::unique_ptr<juce::DialogWindow> dialog_;
};

#endif // VERSIONCHECKER_H_INCLUDED
