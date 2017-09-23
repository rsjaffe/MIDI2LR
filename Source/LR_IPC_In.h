#pragma once
/*
  ==============================================================================

    LR_IPC_In.h

This file is part of MIDI2LR. Copyright 2015-2017 by Rory Jaffe.

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
#ifndef LR_IPC_IN_H_INCLUDED
#define LR_IPC_IN_H_INCLUDED

#include <memory>
#include <mutex>
#include <string>
#include "../JuceLibraryCode/JuceHeader.h"
class CommandMap;
class ControlsModel;
class MIDISender;
class ProfileManager;

class LR_IPC_IN final:
    private juce::StreamingSocket,
    private juce::Timer,
    private juce::Thread {
public:
    LR_IPC_IN(ControlsModel* const c_model, ProfileManager* const profileManager,
        CommandMap* const commandMap);
    virtual ~LR_IPC_IN();
    void Init(std::shared_ptr<MIDISender>& midiSender) noexcept;
    //signal exit to thread
    void PleaseStopThread();
private:
    // Thread interface
    void run() override;
    // Timer callback
    void timerCallback() override;
    // process a line received from the socket
    void processLine(const std::string& line) const;

    bool thread_started_{false};
    bool timer_off_{false};
    CommandMap* const command_map_;
    ControlsModel* const controls_model_; //
    mutable std::mutex timer_mutex_;
    ProfileManager* const profile_manager_;
    std::shared_ptr<MIDISender> midi_sender_{nullptr};
};

#endif  // LR_IPC_IN_H_INCLUDED
