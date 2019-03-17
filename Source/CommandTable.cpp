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

#include <exception>
#include <memory>
#include <unordered_map>

#include <gsl/gsl_util>
#include "CommandTableModel.h"
#include "Misc.h"

CommandTable::CommandTable(const juce::String& component_name, CommandTableModel* model) try : juce
   ::TableListBox{component_name, model}
   {
      auto head = std::make_unique<juce::TableHeaderComponent>();
      setHeader(head.release());
      getHeader().addColumn(TRANS("MIDI Command"), 1, 150, 30, -1,
          juce::TableHeaderComponent::notResizable | juce::TableHeaderComponent::sortable);
      getHeader().addColumn(TRANS("LR Command"), 2, 150, 30, -1,
          juce::TableHeaderComponent::notResizable | juce::TableHeaderComponent::sortable
              | juce::TableHeaderComponent::sortedForwards);
   }
catch (const std::exception& e) {
   rsj::ExceptionResponse(typeid(this).name(), __func__, e);
   throw;
}

bool CommandTable::keyPressed(const juce::KeyPress& k)
{
   try {
      static const std::unordered_map<int, int> kKeyToAction{{juce::KeyPress::deleteKey, 1},
          {juce::KeyPress::downKey, 2}, {juce::KeyPress::upKey, 3}, {juce::KeyPress::pageUpKey, 4},
          {juce::KeyPress::pageDownKey, 5}, {juce::KeyPress::homeKey, 6},
          {juce::KeyPress::endKey, 7}};
      if (const auto f = kKeyToAction.find(k.getKeyCode()); f != kKeyToAction.end()) {
         switch (f->second) {
         case 1: // deleteKey
            if (getSelectedRow() != -1) {
               const auto last = getSelectedRow() == getNumRows() - 1;
               if (const auto ptr = dynamic_cast<CommandTableModel*>(getModel()))
                  ptr->RemoveRow(gsl::narrow_cast<size_t>(getSelectedRow()));
               updateContent();
               if (last) // keep selection at the end
                  selectRow(getNumRows() - 1);
               return true;
            }
            return false;
         case 2: // downKey
            if (getSelectedRow() != -1 && getSelectedRow() < getNumRows() - 1) {
               selectRow(getSelectedRow() + 1);
               return true;
            }
            return false;
         case 3: // upKey
            if (getSelectedRow() > 0 && getNumRows() > 1) {
               selectRow(getSelectedRow() - 1);
               return true;
            }
            return false;
         case 4: // pageUpKey
            if (getNumRows() > 0) {
               auto row = getSelectedRow() - 20;
               if (row < 0)
                  row = 0;
               selectRow(row);
               return true;
            }
            return false;
         case 5: // pageDownKey
            if (getNumRows() > 0) {
               auto row = getSelectedRow() + 20;
               if (row >= getNumRows())
                  row = getNumRows() - 1;
               selectRow(row);
               return true;
            }
            return false;
         case 6: // homeKey
            if (getNumRows() > 0) {
               selectRow(0);
               return true;
            }
            return false;
         case 7: // endKey
            if (getNumRows() > 0) {
               selectRow(getNumRows() - 1);
               return true;
            }
            return false;
         }
      }
      return false;
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}