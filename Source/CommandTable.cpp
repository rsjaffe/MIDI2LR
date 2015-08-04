#include "CommandTable.h"

CommandTable::CommandTable(const String& componentName, TableListBoxModel *model) : TableListBox(componentName, model)
{
    setHeader(new TableHeaderComponent());
    getHeader().addColumn("MIDI Command", 1, 125);
    getHeader().addColumn("LR Command", 2, 125);
}