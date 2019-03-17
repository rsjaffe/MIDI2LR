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

#include <exception>
#include <string>
#include <utility>

#include <gsl/gsl>
#include "CommandMap.h"
#include "ControlsModel.h"
#include "LR_IPC_Out.h"
#include "MIDIReceiver.h"
#include "MidiUtilities.h"
#include "Misc.h"
using namespace std::literals::string_literals;

ProfileManager::ProfileManager(ControlsModel& c_model, CommandMap& cmap) noexcept
    : command_map_{cmap}, controls_model_{c_model}
{
}

void ProfileManager::Init(std::weak_ptr<LrIpcOut>&& out, MidiReceiver* midi_receiver)
{
   // copy the pointers
   lr_ipc_out_ = std::move(out);

   if (const auto ptr = lr_ipc_out_.lock())
      // add ourselves as a listener to LR_IPC_OUT so that we can send plugin
      // settings on connection
      ptr->AddCallback(this, &ProfileManager::ConnectionCallback);
   if (midi_receiver)
      midi_receiver->AddCallback(this, &ProfileManager::MidiCmdCallback);
}

void ProfileManager::SetProfileDirectory(const juce::File& directory)
{
   try {
      profile_location_ = directory;
      juce::Array<juce::File> file_array;
      directory.findChildFiles(file_array, juce::File::findFiles, false, "*.xml");
      current_profile_index_ = 0;
      profiles_.clear();
      for (const auto& file : file_array)
         profiles_.emplace_back(file.getFileName());
      if (!profiles_.empty())
         SwitchToProfile(profiles_.at(0));
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

const std::vector<juce::String>& ProfileManager::GetMenuItems() const noexcept
{
   return profiles_;
}

void ProfileManager::SwitchToProfile(int profile_index)
{
   if (profile_index >= 0 && profile_index < gsl::narrow_cast<int>(profiles_.size())) {
      SwitchToProfile(profiles_.at(gsl::narrow_cast<size_t>(profile_index)));
      current_profile_index_ = profile_index;
   }
}

void ProfileManager::SwitchToProfile(const juce::String& profile)
{
   try {
      const auto profile_file = profile_location_.getChildFile(profile);
      if (profile_file.exists()) {
         if (const auto parsed{juce::XmlDocument::parse(profile_file)}) {
            std::unique_ptr<juce::XmlElement> xml_element{parsed};
            for (const auto& cb : callbacks_)
               cb(xml_element.get(), profile);
            if (const auto ptr = lr_ipc_out_.lock()) {
               auto command =
                   "ChangedToDirectory "s
                   + juce::File::addTrailingSeparator(profile_location_.getFullPathName())
                         .toStdString()
                   + '\n';
               ptr->SendCommand(std::move(command));
               command = "ChangedToFile "s + profile.toStdString() + '\n';
               ptr->SendCommand(std::move(command));
            }
         }
      }
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void ProfileManager::SwitchToNextProfile()
{
   if (++current_profile_index_ == gsl::narrow_cast<int>(profiles_.size()))
      current_profile_index_ = 0;
   SwitchToProfile(current_profile_index_);
}

void ProfileManager::SwitchToPreviousProfile()
{
   if (--current_profile_index_ < 0)
      current_profile_index_ = gsl::narrow_cast<int>(profiles_.size()) - 1;
   SwitchToProfile(current_profile_index_);
}

void ProfileManager::MapCommand(const rsj::MidiMessageId& msg)
{
   const auto cmd = command_map_.GetCommandForMessage(msg);
   if (cmd == "PrevPro"s) {
      switch_state_ = SwitchState::kPrev;
      triggerAsyncUpdate();
   }
   else if (cmd == "NextPro"s) {
      switch_state_ = SwitchState::kNext;
      triggerAsyncUpdate();
   }
}

void ProfileManager::MidiCmdCallback(rsj::MidiMessage mm)
{
   try {
      const rsj::MidiMessageId cc = mm;
      // return if the value isn't high enough (notes may be < 1), or the command isn't a valid
      // profile-related command
      if (controls_model_.ControllerToPlugin(mm) < 0.4 || !command_map_.MessageExistsInMap(cc))
         return;
      MapCommand(cc);
   }
   catch (const std::exception& e) {
      rsj::ExceptionResponse(typeid(this).name(), __func__, e);
      throw;
   }
}

void ProfileManager::ConnectionCallback(bool connected, bool blocked)
{
   if (connected && !blocked) {
      if (const auto ptr = lr_ipc_out_.lock()) {
         ptr->SendCommand(
             "ChangedToDirectory "s
             + juce::File::addTrailingSeparator(profile_location_.getFullPathName()).toStdString()
             + '\n');
      }
   }
}

void ProfileManager::handleAsyncUpdate()
{
   switch (switch_state_) {
   case SwitchState::kPrev:
      SwitchToPreviousProfile();
      switch_state_ = SwitchState::kNone;
      break;
   case SwitchState::kNext:
      SwitchToNextProfile();
      switch_state_ = SwitchState::kNone;
      break;
   default:
      break;
   }
}