#include "CommandTable.h"

CommandTable::CommandTable(const String& componentName, TableListBoxModel *model) : TableListBox(componentName, model)
{
    setHeader(new TableHeaderComponent());
    getHeader().addColumn("MIDI Command", 1, 150, 30, -1, TableHeaderComponent::notResizableOrSortable);
    getHeader().addColumn("LR Command", 2, 150, 30, -1, TableHeaderComponent::notResizableOrSortable);
}