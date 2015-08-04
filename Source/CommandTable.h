#ifndef COMMANDTABLE_H_INCLUDED
#define COMMANDTABLE_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
class CommandTable : public TableListBox
{
public:
    CommandTable(const String& componentName, TableListBoxModel *model);

private:

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE(CommandTable)
};

#endif

