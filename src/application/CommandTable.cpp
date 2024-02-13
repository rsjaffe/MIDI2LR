/*
 * This file is part of MIDI2LR. Copyright (C) 2015 by Rory Jaffe.
 *
 * MIDI2LR is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with MIDI2LR.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include "CommandTable.h"

#include <exception>
#include <memory>
#include <utility>

#include <gsl/util>

#include "CommandTableModel.h"
#include "Misc.h"

CommandTable::CommandTable(const juce::String& component_name, CommandTableModel* model)
try : juce
   ::TableListBox {component_name, model}
   {
      juce::TableListBox::setHeader(std::make_unique<juce::TableHeaderComponent>());
      juce::TableListBox::getHeader().addColumn(juce::translate("MIDI Command"), 1, 150, 30, -1,
          juce::TableHeaderComponent::notResizable | juce::TableHeaderComponent::sortable);
      juce::TableListBox::getHeader().addColumn(juce::translate("LR Command"), 2, 350, 30, -1,
          juce::TableHeaderComponent::notResizable | juce::TableHeaderComponent::sortable
              | juce::TableHeaderComponent::sortedForwards);
   }

catch (const std::exception& e) {
   rsj::ExceptionResponse(e);
   throw;
}

bool CommandTable::keyPressed(const juce::KeyPress& k)
{
   try {
      const auto key_pressed {k.getKeyCode()};
      if (key_pressed == juce::KeyPress::deleteKey) {
         if (juce::ListBox::getSelectedRow() != -1) {
            const auto last {
                juce::ListBox::getSelectedRow() == juce::TableListBox::getNumRows() - 1};
            if (const auto ptr {
                    dynamic_cast<CommandTableModel*>(juce::TableListBox::getTableListBoxModel())}) {
               ptr->RemoveRow(gsl::narrow_cast<size_t>(juce::ListBox::getSelectedRow()));
            }
            juce::ListBox::updateContent();
            if (last) { /* keep selection at the end */
               juce::ListBox::selectRow(getNumRows() - 1);
            }
            return true;
         }
         return false;
      }
      if (key_pressed == juce::KeyPress::downKey) {
         if (juce::ListBox::getSelectedRow() != -1
             && juce::ListBox::getSelectedRow() < juce::TableListBox::getNumRows() - 1) {
            juce::ListBox::selectRow(getSelectedRow() + 1);
            return true;
         }
         return false;
      }
      if (key_pressed == juce::KeyPress::upKey) {
         if (juce::ListBox::getSelectedRow() > 0 && juce::TableListBox::getNumRows() > 1) {
            juce::ListBox::selectRow(getSelectedRow() - 1);
            return true;
         }
         return false;
      }
      if (key_pressed == juce::KeyPress::pageUpKey) {
         if (juce::TableListBox::getNumRows() > 0) {
            juce::ListBox::selectRow(std::max(juce::ListBox::getSelectedRow() - 20, 0));
            return true;
         }
         return false;
      }
      if (key_pressed == juce::KeyPress::pageDownKey) {
         if (juce::TableListBox::getNumRows() > 0) {
            juce::ListBox::selectRow(std::min(juce::ListBox::getSelectedRow() + 20,
                juce::TableListBox::getNumRows() - 1));
            return true;
         }
         return false;
      }
      if (key_pressed == juce::KeyPress::homeKey) {
         if (juce::TableListBox::getNumRows() > 0) {
            juce::ListBox::selectRow(0);
            return true;
         }
         return false;
      }
      if (key_pressed == juce::KeyPress::endKey) {
         if (juce::TableListBox::getNumRows() > 0) {
            juce::ListBox::selectRow(juce::TableListBox::getNumRows() - 1);
            return true;
         }
         return false;
      }
      return false;
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e);
      throw;
   }
}