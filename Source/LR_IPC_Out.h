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
#include "Misc.h"
class CommandMap;
class ControlsModel;
class MIDIProcessor;
namespace RSJ {
    struct MidiMessage;
}

class LR_IPC_OUT final:
    private juce::InterprocessConnection,
    private juce::AsyncUpdater,
    private juce::Timer {
public:
    LR_IPC_OUT(ControlsModel* const c_model, const CommandMap * const mapCommand);
    virtual ~LR_IPC_OUT();
    void Init(MIDIProcessor* const midi_processor);

    template<class T> void addCallback(T* const  object, void(T::* const mf)(bool))
    {
        callbacks_.emplace_back(std::bind(mf, object, std::placeholders::_1));
    }

    // sends a command to the plugin
    void sendCommand(const std::string& command);

    void MIDIcmdCallback(RSJ::MidiMessage);

private:
    // IPC interface
    void connectionMade() override;
    void connectionLost() override;
    void messageReceived(const juce::MemoryBlock& msg) override;
    // AsyncUpdater interface
    void handleAsyncUpdate() override;
    // Timer callback
    void timerCallback() override;

    bool timer_off_{false};
    const CommandMap * const command_map_;
    ControlsModel* const controls_model_;
    mutable RSJ::RelaxTTasSpinLock command_mutex_; //fast spinlock for brief use
    mutable std::mutex timer_mutex_; //fix race during shutdown
    std::string command_;
    std::vector<std::function<void(bool)>> callbacks_;
};

#endif  // LR_IPC_OUT_H_INCLUDED
