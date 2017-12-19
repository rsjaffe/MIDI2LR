// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
  ==============================================================================

    ProfileManager.cpp

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
#include "ProfileManager.h"
#include <string>
#include <utility>
#include <gsl/gsl>
#include "CommandMap.h"
#include "ControlsModel.h"
#include "LR_IPC_Out.h"
#include "LRCommands.h"
#include "MIDIProcessor.h"
#include "MidiUtilities.h"
using namespace std::literals::string_literals;

ProfileManager::ProfileManager(ControlsModel* const c_model, CommandMap* const cmap) noexcept:
command_map_{cmap}, controls_model_{c_model}
{}

void ProfileManager::Init(std::weak_ptr<LR_IPC_OUT>&& out,
    MIDIProcessor* const midiProcessor)
{
    //copy the pointers
    lr_ipc_out_ = std::move(out);

    if (const auto ptr = lr_ipc_out_.lock())
        // add ourselves as a listener to LR_IPC_OUT so that we can send plugin
        // settings on connection
        ptr->addCallback(this, &ProfileManager::ConnectionCallback);

    if (midiProcessor)
        midiProcessor->addCallback(this, &ProfileManager::MIDIcmdCallback);
}

void ProfileManager::setProfileDirectory(const juce::File& directory)
{
    profile_location_ = directory;

    juce::Array<juce::File> file_array;
    directory.findChildFiles(file_array, juce::File::findFiles, false, "*.xml");

    current_profile_index_ = 0;
    profiles_.clear();
    for (const auto& file : file_array)
        profiles_.emplace_back(file.getFileName());

    if (profiles_.size() > 0)
        switchToProfile(profiles_[0]);
}

const std::vector<juce::String>& ProfileManager::getMenuItems() const noexcept
{
    return profiles_;
}

void ProfileManager::switchToProfile(int profile_index)
{
    if (profile_index >= 0 && profile_index < gsl::narrow_cast<int>(profiles_.size())) {
        switchToProfile(profiles_[static_cast<size_t>(profile_index)]);
        current_profile_index_ = profile_index;
    }
}

void ProfileManager::switchToProfile(const juce::String& profile)
{
    const auto profile_file = profile_location_.getChildFile(profile);

    if (profile_file.exists()) {
        std::unique_ptr<juce::XmlElement> xml_element{juce::XmlDocument::parse(profile_file)};
        if (xml_element) {
            for (const auto& cb : callbacks_)
                cb(xml_element.get(), profile);

            if (const auto ptr = lr_ipc_out_.lock()) {
                auto command = "ChangedToDirectory "s +
                    juce::File::addTrailingSeparator(profile_location_.getFullPathName()).toStdString() +
                    '\n';
                ptr->sendCommand(command);
                command = "ChangedToFile "s + profile.toStdString() + '\n';
                ptr->sendCommand(command);
            }
        }
    }
}

void ProfileManager::switchToNextProfile()
{
    current_profile_index_++;
    if (current_profile_index_ == gsl::narrow_cast<int>(profiles_.size()))
        current_profile_index_ = 0;
    switchToProfile(current_profile_index_);
}

void ProfileManager::switchToPreviousProfile()
{
    current_profile_index_--;
    if (current_profile_index_ < 0)
        current_profile_index_ = gsl::narrow_cast<int>(profiles_.size()) - 1;
    switchToProfile(current_profile_index_);
}

void ProfileManager::mapCommand(const RSJ::MidiMessageId& msg)
{
    const auto cmd = command_map_->getCommandforMessage(msg);
    if (cmd == "Previous Profile"s) {
        switch_state_ = SWITCH_STATE::PREV;
        triggerAsyncUpdate();
    }
    else if (cmd == "Next Profile"s) {
        switch_state_ = SWITCH_STATE::NEXT;
        triggerAsyncUpdate();
    }
}

void ProfileManager::MIDIcmdCallback(RSJ::MidiMessage mm)
{
    if (command_map_) {
        const RSJ::MidiMessageId cc = mm;
        // return if the value isn't high enough (notes may be < 1), or the command isn't a valid
        // profile-related command
        if ((controls_model_->ControllerToPlugin(mm) < 0.4)
            || !command_map_->messageExistsInMap(cc))
            return;
        mapCommand(cc);
    }
}

void ProfileManager::ConnectionCallback(bool connected)
{
    if (connected) {
        const auto command = "ChangedToDirectory "s +
            juce::File::addTrailingSeparator(profile_location_.getFullPathName()).toStdString() +
            '\n';
        if (const auto ptr = lr_ipc_out_.lock())
            ptr->sendCommand(command);
    }
}

void ProfileManager::handleAsyncUpdate()
{
    switch (switch_state_) {
        case SWITCH_STATE::PREV:
            switchToPreviousProfile();
            switch_state_ = SWITCH_STATE::NONE;
            break;
        case SWITCH_STATE::NEXT:
            switchToNextProfile();
            switch_state_ = SWITCH_STATE::NONE;
            break;
        default:
            break;
    }
}