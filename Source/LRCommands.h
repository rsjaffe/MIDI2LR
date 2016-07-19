#pragma once
/*
  ==============================================================================

    LRCommands.h

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
#ifndef LRCOMMANDS_H_INCLUDED
#define LRCOMMANDS_H_INCLUDED

#include <string>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"

class LRCommandList {
public:
    // Strings that LR uses
  static const std::vector<std::string> LRStringList;

  // Sectioned and readable develop param strings
  static const std::vector<std::string> KeyShortcuts;
  static const std::vector<std::string> General;
  static const std::vector<std::string> Library;
  static const std::vector<std::string> Develop;
  static const std::vector<std::string> BasicAdjustments;
  static const std::vector<std::string> ToneCurve;
  static const std::vector<std::string> Mixer;
  static const std::vector<std::string> ResetMixer;
  static const std::vector<std::string> SplitToning;
  static const std::vector<std::string> Detail;
  static const std::vector<std::string> LensCorrections;
  static const std::vector<std::string> Effects;
  static const std::vector<std::string> Calibration;
  static const std::vector<std::string> DevelopPresets;
  static const std::vector<std::string> LocalAdjustments;
  static const std::vector<std::string> Crop;
  static const std::vector<std::string> ToolModulePanel;
  static const std::vector<std::string> SecondaryDisplay;
  static const std::vector<std::string> ProgramProfiles;

  // MIDI2LR commands
  static const std::vector<std::string> NextPrevProfile;

  // Map of command strings to indices
  static int getIndexOfCommand(const std::string& command);

private:
  LRCommandList() noexcept;
};

#endif  // LRCOMMANDS_H_INCLUDED
