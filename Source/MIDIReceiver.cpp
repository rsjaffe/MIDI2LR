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

#include "Misc.h"

namespace {
   constexpr rsj::MidiMessage kTerminate{rsj::MessageType::Cc, 129, 0, 0}; /* impossible channel */
}

void MidiReceiver::Start()
{
   try {
      InitDevices();
      dispatch_messages_future_ = std::async(std::launch::async, [this] {
         rsj::LabelThread(L"MidiReceiver dispatch messages thread");
         DispatchMessages();
      });
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void MidiReceiver::Stop()
{
   for (const auto& dev : devices_) {
      dev->stop();
      rsj::Log("Stopped input device " + dev->getName());
   }
   if (const auto remaining = messages_.clear_count_push(kTerminate))
      rsj::Log(juce::String(remaining) + " left in queue in MidiReceiver StopRunning");
   callbacks_.clear(); /* after queue emptied */
}

void MidiReceiver::handleIncomingMidiMessage(
    juce::MidiInput* device, const juce::MidiMessage& message)
{
   /* reentrant ok. NRPN filter contains mutex in case of concurrency. This procedure is in
    * near-real-time, so must return quickly. will place message in multithreaded queue and let
    * separate process handle the messages */
   try {
      const rsj::MidiMessage mess{message};
      switch (mess.message_type_byte) {
      case rsj::MessageType::Cc: {
         const auto result = filters_[device](mess);
         if (result.is_nrpn) {
            /* send when complete */
            if (result.is_ready)
               messages_.emplace(rsj::MessageType::Cc, mess.channel, result.control, result.value);
            /* finished with nrpn piece */
            break;
         }
      }
         /* if not nrpn, handle like other messages */
         [[fallthrough]];
      case rsj::MessageType::NoteOn:
      case rsj::MessageType::Pw:
         messages_.push(mess);
         break;
      default:
          /* no action if other type of MIDI message */;
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void MidiReceiver::RescanDevices()
{
   try {
      for (const auto& dev : devices_) {
         dev->stop();
         rsj::Log("Stopped input device " + dev->getName());
      }
      devices_.clear();
      rsj::Log("Cleared input devices");
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
   InitDevices(); /* initdevices has own try catch block */
}

void MidiReceiver::TryToOpen()
{
   try {
      const auto available_devices{juce::MidiInput::getAvailableDevices()};
      for (const auto& device : available_devices) {
         auto open_device{juce::MidiInput::openDevice(device.identifier, this)};
         if (open_device) {
            open_device->start();
            rsj::Log("Opened input device " + open_device->getName());
            devices_.emplace_back(std::move(open_device));
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void MidiReceiver::InitDevices()
{
   using namespace std::literals::chrono_literals;
   try {
      rsj::Log("Trying to open input devices");
      TryToOpen();
      if (devices_.empty()) /* encountering errors first try on MacOS */
      {
         rsj::Log("Retrying to open input devices");
         rsj::SleepTimedLogged("Open input devices", 20ms);
         TryToOpen();
         if (devices_.empty()) /* encountering errors second try on MacOS */
         {
            rsj::Log("Retrying second time to open input devices");
            rsj::SleepTimedLogged("Open input devices", 80ms);
            TryToOpen();
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void MidiReceiver::DispatchMessages()
{
   try {
      do {
         const auto message_copy = messages_.pop();
         if (message_copy == kTerminate)
            return;
         for (const auto& cb : callbacks_)
#pragma warning(suppress : 26489)
            /* false warning, checked for existence before adding to callbacks_ */
            cb(message_copy);
      } while (true);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}