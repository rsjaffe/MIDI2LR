// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
  ==============================================================================

  CommandTable.cpp

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

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
#include "CommandTable.h"
#include "CommandTableModel.h"
#include <gsl/gsl_util>

CommandTable::CommandTable(const juce::String& component_name, CommandTableModel *model):
    juce::TableListBox{component_name, model} {
    setHeader(new juce::TableHeaderComponent{});
    getHeader().addColumn("MIDI Command", 1, 150, 30, -1,
        juce::TableHeaderComponent::notResizable | juce::TableHeaderComponent::sortable);
    getHeader().addColumn("LR Command", 2, 150, 30, -1,
        juce::TableHeaderComponent::notResizable | juce::TableHeaderComponent::sortable |
        juce::TableHeaderComponent::sortedForwards);
}

bool CommandTable::keyPressed(const KeyPress& k)
{
    if (k.isKeyCode(KeyPress::deleteKey) && getSelectedRow() != -1) {
        const auto last = getSelectedRow() == getNumRows() - 1;
        if (const auto ptr = dynamic_cast<CommandTableModel*>(getModel()))
            ptr->RemoveRow(gsl::narrow_cast<size_t>(getSelectedRow()));
        updateContent();
        if (last) // keep selection at the end
            selectRow(getNumRows() - 1);
        return true;
    }
    if (k.isKeyCode(KeyPress::downKey) && getSelectedRow() != -1 && getSelectedRow() < getNumRows() - 1) {
        selectRow(getSelectedRow() + 1);
        return true;
    }
    if (k.isKeyCode(KeyPress::upKey) && getSelectedRow() > 0 && getNumRows() > 1) {
        selectRow(getSelectedRow() - 1);
        return true;
    }
    if (k.isKeyCode(KeyPress::pageUpKey) && getNumRows() > 0) {
        auto row = getSelectedRow() - 20;
        if (row < 0)
            row = 0;
        selectRow(row);
        return true;
    }
    if (k.isKeyCode(KeyPress::pageDownKey) && getNumRows() > 0) {
        auto row = getSelectedRow() + 20;
        if (row >= getNumRows())
            row = getNumRows() - 1;
        selectRow(row);
        return true;
    }
    if (k.isKeyCode(KeyPress::homeKey) && getNumRows() > 0) {
        selectRow(0);
        return true;
    }
    if (k.isKeyCode(KeyPress::endKey) && getNumRows() > 0) {
        selectRow(getNumRows() - 1);
        return true;
    }
    return false;
}