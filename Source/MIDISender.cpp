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
#include "Misc.h"

void MidiSender::Init()
{
   try {
      InitDevices();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void MidiSender::SendCc(int midi_channel, int controller, int value) const
{
   try {
      if (controller < 128) { // regular message
         for (const auto& dev : output_devices_)
            dev->sendMessageNow(
                juce::MidiMessage::controllerEvent(midi_channel, controller, value));
      }
      else { // NRPN
         const auto parameter_lsb = controller & 0x7f;
         const auto parameter_msb = (controller >> 7) & 0x7F;
         const auto value_lsb = value & 0x7f;
         const auto value_msb = (value >> 7) & 0x7F;
         for (const auto& dev : output_devices_) {
            dev->sendMessageNow(
                juce::MidiMessage::controllerEvent(midi_channel, 99, parameter_msb));
            dev->sendMessageNow(
                juce::MidiMessage::controllerEvent(midi_channel, 98, parameter_lsb));
            dev->sendMessageNow(juce::MidiMessage::controllerEvent(midi_channel, 6, value_msb));
            dev->sendMessageNow(juce::MidiMessage::controllerEvent(midi_channel, 38, value_lsb));
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void MidiSender::SendNoteOn(int midi_channel, int controller, int value) const
{
   try {
      for (const auto& dev : output_devices_)
         dev->sendMessageNow(juce::MidiMessage::noteOn(
             midi_channel, controller, gsl::narrow_cast<juce::uint8>(value)));
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void MidiSender::SendPitchWheel(int midi_channel, int value) const
{
   try {
      for (const auto& dev : output_devices_)
         dev->sendMessageNow(juce::MidiMessage::pitchWheel(midi_channel, value));
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
      for (auto idx = 0; idx < juce::MidiOutput::getDevices().size(); ++idx) {
         auto dev = juce::MidiOutput::openDevice(idx);
         if (dev) {
            output_devices_.emplace_back(dev);
            rsj::Log("Opened output device " + dev->getName());
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}