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

    template<class T> void AddCallback(T* const  object, void(T::* const mf)(bool))
    {
        callbacks_.emplace_back(std::bind(mf, object, std::placeholders::_1));
    }

    // sends a command to the plugin
    void SendCommand(const std::string& command);

    void MidiCmdCallback(rsj::MidiMessage);

private:
    // IPC interface
    void connectionMade() override;
    void connectionLost() override;
    void messageReceived(const juce::MemoryBlock& msg) override;
    // AsyncUpdater interface
    void handleAsyncUpdate() override;
    // Timer callback
    class connect_timer:public juce::Timer {
    public:
        connect_timer(LrIpcOut* owner):owner_(owner)
        {}
        void Start();
        void Stop();
    private:
        void timerCallback() override;
        LrIpcOut* owner_;
        bool timer_off_{false};
        mutable std::mutex connect_mutex_; //fix race during shutdown
    };
    class recenter:public juce::Timer {
    public:
        recenter(LrIpcOut* owner):owner_{owner}
        {}
        void SetMidiMessage(rsj::MidiMessage mm);
    private:
        void timerCallback() override;
        LrIpcOut* owner_;
        rsj::RelaxTTasSpinLock mtx_;
        rsj::MidiMessage mm_{};
    };
    connect_timer connect_timer_{this};
    recenter recenter_{this};
    const CommandMap * const command_map_;
    ControlsModel* const controls_model_;
    mutable rsj::RelaxTTasSpinLock command_mutex_; //fast spinlock for brief use
    std::string command_;
    std::vector<std::function<void(bool)>> callbacks_;
    std::shared_ptr<MidiSender> midi_sender_{nullptr};
};

#endif  // LR_IPC_OUT_H_INCLUDED
