#pragma once
/*
  ==============================================================================

    ProfileManager.h

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
#ifndef MIDI2LR_PROFILEMANAGER_H_INCLUDED
#define MIDI2LR_PROFILEMANAGER_H_INCLUDED

#include <functional>
#include <memory>
#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"
class CommandMap;
class ControlsModel;
class LrIpcOut;
class MidiProcessor;
namespace rsj {
    struct MidiMessage;
    struct MidiMessageId;
}

class ProfileManager final: private juce::AsyncUpdater {
public:
    ProfileManager(ControlsModel* const c_model, CommandMap* const cmap) noexcept;
    virtual ~ProfileManager() = default;
    ProfileManager(ProfileManager const&) = delete;
    void operator=(ProfileManager const&) = delete;
    void Init(std::weak_ptr<LrIpcOut>&& out, MidiProcessor* const midi_processor);

    template<class T>
    void AddCallback(T* const object,
        void(T::* const mf)(juce::XmlElement*, const juce::String&))
    {
        using namespace std::placeholders;
        callbacks_.emplace_back(std::bind(mf, object, _1, _2));
    }

    // sets the default profile directory and scans its contents for profiles
    void SetProfileDirectory(const juce::File& dir);

    // returns an array of profile names
    const std::vector<juce::String>& GetMenuItems() const noexcept;

    // switches to a profile defined by an index
    void SwitchToProfile(int profile_idx);

    // switches to a profile defined by a name
    void SwitchToProfile(const juce::String& profile);

    // switches to the next profile
    void SwitchToNextProfile();

    // switches to the previous profile
    void SwitchToPreviousProfile();

    void MidiCmdCallback(rsj::MidiMessage);

    void ConnectionCallback(bool);

private:
    void MapCommand(const rsj::MidiMessageId& msg);
    // AsyncUpdate interface
    void handleAsyncUpdate() override;
    enum class SwitchState {
        kNone,
        kPrev,
        kNext,
    };

    CommandMap* const command_map_;
    ControlsModel* const controls_model_;
    int current_profile_index_{0};
    juce::File profile_location_;
    std::vector<juce::String> profiles_;
    std::vector<std::function<void(juce::XmlElement*, const juce::String&)>> callbacks_;
    std::weak_ptr<LrIpcOut> lr_ipc_out_;
    SwitchState switch_state_{SwitchState::kNone};
};

#endif  // PROFILEMANAGER_H_INCLUDED
