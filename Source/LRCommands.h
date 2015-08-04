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
    static const std::vector<String> LRStringList;

    // Sectioned and readable strings
    static const std::vector<String> AdjustmentStringList;
    static const std::vector<String> ToneStringList;
    static const std::vector<String> MixerStringList;
    static const std::vector<String> SplitToningStringList;
    static const std::vector<String> DetailStringList;
    static const std::vector<String> EffectsStringList;
    static const std::vector<String> LensCorrectionStringList;
    static const std::vector<String> CalibrateStringList;

private:
    LRCommandList();
};

#endif  // LRCOMMANDS_H_INCLUDED
