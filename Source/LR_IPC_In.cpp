// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
  ==============================================================================

    LR_IPC_In.cpp

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
#include "LR_IPC_In.h"
#include <bitset>
#include <gsl/gsl>
#include "CommandMap.h"
#include "ControlsModel.h"
#include "MIDISender.h"
#include "MidiUtilities.h"
#include "Misc.h"
#include "ProfileManager.h"
#include "SendKeys.h"
#include "Utilities/Utilities.h"
using namespace std::literals::string_literals;

namespace {
    constexpr auto kHost = "127.0.0.1";
    constexpr int kBufferSize = 256;
    constexpr int kConnectTryTime = 100;
    constexpr int kEmptyWait = 100;
    constexpr int kLrInPort = 58764;
    constexpr int kNotConnectedWait = 333;
    constexpr int kReadyWait = 1000;
    constexpr int kStopWait = 1000;
    constexpr int kTimerInterval = 1000;
}

LR_IPC_IN::LR_IPC_IN(ControlsModel* const c_model, ProfileManager* const pmanager, CommandMap* const cmap):
    juce::Thread{"LR_IPC_IN"}, command_map_{cmap},
    controls_model_{c_model}, profile_manager_{pmanager}
{}

LR_IPC_IN::~LR_IPC_IN()
{
    {
        std::lock_guard<decltype(timer_mutex_)> lock(timer_mutex_);
        timer_off_ = true;
        juce::Timer::stopTimer();
    }
    juce::Thread::stopThread(kStopWait);
    socket_.close();
}

void LR_IPC_IN::Init(std::shared_ptr<MIDISender>& midi_sender) noexcept
{
    midi_sender_ = midi_sender;
    //start the timer
    juce::Timer::startTimer(kTimerInterval);
}

void LR_IPC_IN::PleaseStopThread()
{
    juce::Thread::signalThreadShouldExit();
    juce::Thread::notify();
}

void LR_IPC_IN::run()
{
    while (!juce::Thread::threadShouldExit()) {
        //if not connected, executes a wait 333 then goes back to while
        //if connected, tries to read a line, checks thread status and connection
        //status before each read attempt
        //doesn't terminate thread if disconnected, as currently don't have graceful
        //way to restart thread
        if (!socket_.isConnected()) {
            juce::Thread::wait(kNotConnectedWait);
        } //end if (is not connected)
        else {
            char line[kBufferSize + 1] = {0};//plus one for \0 at end
            auto size_read = 0;
            // parse input until we have a line, then process that line, quit if
            // connection lost
            while (std::string(line).back() != '\n' && socket_.isConnected()) {
                if (juce::Thread::threadShouldExit())
                    goto threadExit;//break out of nested whiles
                const auto wait_status = socket_.waitUntilReady(true, kReadyWait);
                switch (wait_status) {
                case -1:
                    goto dumpLine; //read line failed, break out of switch and while
                case 0:
                    juce::Thread::wait(kEmptyWait);
                    break; //try again to read until char shows up
                case 1:
                    if (size_read == kBufferSize)
                        throw std::out_of_range("Buffer overflow in LR_IPC_IN");
                    if (const auto read = socket_.read(line + size_read, 1, false))
                        size_read += read;
                    else
                        // waitUntilReady returns 1 but read will is 0: it's an indication of a broken socket.
                        juce::JUCEApplication::getInstance()->systemRequestedQuit();
                    break;
                default:
                    Expects(!"Unexpected value for wait_status");
                }
            } // end while !\n and is connected

            // if lose connection, line may not be terminated
            {
                std::string param{line};
                if (param.back() == '\n')
                    processLine(param);
            } //scope param
        dumpLine: /* empty statement */;
        } //end else (is connected)
    } //while not threadshouldexit
threadExit: /* empty statement */;
    std::lock_guard< decltype(timer_mutex_) > lock(timer_mutex_);
    timer_off_ = true;
    juce::Timer::stopTimer();
    //thread_started_ = false; //don't change flag while depending upon it
}

void LR_IPC_IN::timerCallback()
{
    std::lock_guard< decltype(timer_mutex_) > lock(timer_mutex_);
    if (!timer_off_ && !socket_.isConnected() && !juce::Thread::threadShouldExit()) {
        if (socket_.connect(kHost, kLrInPort, kConnectTryTime))
            if (!thread_started_) {
                juce::Thread::startThread(); //avoid starting thread during shutdown
                thread_started_ = true;
            }
    }
}

void LR_IPC_IN::processLine(const std::string& line) const
{
    const static std::unordered_map<std::string, int> cmds = {
        {"SwitchProfile"s, 1},
        {"SendKey"s, 2},
        {"TerminateApplication"s, 3},
    };
    // process input into [parameter] [Value]
    const auto trimmed_line = RSJ::trim(line);
    const auto command = trimmed_line.substr(0, trimmed_line.find(' '));
    const auto value_string = trimmed_line.substr(trimmed_line.find(' ') + 1);

    switch (cmds.count(command) ? cmds.at(command) : 0) {
    case 1: //SwitchProfile
        if (profile_manager_)
            profile_manager_->switchToProfile(value_string);
        break;
    case 2: //SendKey
    {
        // ReSharper disable once CppUseAuto
        std::bitset<3> modifiers{static_cast<decltype(modifiers)>
            (std::stoi(value_string))};
        RSJ::SendKeyDownUp(RSJ::ltrim(RSJ::ltrim(value_string, RSJ::digit)),
            modifiers[0], modifiers[1], modifiers[2]);//ltrim twice on purpose: first digits, then spaces
        break;
    }
    case 3: //TerminateApplication
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
        break;
    case 0:
        // send associated messages to MIDI OUT devices
        if (command_map_ && midi_sender_) {
            const auto original_value = std::stod(value_string);
            for (const auto msg : command_map_->getMessagesForCommand(command)) {
                short msgtype{0};
                switch (msg->msg_id_type) {
                case RSJ::MsgIdEnum::NOTE:
                    msgtype = RSJ::kNoteOnFlag;
                    break;
                case RSJ::MsgIdEnum::CC:
                    msgtype = RSJ::kCCFlag;
                    break;
                case RSJ::MsgIdEnum::PITCHBEND:
                    msgtype = RSJ::kPWFlag;
                }
                const auto value = controls_model_->PluginToController(msgtype,
                    static_cast<size_t>(msg->channel - 1),
                    gsl::narrow_cast<short>(msg->controller), original_value);

                if (midi_sender_) {
                    switch (msgtype) {
                    case RSJ::kNoteOnFlag:
                        midi_sender_->sendNoteOn(msg->channel, msg->controller, value);
                        break;
                    case RSJ::kCCFlag:
                        if (controls_model_->getCCmethod(static_cast<size_t>(msg->channel - 1),
                            gsl::narrow_cast<short>(msg->controller)) == RSJ::CCmethod::absolute)
                            midi_sender_->sendCC(msg->channel, msg->controller, value);
                        break;
                    case RSJ::kPWFlag:
                        midi_sender_->sendPitchWheel(msg->channel, value);
                        break;
                    default:
                        Expects(!"Unexpected result for msgtype");
                    }
                }
            }
        }
        break;
    default:
        Expects(!"Unexpected result for cmds");
    }
}