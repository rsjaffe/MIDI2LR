--[[----------------------------------------------------------------------------

Info.lua
MIDI2LR Plugin properties
 
This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>. 
------------------------------------------------------------------------------]]

return {
  LrForceInitPlugin = true,  
  LrInitPlugin = 'Client.lua', -- Main client logic
  LrPluginName = 'MIDI2LR',
  LrSdkMinimumVersion = 6.0, -- minimum SDK version required by this plug-in
  LrSdkVersion = 6.0,
  LrShutdownPlugin = 'ShutDownPlugin.lua',
  LrToolkitIdentifier = 'com.rsjaffe.midi2lr',
  LrExportMenuItems = {
    {
      title = LOC("$$$/AgCreations/NewCreationDialog/CreateItemDialog/Options=Options"),
      file = 'Options.lua',
    },
    {
      title = LOC("$$$/AgWPG/Dialogs/About/Title=About"),
      file = "About.lua",
    },
    {
      title = LOC("$$$/AgPluginManager/Status/HttpServer/StartServer=Start Server"),
      file = "LaunchServer.lua"
    },
    {
      title = LOC("$$$/AgPluginManager/Status/HttpServer/StopServer=Stop Server"),
      file = "StopServer.lua"
    },
    {
      title = LOC("$$$/AgMenu/LoadPresets=Load presets"),
      file = "FileLoadPref.lua",
    },
    {
      title = LOC("$$$/AgLibrary/Menu/File/ExportPresets=Export presets"),
      file = "FileSavePref.lua"
    },
    {
      title = LOC("$$$/MIDI2LR/Info/BuildFiles=Build files (development use only)"),
      file = "Build.lua"
    },
  },
  VERSION = { major=2, minor=0, revision=4, build=0}
}
