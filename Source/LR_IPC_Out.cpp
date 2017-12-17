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
    constexpr int kConnectTryTime = 100;
    constexpr int kLrOutPort = 58763;
    constexpr int kTimerInterval = 1000;
}

LR_IPC_OUT::LR_IPC_OUT(ControlsModel* const c_model, const CommandMap * const mapCommand):
    juce::InterprocessConnection(), command_map_{mapCommand}, controls_model_{c_model} {}

LR_IPC_OUT::~LR_IPC_OUT()
{
    {
        std::lock_guard<decltype(timer_mutex_)> lock(timer_mutex_);
        timer_off_ = true;
        juce::Timer::stopTimer();
    }
    juce::InterprocessConnection::disconnect();
}

void LR_IPC_OUT::Init(std::shared_ptr<MIDISender>& midiSender, MIDIProcessor* const midi_processor)
{
    midi_sender_ = midiSender;

    if (midi_processor)
        midi_processor->addCallback(this, &LR_IPC_OUT::MIDIcmdCallback);

    //start the timer
    juce::Timer::startTimer(kTimerInterval);
}

void LR_IPC_OUT::sendCommand(const std::string& command)
{
    {
        std::lock_guard<decltype(command_mutex_)> lock(command_mutex_);
        command_ += command;
    }
    juce::AsyncUpdater::triggerAsyncUpdate();
}

void LR_IPC_OUT::MIDIcmdCallback(RSJ::MidiMessage mm)
{
    const RSJ::MidiMessageId message{mm};
    static const std::unordered_map<std::string, std::pair<std::string, std::string>> cmdupdown{
        {"ChangeBrushSize"s, {"BrushSizeLarger 1\n"s, "BrushSizeSmaller 1\n"s}},
    {"ChangeCurrentSlider"s, {"SliderIncrease 1\n"s, "SliderDecrease 1\n"s}},
    {"ChangeFeatherSize"s, {"BrushFeatherLarger 1\n"s, "BrushFeatherSmaller 1\n"s}},
    {"ChangeLastDevelopParameter"s, {"IncrementLastDevelopParameter 1\n"s, "DecrementLastDevelopParameter 1\n"s}},
    {"Key38Key37"s, {"Key38 1\n"s, "Key37 1\n"s}},
    {"Key40Key39"s, {"Key40 1\n"s, "Key39 1\n"s}},
    {"NextPrev"s, {"Next 1\n"s, "Prev 1\n"s}},
    {"RedoUndo"s, {"Redo 1\n"s, "Undo 1\n"s}},
    };
    if (!command_map_->messageExistsInMap(message) ||
        command_map_->getCommandforMessage(message) == "Unmapped"s ||
        find(LRCommandList::NextPrevProfile.begin(),
            LRCommandList::NextPrevProfile.end(),
            command_map_->getCommandforMessage(message)) != LRCommandList::NextPrevProfile.end()) {
        return;
    }
    auto command_to_send = command_map_->getCommandforMessage(message);
    //if it is a repeated command, change command_to_send appropriately
    if (const auto a = cmdupdown.find(command_to_send); a != cmdupdown.end()) {
        static RSJ::timetype nextresponse{0};
        constexpr RSJ::timetype delay{10};
        if (const auto now = RSJ::now_ms(); nextresponse < now) {
            nextresponse = now + delay;
            const auto[change, newvalue] = controls_model_->MeasureChange(mm);
            switch (mm.message_type_byte) {
                case RSJ::kPWFlag:
                {
                    midi_sender_->sendPitchWheel(mm.channel, newvalue);
                    break;
                }
                case RSJ::kCCFlag:
                {
                    if (controls_model_->getCCmethod(mm.channel, mm.number) == RSJ::CCmethod::absolute)
                        midi_sender_->sendCC(mm.channel, mm.number, newvalue);
                    [[fallthrough]];
                }
                default:
                    /* ignore everything else */;
            }
            switch (change) {//knob may be far off center first time used
                case -5:
                case -4:
                case -3:
                case -2:
                case -1:
                    command_to_send = a->second.second;
                    break;//a->second is std::pair of selected unordered_map element
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    command_to_send = a->second.first;
                    [[fallthrough]];
                default:
                    /*do nothing*/;
            }
        }
    }
    else {
        const auto computed_value = controls_model_->ControllerToPlugin(mm);
        command_to_send += ' ' + std::to_string(computed_value) + '\n';
    }
    { //lock scope
        std::lock_guard<decltype(command_mutex_)> lock(command_mutex_);
        command_ += command_to_send;
    }
    juce::AsyncUpdater::triggerAsyncUpdate();
}

void LR_IPC_OUT::connectionMade()
{
    for (const auto& cb : callbacks_)
        cb(true);
}

void LR_IPC_OUT::connectionLost()
{
    for (const auto& cb : callbacks_)
        cb(false);
}

void LR_IPC_OUT::messageReceived(const juce::MemoryBlock& /*msg*/)
{}

void LR_IPC_OUT::handleAsyncUpdate()
{
    std::string command_copy;
    { //lock scope
        std::lock_guard<decltype(command_mutex_)> lock(command_mutex_);
        command_copy.swap(command_);
    }
    //check if there is a connection
    if (juce::InterprocessConnection::isConnected()) {
        juce::InterprocessConnection::getSocket()->
            write(command_copy.c_str(), gsl::narrow_cast<int>(command_copy.length()));
    }
}

void LR_IPC_OUT::timerCallback()
{
    std::lock_guard<decltype(timer_mutex_)> lock(timer_mutex_);
    if (!timer_off_ && !juce::InterprocessConnection::isConnected())
        juce::InterprocessConnection::connectToSocket(kHost, kLrOutPort, kConnectTryTime);
}