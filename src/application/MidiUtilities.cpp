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
#include "MidiUtilities.h"

#include <gsl/gsl>

#include <JuceLibraryCode/JuceHeader.h>

rsj::MidiMessage::MidiMessage(const juce::MidiMessage& mm)
{
   /* anything not set below is set to zero by default constructor */
#pragma warning(push)
#pragma warning(disable : 26481) /* doing raw pointer arithmetic, parsing low-level structure */
   // ReSharper disable CppClangTidyCppcoreguidelinesProBoundsPointerArithmetic
   const auto raw {mm.getRawData()};
   Ensures(raw);
   if (rsj::ValidMessageType(raw[0])) {
      message_type_byte = rsj::ToMessageType(raw[0]);
      channel = raw[0] & 0xF;
      switch (message_type_byte) {
      case MessageType::Pw:
         value = raw[2] << 7 | raw[1];
         break;
      case MessageType::Cc:
      case MessageType::KeyPressure:
      case MessageType::NoteOff:
      case MessageType::NoteOn:
         value = raw[2];
         control_number = raw[1];
         break;
      case MessageType::PgmChange:
         control_number = raw[1];
         break;
      case MessageType::ChanPressure:
         value = raw[1];
         break;
      case MessageType::System:
         break; /* no action */
      }
   }
   else
      message_type_byte = MessageType::System;
#pragma warning(pop)
}