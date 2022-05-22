--[[----------------------------------------------------------------------------

Info.lua
MIDI2LR Plugin properties

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

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
  LrAlsoUseBuiltInTranslations = true,
  LrForceInitPlugin = true,
  LrInitPlugin = 'Client.lua', -- Main client logic
  LrPluginInfoUrl = 'https://github.com/rsjaffe/MIDI2LR/wiki',
  LrPluginName = 'MIDI2LR',
  LrSdkMinimumVersion = 11.0, -- minimum SDK version required by this plug-in
  LrSdkVersion = 11.0,
  LrShutdownPlugin = 'ShutDownPlugin.lua',
  LrToolkitIdentifier = 'com.rsjaffe.midi2lr',
  LrExportMenuItems = {
    {
      title = LOC("$$$/MIDI2LR/Menu/GeneralOptions=General &options"),
      file = 'Options.lua',
    },
    {
      title = LOC("$$$/SmartCollection/Criteria/DevelopPreset=Develop Preset"),
      file = 'PresetsDialog.lua',
    },
    {
      title = LOC("$$$/AgLibrary/Filter/BrowserCriteria/Keywords=Keywords"),
      file = 'KeywordsDialog.lua',
    },
    {
      title = LOC("$$$/MIDI2LR/Menu/StartApplication=&Start application"),
      file = "LaunchServer.lua"
    },
    {
      title = LOC("$$$/MIDI2LR/Menu/CloseApplication=&Close application"),
      file = "StopServer.lua"
    },
    {
      title = LOC("$$$/MIDI2LR/Menu/ImportConfiguration=&Import configuration"),
      file = "FileLoadPref.lua",
    },
    {
      title = LOC("$$$/MIDI2LR/Menu/ExportConfiguration=&Export configuration"),
      file = "FileSavePref.lua"
    },
    {
      title = LOC("$$$/MIDI2LR/Help/OnlineHelp=Online &help"),
      file = "OnlineHelp.lua",
    },
    {
      title = LOC("$$$/MIDI2LR/Info/DeleteSettings=Delete settings"),
      file = "PrefReset.lua",
    },
    {
      title = LOC("$$$/MIDI2LR/Info/BuildFiles=Build files (development use only)"),
      file = "Build.lua"
    },
  },
  LrHelpMenuItems = {
    {
      title = LOC("$$$/MIDI2LR/Help/OnlineHelp=&Online help"),
      file = "OnlineHelp.lua",
    },
    {
      title = LOC("$$$/MIDI2LR/Help/Support=&Support"),
      file = "Support.lua",
    },
    {
      title = LOC("$$$/MIDI2LR/Help/Latest=&View latest release"),
      file = "Latest.lua",
    },
    {
      title = LOC("$$$/MIDI2LR/Help/Donate=&Donate to support project development"),
      file = "Donate.lua",
    },
    {
      title = LOC("$$$/AgWPG/Dialogs/About/Title=About"),
      file = "About.lua",
    },
    {
      title = LOC("$$$/AgPreferences/Sync/GenerateFullDiagnosticLog=Generate diagnostic report"),
      file = "LogSave.lua",
    },
  },
  VERSION = { major=5, minor=4, revision=0, build=0}
}
