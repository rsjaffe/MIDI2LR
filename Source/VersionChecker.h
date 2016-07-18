#pragma once
/*
  ==============================================================================

    VersionChecker.h

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
#ifndef VERSIONCHECKER_H_INCLUDED
#define VERSIONCHECKER_H_INCLUDED

#include <memory>
#include "../JuceLibraryCode/JuceHeader.h"
#include "SettingsManager.h"

class VersionChecker final: public juce::Thread, private juce::AsyncUpdater {
public:
  VersionChecker() noexcept;
  ~VersionChecker();

  void Init(std::weak_ptr<SettingsManager>&& profile_manager) noexcept;

private:
  // Thread interface
  virtual void run() override;

  // AsyncUpdater interface
  virtual void handleAsyncUpdate() override;

  int new_version_;
  std::weak_ptr<SettingsManager> settings_manager_;
  std::unique_ptr<juce::DialogWindow> dialog_;
};

#endif  // VERSIONCHECKER_H_INCLUDED
