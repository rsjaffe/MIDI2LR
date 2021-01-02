--[[----------------------------------------------------------------------------

Options.lua

Manages options for plugin

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

local ActionSeries      = require 'ActionSeries'
local Filters           = require 'Filters'
local Keys              = require 'Keys'
local Limits            = require 'Limits'
local LocalPresets      = require 'LocalPresets'
local OU                = require 'OptionsUtilities'
local Preferences       = require 'Preferences'
local Profiles          = require 'Profiles'
local LrBinding         = import 'LrBinding'
local LrDialogs         = import 'LrDialogs'
local LrFunctionContext = import 'LrFunctionContext'
local LrView            = import 'LrView'
--[[-----------debug section, enable by adding - to beginning this line
local LrMobdebug = import 'LrMobdebug'
--]]-----------end debug section

local function setOptions()
  LrFunctionContext.callWithContext( "assignPresets", function( context )
      --[[-----------debug section, enable by adding - to beginning this line
      LrMobdebug.on()
      --]]-----------end debug section
      local f = LrView.osFactory()
      local properties = LrBinding.makePropertyTable( context )
      --following not managed by another module
      properties.ClientShowBezelOnChange = ProgramPreferences.ClientShowBezelOnChange
      properties.TrackingDelay = ProgramPreferences.TrackingDelay
      properties.RevealAdjustedControls = ProgramPreferences.RevealAdjustedControls

      -- assemble dialog box contents
      local contents =
      f:view{
        bind_to_object = properties, -- default bound table
        f:tab_view {
          f:tab_view_item{
            title = LOC("$$$/CRaw/Style/ProfileGroup/Profiles=Profile"),
            identifier = 'profiles',
            Profiles.StartDialog(properties,f),
          },
          f:tab_view_item{
            title = LOC("$$$/Library/Filter/FilterLabel=Library filter"):gsub(' ?:',''),
            identifier = 'filters',
            Filters.StartDialog(properties,f),
          },
          f:tab_view_item {
            title = LOC("$$$/AgPrint/ProfileMenu/Other=Other..."),
            identifier = 'othersettings',
            f:view(Limits.StartDialog(properties,f)),
            f:separator {fill_horizontal = 0.9},
            f:row {
              f:checkbox {title = LOC("$$$/AgDocument/ModulePicker/Settings/ShowStatusAndActivity=Show status and activity"), value = LrView.bind('ClientShowBezelOnChange')},
              f:checkbox {title = LOC("$$$/MIDI2LR/Options/RevealAdjustedControls=Reveal adjusted controls"), value = LrView.bind('RevealAdjustedControls')},
              f:spacer {width = 40},
              OU.slider(f,properties,LOC("$$$/MIDI2LR/Options/TrackingDelay=Tracking Delay"),'slidersets','TrackingDelay',0,3,2),
            }, -- row
            f:separator {fill_horizontal = 0.9},
            Keys.StartDialog(properties,f),
          }, -- tab_view_item
          f:tab_view_item {
            title = LOC("$$$/MIDI2LR/Shortcuts/SeriesofCommands=Series of commands"),
            identifier = 'commandseries',
            ActionSeries.StartDialog(properties,f),
          }, --tab_view_item
          f:tab_view_item {
            title = LOC("$$$/MIDI2LR/LocalPresets/Presets=Local adjustments presets"),
            identifier = 'localpresets',
            LocalPresets.StartDialog(properties,f),
          }, --tab_view_item
        }, -- tab_view
      } -- view

      -- display dialog
      local result = LrDialogs.presentModalDialog {
        title = LOC('$$$/MIDI2LR/Options/dlgtitle=Set MIDI2LR options'),
        contents = contents,
      }
      ActionSeries.EndDialog(properties,result)
      Filters.EndDialog(properties,result)
      Keys.EndDialog(properties,result)
      Limits.EndDialog(properties,result)
      LocalPresets.EndDialog(properties,result)
      Profiles.EndDialog(properties,result)
      if result == 'ok' then
        local LrDevelopController = import 'LrDevelopController'
        --following not managed by another lua module file
        ProgramPreferences.ClientShowBezelOnChange = properties.ClientShowBezelOnChange
        ProgramPreferences.TrackingDelay = properties.TrackingDelay
        if ProgramPreferences.TrackingDelay ~= nil then
          LrDevelopController.setTrackingDelay(ProgramPreferences.TrackingDelay)
        end

        if ProgramPreferences.RevealAdjustedControls ~= properties.RevealAdjustedControls then
          ProgramPreferences.RevealAdjustedControls = properties.RevealAdjustedControls
          LrDevelopController.revealAdjustedControls(ProgramPreferences.RevealAdjustedControls)
        end

        --then save preferences
        Preferences.Save()
      end -- if result ok
      -- finished with assigning values from dialog
    end
  )
end

setOptions() --execute
