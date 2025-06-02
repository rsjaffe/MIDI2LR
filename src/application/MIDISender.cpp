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
#include "MIDISender.h"

#include <exception>
#include <utility>

#include <fmt/format.h>
#include <gsl/gsl>

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>

#include "Devices.h"
#include "MidiUtilities.h"
#include "Misc.h"

void MidiSender::Start()
{
   try {
      InitDevices();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

namespace {
   void LogUnexpectedDataType(rsj::MidiMessageId id)
   {
      constexpr auto msge {"MIDISender: Unexpected data type: {:n}."};
      const auto msgt {juce::translate(msge).toStdString()};
      rsj::LogAndAlertError(fmt::format(fmt::runtime_format_string<>(msgt), id.msg_id_type),
          fmt::format(fmt::runtime_format_string<>(msge), id.msg_id_type),
          std::source_location::current());
   }
} // namespace

void MidiSender::Send(rsj::MidiMessageId id, int value) const
{
   try {
      switch (id.msg_id_type) {
      case rsj::MessageType::kCc:
         SendControllerEvent(id, value);
         break;
      case rsj::MessageType::kNoteOn:
         SendNoteOn(id, value);
         break;
      case rsj::MessageType::kPw:
         SendPitchWheel(id, value);
         break;
      default:
         LogUnexpectedDataType(id);
         break;
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void MidiSender::SendPitchWheel(rsj::MidiMessageId id, int value) const
{
   const auto msg {juce::MidiMessage::pitchWheel(id.channel, value)};
   for (const auto& dev : output_devices_) { dev->sendMessageNow(msg); }
}

void MidiSender::SendNoteOn(rsj::MidiMessageId id, int value) const
{
   const auto msg {juce::MidiMessage::noteOn(id.channel, id.control_number,
       gsl::narrow_cast<juce::uint8>(value))};
   for (const auto& dev : output_devices_) { dev->sendMessageNow(msg); }
}

void MidiSender::SendControllerEvent(rsj::MidiMessageId id, int value) const
{
   if (id.control_number < 128 && value < 128) {
      /* regular message */
      const auto msg {juce::MidiMessage::controllerEvent(id.channel, id.control_number, value)};
      for (const auto& dev : output_devices_) { dev->sendMessageNow(msg); }
   }
   else {
      /* NPRN */
      SendNrpn(id, value);
   }
}

void MidiSender::SendNrpn(rsj::MidiMessageId id, int value) const
{
   const auto msg_parm_msb {
       juce::MidiMessage::controllerEvent(id.channel, 99, id.control_number >> 7 & 0x7F)};
   const auto msg_parm_lsb {
       juce::MidiMessage::controllerEvent(id.channel, 98, id.control_number & 0x7f)};
   const auto msg_val_msb {juce::MidiMessage::controllerEvent(id.channel, 6, value >> 7 & 0x7F)};
   const auto msg_val_lsb {juce::MidiMessage::controllerEvent(id.channel, 38, value & 0x7f)};
   for (const auto& dev : output_devices_) {
      dev->sendMessageNow(msg_parm_msb);
      dev->sendMessageNow(msg_parm_lsb);
      dev->sendMessageNow(msg_val_msb);
      dev->sendMessageNow(msg_val_lsb);
   }
}

void MidiSender::RescanDevices()
{
   try {
      output_devices_.clear();
      rsj::Log("Cleared output devices.", std::source_location::current());
      InitDevices();
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

bool MidiSender::ShouldOpenDevice(const std::string& devname, const auto& open_device)
{
   if constexpr (MSWindows) {
      return devname != "Microsoft GS Wavetable Synth"
             && devices_.EnabledOrNew(open_device->getDeviceInfo(), "output");
   }
   else {
      return devices_.EnabledOrNew(open_device->getDeviceInfo(), "output");
   }
}

void MidiSender::InitDevices()
{
   try {
      const auto available_devices {juce::MidiOutput::getAvailableDevices()};
      for (const auto& device : available_devices) {
         if (auto open_device {juce::MidiOutput::openDevice(device.identifier)}) {
            const auto devname {open_device->getName().toStdString()};
            if (ShouldOpenDevice(devname, open_device)) {
               rsj::Log(fmt::format("Opened output device {}.", devname),
                   std::source_location::current());
               output_devices_.push_back(std::move(open_device));
            }
            else {
               rsj::Log(fmt::format("Ignored output device {}.", devname),
                   std::source_location::current());
            }
         }
      } /* devices that are skipped have their pointers deleted and are automatically closed*/
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}