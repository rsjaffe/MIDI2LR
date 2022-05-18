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
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void MidiSender::Send(rsj::MidiMessageId id, int value) const
{
   try {
      if (id.msg_id_type == rsj::MessageType::kPw) {
         const auto msg {juce::MidiMessage::pitchWheel(id.channel, value)};
         for (const auto& dev : output_devices_) { dev->sendMessageNow(msg); }
      }
      else if (id.msg_id_type == rsj::MessageType::kNoteOn) {
         const auto msg {juce::MidiMessage::noteOn(
             id.channel, id.control_number, gsl::narrow_cast<juce::uint8>(value))};
         for (const auto& dev : output_devices_) { dev->sendMessageNow(msg); }
      }
      else if (id.msg_id_type == rsj::MessageType::kCc) {
         if (id.control_number < 128 && value < 128) {
            /* regular message */
            const auto msg {
                juce::MidiMessage::controllerEvent(id.channel, id.control_number, value)};
            for (const auto& dev : output_devices_) { dev->sendMessageNow(msg); }
         }
         else {
            /* NRPN */
            const auto msg_parm_msb {
                juce::MidiMessage::controllerEvent(id.channel, 99, id.control_number >> 7 & 0x7F)};
            const auto msg_parm_lsb {
                juce::MidiMessage::controllerEvent(id.channel, 98, id.control_number & 0x7f)};
            const auto msg_val_msb {
                juce::MidiMessage::controllerEvent(id.channel, 6, value >> 7 & 0x7F)};
            const auto msg_val_lsb {
                juce::MidiMessage::controllerEvent(id.channel, 38, value & 0x7f)};
            for (const auto& dev : output_devices_) {
               dev->sendMessageNow(msg_parm_msb);
               dev->sendMessageNow(msg_parm_lsb);
               dev->sendMessageNow(msg_val_msb);
               dev->sendMessageNow(msg_val_lsb);
            }
         }
      }
      else {
         constexpr auto msge {"MIDISender: Unexpected data type: {:n}."};
         const auto msgt {juce::translate(msge).toStdString()};
         rsj::LogAndAlertError(
             fmt::format(msgt, id.msg_id_type), fmt::format(msge, id.msg_id_type));
      }
   }

   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void MidiSender::RescanDevices()
{
   try {
      output_devices_.clear();
      rsj::Log("Cleared output devices.");
      InitDevices();
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

void MidiSender::InitDevices()
{
   try {
      const auto available_devices {juce::MidiOutput::getAvailableDevices()};
      for (const auto& device : available_devices) {
         if (auto open_device {juce::MidiOutput::openDevice(device.identifier)}) {
            const auto devname {open_device->getName().toStdString()};
            if constexpr (MSWindows) {
               if (devname != "Microsoft GS Wavetable Synth"
                   && devices_.EnabledOrNew(open_device->getDeviceInfo(), "output")) {
                  rsj::Log(fmt::format(FMT_STRING("Opened output device {}."), devname));
                  output_devices_.push_back(std::move(open_device));
               }
               else {
                  rsj::Log(fmt::format(FMT_STRING("Ignored output device {}."), devname));
               }
            }
            else {
               if (devices_.EnabledOrNew(open_device->getDeviceInfo(), "output")) {
                  rsj::Log(fmt::format(FMT_STRING("Opened output device {}."), devname));
                  output_devices_.push_back(std::move(open_device));
               }
               else {
                  rsj::Log(fmt::format(FMT_STRING("Ignored output device {}."), devname));
               }
            }
         }
      } /* devices that are skipped have their pointers deleted and are automatically closed*/
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}