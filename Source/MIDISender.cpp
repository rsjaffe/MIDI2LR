/*
  ==============================================================================

    MIDISender.cpp

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
#include "MIDISender.h"

#include <exception>

#include <gsl/gsl>
#include <JuceLibraryCode/JuceHeader.h>
#include "MidiUtilities.h"
#include "Misc.h"

void MidiSender::StartRunning()
{
   try {
      InitDevices();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void MidiSender::Send(rsj::MidiMessageId id, int value) const
{
   try {
      if (id.msg_id_type == rsj::MessageType::Pw)
         for (const auto& dev : output_devices_)
            dev->sendMessageNow(juce::MidiMessage::pitchWheel(id.channel, value));
      else if (id.msg_id_type == rsj::MessageType::NoteOn)
         for (const auto& dev : output_devices_)
            dev->sendMessageNow(juce::MidiMessage::noteOn(
                id.channel, id.control_number, gsl::narrow_cast<juce::uint8>(value)));
      else if (id.msg_id_type == rsj::MessageType::Cc) {
         if (id.control_number < 128) { // regular message
            for (const auto& dev : output_devices_)
               dev->sendMessageNow(
                   juce::MidiMessage::controllerEvent(id.channel, id.control_number, value));
         }
         else { // NRPN
            const auto parameter_lsb = id.control_number & 0x7f;
            const auto parameter_msb = id.control_number >> 7 & 0x7F;
            const auto value_lsb = value & 0x7f;
            const auto value_msb = value >> 7 & 0x7F;
            for (const auto& dev : output_devices_) {
               dev->sendMessageNow(
                   juce::MidiMessage::controllerEvent(id.channel, 99, parameter_msb));
               dev->sendMessageNow(
                   juce::MidiMessage::controllerEvent(id.channel, 98, parameter_lsb));
               dev->sendMessageNow(juce::MidiMessage::controllerEvent(id.channel, 6, value_msb));
               dev->sendMessageNow(juce::MidiMessage::controllerEvent(id.channel, 38, value_lsb));
            }
         }
      }
      else
         rsj::LogAndAlertError(juce::String("MIDISender had unexpected MessageType: ")
                               + rsj::MessageTypeToName(id.msg_id_type));
   }

   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void MidiSender::RescanDevices()
{
   try {
      output_devices_.clear();
      rsj::Log("Cleared output devices");
      InitDevices();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void MidiSender::InitDevices()
{
   try {
      auto available_devices = juce::MidiOutput::getAvailableDevices();
      for (auto&& device : available_devices) {
         auto open_device = juce::MidiOutput::openDevice(device.identifier);
         if (open_device) {
            rsj::Log("Opened output device " + open_device->getName());
            output_devices_.emplace_back(std::move(open_device));
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}