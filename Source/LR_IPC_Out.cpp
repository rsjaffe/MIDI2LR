// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
  ==============================================================================

    LR_IPC_OUT.cpp

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
#include <gsl/gsl>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include "LR_IPC_Out.h"
#include "CommandMap.h"
#include "ControlsModel.h"
#include "LRCommands.h"
#include "MIDIProcessor.h"
#include "MidiUtilities.h"
#include "MIDISender.h"
using namespace std::string_literals;

namespace {
    constexpr auto kHost = "127.0.0.1";
    constexpr int kConnectTimer = 1000;
    constexpr int kConnectTryTime = 100;
    constexpr int kDelay{8}; //in between recurrent actions
    constexpr int kLrOutPort = 58763;
    constexpr int kMinRecenterTimer{250}; //give controller enough of a refractory period before resetting it
    constexpr int kRecenterTimer{std::max(kMinRecenterTimer, kDelay + kDelay / 2)};//don't change, change kDelay and kMinRecenterTimer
}

LrIpcOut::LrIpcOut(ControlsModel* c_model, const CommandMap * map_command):
    command_map_{map_command}, controls_model_{c_model} {}

LrIpcOut::~LrIpcOut()
{
    connect_timer_.Stop();
    juce::InterprocessConnection::disconnect();
}

void LrIpcOut::Init(std::shared_ptr<MidiSender> midi_sender, MidiProcessor* midi_processor)
{
    midi_sender_ = std::move(midi_sender);
    if (midi_processor)
        midi_processor->AddCallback(this, &LrIpcOut::MidiCmdCallback);
    connect_timer_.Start();
}

void LrIpcOut::SendCommand(std::string&& command)
{
    if (sending_stopped_) return;
    const thread_local moodycamel::ProducerToken ptok(command_);
    command_.enqueue(ptok, std::move(command));
    juce::AsyncUpdater::triggerAsyncUpdate();
}

void LrIpcOut::MidiCmdCallback(rsj::MidiMessage mm)
{
    const rsj::MidiMessageId message{mm};
#pragma warning(suppress: 26426)
    static const std::unordered_map<std::string, std::pair<std::string, std::string>> kCmdUpDown{
        {"ChangeBrushSize"s, {"BrushSizeLarger 1\n"s, "BrushSizeSmaller 1\n"s}},
        {"ChangeCurrentSlider"s, {"SliderIncrease 1\n"s, "SliderDecrease 1\n"s}},
        {"ChangeFeatherSize"s, {"BrushFeatherLarger 1\n"s, "BrushFeatherSmaller 1\n"s}},
        {"ChangeLastDevelopParameter"s, {"IncrementLastDevelopParameter 1\n"s, "DecrementLastDevelopParameter 1\n"s}},
        {"Key32Key31"s, {"Key32 1\n"s, "Key31 1\n"s}},
        {"Key34Key33"s, {"Key34 1\n"s, "Key33 1\n"s}},
        {"Key36Key35"s, {"Key36 1\n"s, "Key35 1\n"s}},
        {"Key38Key37"s, {"Key38 1\n"s, "Key37 1\n"s}},
        {"Key40Key39"s, {"Key40 1\n"s, "Key39 1\n"s}},
        {"NextPrev"s, {"Next 1\n"s, "Prev 1\n"s}},
        {"RedoUndo"s, {"Redo 1\n"s, "Undo 1\n"s}},
        {"SelectRightLeft"s, {"Select1Right 1\n"s, "Select1Left 1\n"s}},
        {"ZoomInOut"s, {"ZoomInSmallStep 1\n"s, "ZoomOutSmallStep 1\n"s}},
        {"ZoomOutIn"s, {"ZoomOutSmallStep 1\n"s, "ZoomInSmallStep 1\n"s}},
    };
    if (!command_map_->MessageExistsInMap(message) ||
        command_map_->GetCommandforMessage(message) == "Unmapped"s ||
        find(LrCommandList::NextPrevProfile.begin(),
            LrCommandList::NextPrevProfile.end(),
            command_map_->GetCommandforMessage(message)) != LrCommandList::NextPrevProfile.end()) {
        return;
    }
    auto command_to_send = command_map_->GetCommandforMessage(message);
    //if it is a repeated command, change command_to_send appropriately
    if (const auto a = kCmdUpDown.find(command_to_send); a != kCmdUpDown.end()) {
        static rsj::TimeType nextresponse{0};
        if (const auto now = rsj::NowMs(); nextresponse < now) {
            nextresponse = now + kDelay;
            if (mm.message_type_byte == rsj::kPwFlag ||
                (mm.message_type_byte == rsj::kCcFlag &&
                    controls_model_->GetCcMethod(mm.channel, mm.number)
                    == rsj::CCmethod::kAbsolute)) {
                recenter_.SetMidiMessage(mm);
            }
            const auto change = controls_model_->MeasureChange(mm);
            if (change == 0)
                return;//don't send any signal
            if (change > 0) //turned clockwise
                command_to_send = a->second.first;
            else //turned counterclockwise
                command_to_send = a->second.second;
        }
        else
            return; //too soon, don't send anything
    }
    else {
        const auto computed_value = controls_model_->ControllerToPlugin(mm);
        command_to_send += ' ' + std::to_string(computed_value) + '\n';
    }
    SendCommand(std::move(command_to_send));
}

void LrIpcOut::Stop()
{
    sending_stopped_ = true;
    const auto connected = isConnected();
    for (const auto& cb : callbacks_)
        cb(connected, true);
}

void LrIpcOut::Restart()
{
    sending_stopped_ = false;
    const auto connected = isConnected();
    for (const auto& cb : callbacks_)
        cb(connected, false);
    //resync controls
    SendCommand("FullRefresh 1\n"s);
}

void LrIpcOut::connectionMade()
{
    for (const auto& cb : callbacks_)
        cb(true, sending_stopped_);
}

void LrIpcOut::connectionLost()
{
    for (const auto& cb : callbacks_)
        cb(false, sending_stopped_);
}

void LrIpcOut::messageReceived(const juce::MemoryBlock& /*msg*/) noexcept
{}

void LrIpcOut::handleAsyncUpdate()
{
    do {
        std::string command_copy;
        thread_local moodycamel::ConsumerToken ctok(command_);
        if (!command_.try_dequeue(ctok, command_copy))
            return;
        //check if there is a connection
        if (juce::InterprocessConnection::isConnected()) {
            if (command_copy.back() != '\n') //should be terminated with \n
                command_copy += '\n';
            juce::InterprocessConnection::getSocket()->
                write(command_copy.c_str(), gsl::narrow_cast<int>(command_copy.length()));
        }
    } while (true);
}

void LrIpcOut::ConnectTimer::Start()
{
    std::lock_guard<decltype(connect_mutex_)> lock(connect_mutex_);
    juce::Timer::startTimer(kConnectTimer);
    timer_off_ = false;
}

void LrIpcOut::ConnectTimer::Stop()
{
    std::lock_guard<decltype(connect_mutex_)> lock(connect_mutex_);
    juce::Timer::stopTimer();
    timer_off_ = true;
}

void LrIpcOut::ConnectTimer::timerCallback()
{
    std::lock_guard<decltype(connect_mutex_)> lock(connect_mutex_);
    if (!timer_off_ && !owner_->juce::InterprocessConnection::isConnected())
        owner_->juce::InterprocessConnection::connectToSocket(kHost, kLrOutPort, kConnectTryTime);
}

void LrIpcOut::Recenter::SetMidiMessage(rsj::MidiMessage mm)
{
    std::lock_guard<decltype(mtx_)> lock(mtx_);
    mm_ = mm;
    juce::Timer::startTimer(kRecenterTimer);
}

void LrIpcOut::Recenter::timerCallback()
{
    rsj::MidiMessage local_mm{};
    {
        std::lock_guard<decltype(mtx_)> lock(mtx_);
        juce::Timer::stopTimer();
        local_mm = mm_;
    }
    const auto center = owner_->controls_model_->SetToCenter(local_mm);
    //send center to control//
    switch (local_mm.message_type_byte) {
        case rsj::kPwFlag:
        {
            owner_->midi_sender_->SendPitchWheel(local_mm.channel + 1, center);
            break;
        }
        case rsj::kCcFlag:
        {
            owner_->midi_sender_->SendCc(local_mm.channel + 1, local_mm.number, center);
            break;
        }
        default:
            /* no action */;
    }
}