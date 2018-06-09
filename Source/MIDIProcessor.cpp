// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
#include "MIDIProcessor.h"
#include <exception>
#include <future>
#include "Misc.h"

namespace {
   constexpr rsj::MidiMessage kTerminate{0, 129, 0, 0}; // impossible channel
}

MidiProcessor::~MidiProcessor()
{
   for (const auto& dev : devices_)
      dev->stop();
   moodycamel::ConsumerToken ctok(messages_);
   rsj::MidiMessage message_copy{};
   while (messages_.try_dequeue(ctok, message_copy)) {
      /* pump the queue empty */
   }
   messages_.enqueue(kTerminate);
}

void MidiProcessor::Init()
{
   try {
      InitDevices();
      dispatch_messages_future_ =
          std::async(std::launch::async, &MidiProcessor::DispatchMessages, this);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void MidiProcessor::handleIncomingMidiMessage(
    juce::MidiInput* /*device*/, const juce::MidiMessage& message)
{
   try {
      // this procedure is in near-real-time, so must return quickly.
      // will place message in multithreaded queue and let separate process handle the messages
      static const thread_local moodycamel::ProducerToken ptok(messages_);
      const rsj::MidiMessage mess{message};
      switch (mess.message_type_byte) {
      case rsj::kCcFlag:
         if (nrpn_filter_.ProcessMidi(mess.channel, mess.number, mess.value)) { // true if nrpn
                                                                                // piece
            const auto nrpn = nrpn_filter_.GetNrpnIfReady(mess.channel);
            if (nrpn.is_valid) { // send when finished
               const auto n_message{
                   rsj::MidiMessage{rsj::kCcFlag, mess.channel, nrpn.control, nrpn.value}};
               messages_.enqueue(ptok, n_message);
            }
            break; // finished with nrpn piece
         }
         [[fallthrough]]; // if not nrpn, handle like other messages
      case rsj::kNoteOnFlag:
      case rsj::kPwFlag:
         messages_.enqueue(ptok, mess);
         break;
      default:; // no action if other type of MIDI message
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void MidiProcessor::RescanDevices()
{
   try {
      for (const auto& dev : devices_)
         dev->stop();
      devices_.clear();
   }
   catch (const std::exception& e) {
      juce::NativeMessageBox::showMessageBox(juce::AlertWindow::WarningIcon, "Error",
          juce::String("Exception ") + e.what() + ' ' + __func__ + ' ' + __FILE__ + ". Version "
              + ProjectInfo::versionString);
      throw;
   }
   InitDevices(); // initdevices has own try catch block
}

void MidiProcessor::InitDevices()
{
   try {
      for (auto idx = 0; idx < juce::MidiInput::getDevices().size(); ++idx) {
         const auto dev = juce::MidiInput::openDevice(idx, this);
         if (dev) {
            devices_.emplace_back(dev);
            dev->start();
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void MidiProcessor::DispatchMessages()
{
   try {
      static thread_local moodycamel::ConsumerToken ctok(messages_);
      do {
         rsj::MidiMessage message_copy;
         if (!messages_.try_dequeue(ctok, message_copy))
            messages_.wait_dequeue(message_copy);
         if (message_copy == kTerminate)
            return;
         for (const auto& cb : callbacks_)
            cb(message_copy);
      } while (true);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}