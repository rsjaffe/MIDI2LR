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
	static const std::vector<String> StringList;
	static const std::vector<String> LRStringList;

private:
	LRCommandList();
};

#endif  // LRCOMMANDS_H_INCLUDED
