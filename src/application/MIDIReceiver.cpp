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
#include "MIDIReceiver.h"

#include <chrono>
#include <exception>
#include <thread>
#include <utility>

#include <fmt/format.h>

#include "Devices.h"
#include "Misc.h"

#ifdef _WIN32
extern "C" {
   __declspec(dllimport) unsigned long __stdcall SetThreadExecutionState(
       _In_ unsigned long esFlags);
}
#endif

namespace {
   constexpr rsj::MidiMessage kTerminate {rsj::MessageType::kCc, 129, 0, 0}; /* impossible */
} // namespace

void MidiReceiver::Start()
{
   try {
      InitDevices();
      dispatch_messages_future_ = std::async(std::launch::async, [this] {
         rsj::LabelThread(MIDI2LR_UC_LITERAL("MidiReceiver dispatch messages thread"));
         MIDI2LR_FAST_FLOATS;
         DispatchMessages();
      });
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void MidiReceiver::Stop()
{
   for (const auto& dev : input_devices_) {
      dev->stop();
      rsj::Log(fmt::format(FMT_STRING("Stopped input device {}."), dev->getName().toStdString()),
          std::source_location::current());
   }
   if (const auto remaining {messages_.clear_count_push({kTerminate, nullptr})}) {
      rsj::Log(fmt::format(FMT_STRING("{} left in queue in MidiReceiver StopRunning."), remaining),
          std::source_location::current());
   }
}

void MidiReceiver::RescanDevices()
{
   try {
      for (const auto& dev : input_devices_) {
         dev->stop();
         rsj::Log(fmt::format(FMT_STRING("Stopped input device {}."), dev->getName().toStdString()),
             std::source_location::current());
      }
      input_devices_.clear();
      rsj::Log("Cleared input devices.", std::source_location::current());
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
   InitDevices(); /* initdevices has own try catch block */
}

void MidiReceiver::TryToOpen()
{
   try {
      const auto available_devices {juce::MidiInput::getAvailableDevices()};
      for (const auto& device : available_devices) {
         if (auto open_device {juce::MidiInput::openDevice(device.identifier, this)}) {
            if (devices_.EnabledOrNew(open_device->getDeviceInfo(), "input")) {
               open_device->start();
               rsj::Log(fmt::format(FMT_STRING("Opened input device {}."),
                            open_device->getName().toStdString()),
                   std::source_location::current());
               input_devices_.push_back(std::move(open_device));
            }
            else {
               rsj::Log(fmt::format(FMT_STRING("Ignored input device {}."),
                            open_device->getName().toStdString()),
                   std::source_location::current());
            }
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void MidiReceiver::InitDevices()
{
   using namespace std::literals::chrono_literals;
   try {
      for (int i = 0; i < 3; ++i) {
         rsj::Log("Trying to open input devices.", std::source_location::current());
         TryToOpen();
         if (!input_devices_.empty()) { break; }
         const auto sleep_duration = 20ms * (1 << i);
         rsj::Log(fmt::format(FMT_STRING("Retrying to open input devices after {}ms."),
                      sleep_duration.count()),
             std::source_location::current());
         std::this_thread::sleep_for(sleep_duration);
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void MidiReceiver::DispatchCcMessage(const std::pair<rsj::MidiMessage, juce::MidiInput*>& popped)
{
   try {
      if (const auto result {filters_[popped.second](popped.first)}; result.is_nrpn) {
         if (result.is_ready) {
            const rsj::MidiMessage nrpn_message {
                rsj::MessageType::kCc, popped.first.channel, result.control, result.value};
            for (const auto& cb : callbacks_) { cb(nrpn_message); }
         }
      }
      else {
         for (const auto& cb : callbacks_) { cb(popped.first); }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void MidiReceiver::DispatchNoteOnPwMessage(
    const std::pair<rsj::MidiMessage, juce::MidiInput*>& popped) const
{
   try {
      for (const auto& cb : callbacks_) { cb(popped.first); }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}

void MidiReceiver::DispatchMessages()
{
   try {
      decltype(messages_)::value_type popped;
      while ((popped = messages_.pop()).first != kTerminate) {
#ifdef _WIN32
         SetThreadExecutionState(0x00000002UL | 0x00000001UL);
#endif
         switch (popped.first.message_type_byte) {
         case rsj::MessageType::kCc:
            DispatchCcMessage(popped);
            break;
         case rsj::MessageType::kNoteOn:
         case rsj::MessageType::kPw:
            DispatchNoteOnPwMessage(popped);
            break;
         case rsj::MessageType::kChanPressure:
         case rsj::MessageType::kKeyPressure:
         case rsj::MessageType::kNoteOff:
         case rsj::MessageType::kPgmChange:
         case rsj::MessageType::kSystem:
            break; /* no action if other type of MIDI message */
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(e, std::source_location::current());
      throw;
   }
}