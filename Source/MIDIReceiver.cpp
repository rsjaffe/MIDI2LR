/*
  ==============================================================================

    MIDIProcessor.cpp

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
#include "MIDIReceiver.h"

#include <chrono>

namespace {
   constexpr rsj::MidiMessage kTerminate{rsj::MessageType::Cc, 129, 0, 0}; // impossible channel
}

#pragma warning(push)
#pragma warning(disable : 4297 26447)
MidiReceiver::~MidiReceiver() try {
   for (const auto& dev : devices_) {
      dev->stop();
      rsj::Log("Stopped input device " + dev->getName());
   }
   if (const auto remaining = messages_.clear_count_push(kTerminate))
      rsj::Log(juce::String(remaining) + " left in queue in MidiReceiver destructor");
}
catch (const std::exception& e) {
   rsj::ExceptionResponse(typeid(this).name(), __func__, e);
   return; // The program is ending anyway. CERT C++ Coding Standard DCL57-CPP.
}
catch (...) {
   rsj::LogAndAlertError("Exception in MidiReceiver Destructor. Non-standard exception.");
   return; // The program is ending anyway. CERT C++ Coding Standard DCL57-CPP.
}
#pragma warning(pop)

void MidiReceiver::StartRunning()
{
   try {
      InitDevices();
      dispatch_messages_future_ =
          std::async(std::launch::async, &MidiReceiver::DispatchMessages, this);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void MidiReceiver::handleIncomingMidiMessage(
    juce::MidiInput* device, const juce::MidiMessage& message)
{
   try {
      // this procedure is in near-real-time, so must return quickly.
      // will place message in multithreaded queue and let separate process handle the messages
      const rsj::MidiMessage mess{message};
      switch (mess.message_type_byte) {
      case rsj::MessageType::Cc: {
         NrpnFilter::ProcessResult result{};
         {
            auto lock = std::scoped_lock(filter_mutex_);
            result = filters_[device](mess);
         }
         if (result.is_nrpn) {
            if (result.is_ready) { // send when finished
               messages_.emplace(rsj::MessageType::Cc, mess.channel, result.control, result.value);
            }
            break; // finished with nrpn piece
         }
      }
         [[fallthrough]]; // if not nrpn, handle like other messages
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
   InitDevices(); // initdevices has own try catch block
}

void MidiReceiver::TryToOpen()
{
   try {
      for (auto idx = 0; idx < juce::MidiInput::getDevices().size(); ++idx) {
         const auto dev = juce::MidiInput::openDevice(idx, this);
         if (dev) {
            devices_.emplace_back(dev);
            dev->start();
            rsj::Log("Opened input device " + dev->getName());
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
      if (devices_.empty()) // encountering errors first try on MacOS
      {
         rsj::Log("Retrying to open input devices");
         rsj::SleepTimedLogged("Open input devices", 20ms);
         TryToOpen();
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
         auto message_copy = messages_.pop();
         if (message_copy == kTerminate)
            return;
         for (const auto& cb : callbacks_)
#pragma warning(suppress : 26489) // false alarm, checked for existence before adding to callbacks_
            cb(message_copy);
      } while (true);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}