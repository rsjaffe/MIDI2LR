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
#include <array>
#include <atomic>
#include <future>
#include <limits>
#include "MidiUtilities.h"

MidiProcessor::MidiProcessor() noexcept
{}

MidiProcessor::~MidiProcessor()
{}

void MidiProcessor::Init()
{
    InitDevices();
}

void MidiProcessor::handleIncomingMidiMessage(juce::MidiInput * /*device*/,
    const juce::MidiMessage& message)
{
    //this procedure is in near-real-time, so must return quickly. Will spin off
    //callbacks to let that happen. As future is replaced in queue, it is destroyed
    //guaranteeing that async task is run (forced to run before future destroyed if not run earlier)
    constexpr size_t kRingSize{20}; //how many futures to store
    static std::array<std::future<void>, kRingSize> futures;
    static std::atomic<size_t> future_index{0};
    static std::mutex mtx;
    const rsj::MidiMessage mess{message};
    if (future_index.load(std::memory_order_relaxed) > std::numeric_limits<size_t>::max() / 2) {
        std::lock_guard<decltype(mtx)> lock(mtx);//guard access to future_index as this change is non-atomic
        if (auto curr_idx = future_index.load(std::memory_order_acquire); curr_idx > std::numeric_limits<size_t>::max() / 2)
            future_index.store(curr_idx % kRingSize, std::memory_order_release);
    } //note: small risk that another thread will change future_index between curr_idx load and future_index store--not big problem here
    //worst that will happen is that it will attempt to destroy active future, in which case will block until that thread is done
    switch (mess.message_type_byte) {
        case rsj::kCcFlag:
            if (nrpn_filter_.ProcessMidi(mess.channel, mess.number, mess.value)) { //true if nrpn piece
                const auto nrpn = nrpn_filter_.GetNrpnIfReady(mess.channel);
                if (nrpn.is_valid) {//send when finished
                    const auto n_message{rsj::MidiMessage{rsj::kCcFlag, mess.channel, nrpn.control, nrpn.value}};
                    for (const auto& cb : callbacks_)
                        futures[future_index++ % kRingSize] = std::async(std::launch::async, cb, n_message);
                }
            }
            else //regular message
                for (const auto& cb : callbacks_)
                    futures[future_index++ % kRingSize] = std::async(std::launch::async, cb, mess);
            break;
        case rsj::kNoteOnFlag:
        case rsj::kPwFlag:
            for (const auto& cb : callbacks_)
                futures[future_index++ % kRingSize] = std::async(std::launch::async, cb, mess);
            break;
        default:
            ; //no action if other type of MIDI message
    }
}

void MidiProcessor::RescanDevices()
{
    for (const auto& dev : devices_)
        dev->stop();
    devices_.clear();
    InitDevices();
}

void MidiProcessor::InitDevices()
{
    for (auto idx = 0; idx < juce::MidiInput::getDevices().size(); ++idx) {
        const auto dev = juce::MidiInput::openDevice(idx, this);
        if (dev != nullptr) {
            devices_.emplace_back(dev);
            dev->start();
        }
    }
}