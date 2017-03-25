#pragma once
/*
  ==============================================================================

    ProfileManager.h

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
#ifndef PROFILEMANAGER_H_INCLUDED
#define PROFILEMANAGER_H_INCLUDED

#include <functional>
#include <memory>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
class CommandMap;
class ControlsModel;
class LR_IPC_OUT;
class MIDIProcessor;
namespace RSJ {
    struct MidiMessage;
    struct MidiMessageId;
}

class ProfileManager final: private juce::AsyncUpdater {
public:
    ProfileManager(ControlsModel* c_model, CommandMap* const cmap) noexcept;
    virtual ~ProfileManager() = default;
    ProfileManager(ProfileManager const&) = delete;
    void operator=(ProfileManager const&) = delete;
    void Init(std::weak_ptr<LR_IPC_OUT>&& out,
        std::shared_ptr<MIDIProcessor>& midi_processor);

    template<class T> void addCallback(T* object, void(T::*mf)(juce::XmlElement*, const juce::String&))
    {
        using namespace std::placeholders;
        callbacks_.emplace_back(std::bind(mf, object, _1, _2));
    }

    // sets the default profile directory and scans its contents for profiles
    void setProfileDirectory(const juce::File& dir);

    // returns an array of profile names
    const std::vector<juce::String>& getMenuItems() const noexcept;

    // switches to a profile defined by an index
    void switchToProfile(int profileIdx);

    // switches to a profile defined by a name
    void switchToProfile(const juce::String& profile);

    // switches to the next profile
    void switchToNextProfile();

    // switches to the previous profile
    void switchToPreviousProfile();

    void MIDIcmdCallback(RSJ::MidiMessage);

    void ConnectionCallback(bool);

private:
    void mapCommand(const RSJ::MidiMessageId& msg);
    // AsyncUpdate interface
    virtual void handleAsyncUpdate() override;
    enum class SWITCH_STATE {
        NONE,
        PREV,
        NEXT,
    };

    CommandMap* const command_map_;
    ControlsModel* const controls_model_;
    int current_profile_index_{0};
    juce::File profile_location_;
    std::vector<juce::String> profiles_;
    std::vector<std::function<void(juce::XmlElement*, const juce::String&)>> callbacks_;
    std::weak_ptr<LR_IPC_OUT> lr_ipc_out_;
    SWITCH_STATE switch_state_{SWITCH_STATE::NONE};
};

#endif  // PROFILEMANAGER_H_INCLUDED
