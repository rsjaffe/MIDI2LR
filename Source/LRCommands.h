/*
  ==============================================================================

	LRCommands.h
	Created: 31 Jul 2015 10:21:12pm
	Author:  Parth, Jaffe

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe, derived from code
by Parth.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.  
  ==============================================================================
*/
#ifndef LRCOMMANDS_H_INCLUDED
#define LRCOMMANDS_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>

class LRCommandList
{
public:
	// Strings that LR uses
	static const std::vector<String> LRStringList;

	// Sectioned and readable develop param strings
	static const std::vector<String> AdjustmentStringList;
	static const std::vector<String> ToneStringList;
	static const std::vector<String> MixerStringList;
	static const std::vector<String> ResetMixerStringList;
	static const std::vector<String> SplitToningStringList;
	static const std::vector<String> DetailStringList;
	static const std::vector<String> EffectsStringList;
	static const std::vector<String> LensCorrectionStringList;
	static const std::vector<String> CalibrateStringList;

	// Sectioned and readable LR tool string
	static const std::vector<String> SelectionList;
    static const std::vector<String> PresetsList;
	static const std::vector<String> ToolsList;
	static const std::vector<String> ModulesList;
    static const std::vector<String> ViewModesList;
    static const std::vector<String> ProfilesList;
    // MIDI2LR commands
	static const std::vector<String> NextPrevProfile;

	// Map of command strings to indices
	static int getIndexOfCommand(const String& command);

private:
	LRCommandList();
};

#endif  // LRCOMMANDS_H_INCLUDED
