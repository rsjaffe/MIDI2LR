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
#include "CommandTableModel.h"

#include <algorithm>
#include <exception>
#include <utility>

#include <fmt/format.h>
#include <gsl/gsl>

#include "CommandMenu.h"
#include "Misc.h"

CommandTableModel::CommandTableModel(const CommandSet& command_set, Profile& profile) noexcept
    : command_set_ {command_set}, profile_ {profile}
{
}

void CommandTableModel::paintCell(juce::Graphics& g, int row_number, const int column_id,
    const int width, const int height, bool /*rowIsSelected*/)
{
   /* This must draw one of the cells. The graphics context's origin will already be set to the
    * top-left of the cell, whose size is specified by(width, height). Note that the rowNumber value
    * may be greater than the number of rows in your list, so be careful that you don't assume it's
    * less than getNumRows(). */
   try {
      g.setColour(juce::Colours::black);
      g.setFont(std::min(16.0f, static_cast<float>(height) * 0.7f));
      if (column_id == 1) {
         /* write the MIDI message in the MIDI command column */
         if (rsj::cmp_less_equal(profile_.Size(), row_number)) {
            /* error condition */
            g.drawText("Unknown control", 0, 0, width, height, juce::Justification::centred);
            rsj::Log(fmt::format(FMT_STRING("Unknown control CommandTableModel::paintCell. {} rows "
                                            "in profile, row number to be painted is {}."),
                profile_.Size(), row_number));
         }
         else {
            std::string format_str;
            switch (
                const auto cmd {profile_.GetMessageForNumber(gsl::narrow_cast<size_t>(row_number))};
                cmd.msg_id_type) {
            case rsj::MessageType::NoteOn:
               format_str =
                   fmt::format(FMT_STRING("{} | Note : {}"), cmd.channel, cmd.control_number);
               break;
            case rsj::MessageType::NoteOff:
               format_str =
                   fmt::format(FMT_STRING("{} | Note Off: {}"), cmd.channel, cmd.control_number);
               break;
            case rsj::MessageType::Cc:
               format_str = fmt::format(FMT_STRING("{} | CC: {}"), cmd.channel, cmd.control_number);
               break;
            case rsj::MessageType::Pw:
               format_str = fmt::format(FMT_STRING("{} | Pitch Bend"), cmd.channel);
               break;
            case rsj::MessageType::KeyPressure:
               format_str = fmt::format(
                   FMT_STRING("{} | Key Pressure: {}"), cmd.channel, cmd.control_number);
               break;
            case rsj::MessageType::ChanPressure:
               format_str = fmt::format(FMT_STRING("{} | Channel Pressure"), cmd.channel);
               break;
            case rsj::MessageType::PgmChange:
               format_str = fmt::format(FMT_STRING("{} | Program Change"), cmd.channel);
               break;
            case rsj::MessageType::System:
               break;
            }
            g.drawText(format_str, 0, 0, width, height, juce::Justification::centredLeft);
         }
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void CommandTableModel::paintRowBackground(
    juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, const bool row_is_selected)
{
   /* This must draw the background behind one of the rows in the table. The graphics context has
    * its origin at the row's top-left, and your method should fill the area specified by the width
    * and height parameters. Note that the rowNumber value may be greater than the number of rows in
    * your list, so be careful that you don't assume it's less than getNumRows(). */
   try {
      if (row_is_selected)
         g.fillAll(juce::Colours::lightblue);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

juce::Component* CommandTableModel::refreshComponentForCell(int row_number, const int column_id,
    bool /*isRowSelected*/, juce::Component* existing_component)
{
   /* This is used to create or update a custom component to go in a cell. Any cell may contain a
    * custom component, or can just be drawn with the paintCell() method and handle mouse clicks
    * with cellClicked(). This method will be called whenever a custom component might need to be
    * updated - e.g. when the table is changed, or TableListBox::updateContent() is called. If you
    * don't need a custom component for the specified cell, then return nullptr. (Bear in mind that
    * even if you're not creating a new component, you may still need to delete
    * existingComponentToUpdate if it's non-null). If you do want a custom component, and the
    * existingComponentToUpdate is null, then this method must create a new component suitable for
    * the cell, and return it. If the existingComponentToUpdate is non - null, it will be a pointer
    * to a component previously created by this method.In this case, the method must either update
    * it to make sure it's correctly representing the given cell(which may be different from the one
    * that the component was created for), or it can delete this component and return a new one.
    * Because Juce recycles these components when scrolling, we need to reset their properties. */
   try {
      if (column_id == 2) /* LR command column */
      {
         auto command_select {dynamic_cast<CommandMenu*>(existing_component)};
         if (command_select == nullptr) {
            /* create a new command menu, delete old one if it exists */
            delete existing_component; // NOLINT(cppcoreguidelines-owning-memory)
            auto new_select {std::make_unique<CommandMenu>(
                profile_.GetMessageForNumber(gsl::narrow_cast<size_t>(row_number)), command_set_,
                profile_)};
            new_select->SetSelectedItem(
                command_set_.CommandTextIndex(profile_.GetCommandForMessage(
                    profile_.GetMessageForNumber(gsl::narrow_cast<size_t>(row_number))))
                + 1);
            return new_select.release();
         }
         /* change old command menu */
         command_select->SetMsg(profile_.GetMessageForNumber(gsl::narrow_cast<size_t>(row_number)));
         command_select->SetSelectedItem(
             command_set_.CommandTextIndex(profile_.GetCommandForMessage(
                 profile_.GetMessageForNumber(gsl::narrow_cast<size_t>(row_number))))
             + 1);
         return command_select;
      }
      return nullptr;
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void CommandTableModel::sortOrderChanged(int new_sort_column_id, bool is_forwards)
{
   try {
      const auto current_sort {std::make_pair(new_sort_column_id, is_forwards)};
      profile_.Resort(current_sort);
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}