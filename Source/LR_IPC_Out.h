#pragma once
/*
  ==============================================================================

    LR_IPC_Out.h

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
#ifndef MIDI2LR_LR_IPC_OUT_H_INCLUDED
#define MIDI2LR_LR_IPC_OUT_H_INCLUDED

#include <functional>
#include <mutex>
#include <string>
#include <vector>
#include "MoodyCamel/concurrentqueue.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "MidiUtilities.h"
#include "Misc.h"
class CommandMap;
class ControlsModel;
class MidiProcessor;
class MidiSender;


class LrIpcOut final:
    private juce::InterprocessConnection,
    private juce::AsyncUpdater {
public:
    LrIpcOut(ControlsModel* const c_model, const CommandMap * const map_command);
    virtual ~LrIpcOut();
    void Init(std::shared_ptr<MidiSender>& midi_sender, MidiProcessor* const midi_processor);

    template<class T> void AddCallback(T* const  object, void(T::* const mf)(bool,bool))
    {
        callbacks_.emplace_back(std::bind(mf, object, std::placeholders::_1, std::placeholders::_2));
    }

    // sends a command to the plugin
    void SendCommand(std::string&& command);

    void MidiCmdCallback(rsj::MidiMessage);
    void Stop();
    void Restart();

private:
    // IPC interface
    void connectionLost() override;
    void connectionMade() override;
    void messageReceived(const juce::MemoryBlock& msg) override;
    // AsyncUpdater interface
    void handleAsyncUpdate() override;
    //private members
    const CommandMap* const command_map_{};
    ControlsModel* const controls_model_{};
    moodycamel::ConcurrentQueue<std::string> command_;
    bool sending_stopped_{false};
    std::shared_ptr<MidiSender> midi_sender_{nullptr};
    std::vector<std::function<void(bool,bool)>> callbacks_{};
    // helper classes
    class ConnectTimer:public juce::Timer {
    public:
        explicit ConnectTimer(LrIpcOut* owner):owner_(owner)
        {}
        void Start();
        void Stop();
    private:
        void timerCallback() override;
        LrIpcOut* owner_;
        bool timer_off_{false};
        mutable std::mutex connect_mutex_; //fix race during shutdown
    };
    class Recenter:public juce::Timer {
    public:
        explicit Recenter(LrIpcOut* owner):owner_{owner}
        {}
        void SetMidiMessage(rsj::MidiMessage mm);
    private:
        void timerCallback() override;
        LrIpcOut* owner_;
        rsj::RelaxTTasSpinLock mtx_;
        rsj::MidiMessage mm_{};
    };
    ConnectTimer connect_timer_{this};
    Recenter recenter_{this};
};

#endif  // LR_IPC_OUT_H_INCLUDED
