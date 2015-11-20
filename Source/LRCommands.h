/*
  ==============================================================================

    LRCommands.h
    Created: 31 Jul 2015 10:21:12pm
    Author:  Parth

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
    static std::vector<String> LRStringList;

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
    static std::vector<String> PresetsList;

    // Sectioned and readable LR tool string
    static const std::vector<String> SelectionList;
    static const std::vector<String> ToolsList;
    static const std::vector<String> ModulesList;
    static const std::vector<String> ViewModesList;
    // MIDI2LR commands
    static const std::vector<String> ProfileList;

    // Map of command strings to indices
    static int getIndexOfCommand(const String& command);

    //methods to update presets listing from lightroom
    static void ClearPresets();
    static void AddPreset(String ID, String presetName);

private:
    LRCommandList();
    static size_t baseSize;
};

#endif  // LRCOMMANDS_H_INCLUDED
