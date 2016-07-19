/*
  ==============================================================================

  CommandTable.cpp

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
#include "CommandTable.h"

CommandTable::CommandTable(const juce::String& component_name, juce::TableListBoxModel *model):
  juce::TableListBox{component_name, model} {
  setHeader(new juce::TableHeaderComponent{});
  getHeader().addColumn("MIDI Command", 1, 150, 30, -1,
    juce::TableHeaderComponent::notResizable | juce::TableHeaderComponent::sortable);
  getHeader().addColumn("LR Command", 2, 150, 30, -1,
    juce::TableHeaderComponent::notResizable | juce::TableHeaderComponent::sortable |
    juce::TableHeaderComponent::sortedForwards);
}