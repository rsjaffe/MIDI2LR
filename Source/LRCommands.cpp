/*
  ==============================================================================

    LRCommands.cpp
    Created: 31 Jul 2015 10:21:12pm
    Author:  Parth

  ==============================================================================
*/

#include "LRCommands.h"

/*
Adjustment
"Temperature",
"Tint",
"Exposure",
"Highlights", 	(controls Recovery in PV2003 and PV2010)
"Shadows", 	(controls Fill Light in PV2003 and PV2010)
"Brightness", (no effect unless in PV2003 or PV2010)
"Contrast",
"Whites", 	(no effect in PV2003 and PV2010)
"Blacks",
"Clarity",
"Vibrance",
"Saturation",


*/

const std::vector<String> LRCommandList::StringList = {
	" ",
	"Temperature",
	"Tint",
	"Exposure",
	"Highlights",
	"Shadows",
	"Contrast",
	"Whites",
	"Blacks",
	"Clarity",
	"Vibrance",
	"Saturation",
};

const std::vector<String> LRCommandList::LRStringList = {
	" ",
	"Temperature",
	"Tint",
	"Exposure",
	"Highlights",
	"Shadows",
	"Contrast",
	"Whites",
	"Blacks",
	"Clarity",
	"Vibrance",
	"Saturation",
};