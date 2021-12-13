/*
 * This file is part of MIDI2LR. Copyright (C) 2015 by Rory Jaffe.
 *
 * MIDI2LR is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with MIDI2LR.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include "SettingsManager.h"

#include <exception>
#include <mutex>
#include <string>

#include "DebugInfo.h"

SettingsManager::SettingsManager(ProfileManager& profile_manager, LrIpcOut& lr_ipc_out)
    : lr_ipc_out_ {lr_ipc_out}, profile_manager_ {profile_manager}
{
   try {
      juce::PropertiesFile::Options file_options;
      file_options.applicationName = "MIDI2LR";
      file_options.commonToAllUsers = false;
      file_options.filenameSuffix = "xml";
      file_options.osxLibrarySubFolder = "Application Support/MIDI2LR";
      file_options.storageFormat = juce::PropertiesFile::storeAsXML;
      properties_file_ = std::make_unique<juce::PropertiesFile>(file_options);
      /* add a listener to LR_IPC_OUT so that we can send plugin settings on connection */
      lr_ipc_out_.AddCallback(this, &SettingsManager::ConnectionCallback);
      profile_manager_.SetProfileDirectory(GetProfileDirectory());
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}

// ReSharper disable CppMemberFunctionMayBeConst

/*Const means method won't change object's client-visible state. These methods do, by changing the
 * underlying file. ConnectionCallback is not const as it needs to be compatible with the callback
 * type. */
void SettingsManager::ConnectionCallback(const bool connected, const bool blocked)
{
   try {
      if (connected && !blocked) {
         if (GetPickupEnabled()) {
            lr_ipc_out_.SendCommand("Pickup 1\n");
            rsj::Log("Pickup is enabled.");
         }
         else {
            lr_ipc_out_.SendCommand("Pickup 0\n");
            rsj::Log("Pickup is disabled.");
         }
         static std::once_flag of; /* add debug info once to logs */
         std::call_once(of, [this] {
            const DebugInfo db {GetProfileDirectory().toStdString()};
            lr_ipc_out_.SendCommand("AppInfoClear 1\n");
            for (const auto& info : db.GetInfo()) {
               lr_ipc_out_.SendCommand(fmt::format(FMT_STRING("AppInfo {}\n"), info));
            }
            lr_ipc_out_.SendCommand("AppInfoDone 1\n");
            lr_ipc_out_.SendCommand("GetPluginInfo 1\n");
         });
      }
   }
   catch (const std::exception& e) {
      MIDI2LR_E_RESPONSE;
      throw;
   }
}
// ReSharper restore CppMemberFunctionMayBeConst