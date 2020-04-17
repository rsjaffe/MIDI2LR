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

local Preferences       = require 'Preferences'
local Presets           = require 'Presets'
local LrBinding         = import 'LrBinding'
local LrDialogs         = import 'LrDialogs'
local LrFunctionContext = import 'LrFunctionContext'
local LrView            = import 'LrView'
--[[-----------debug section, enable by adding - to beginning this line
local LrMobdebug = import 'LrMobdebug'
--]]-----------end debug section

local function setOptions()
  LrFunctionContext.callWithContext( "assignPresetsOnly", function( context )
      --[[-----------debug section, enable by adding - to beginning this line
      LrMobdebug.on()
      --]]-----------end debug section
      local f = LrView.osFactory()
      local properties = LrBinding.makePropertyTable( context )
      -- assemble dialog box contents
      local contents = 
      f:view{
        bind_to_object = properties, -- default bound table
        Presets.StartDialog(properties,f),
      } -- view

      -- display dialog
      local result = LrDialogs.presentModalDialog {
        title = LOC("$$$/SmartCollection/Criteria/DevelopPreset=Develop Preset"),
        contents = contents,
      }
      Presets.EndDialog(properties,result)
      if result == 'ok' then
        --then save preferences
        Preferences.Save()
      end -- if result ok
      -- finished with assigning values from dialog
    end
  )
end
setOptions() --execute
