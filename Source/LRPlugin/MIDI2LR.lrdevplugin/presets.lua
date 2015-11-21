--[[----------------------------------------------------------------------------
presets.lua
Enumerates presets in Lightroom for MIDI2LR
Saves selected presets for mapping to MIDI2LR commands
------------------------------------------------------------------------------]]
--[[
This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with MIDI2LR.  If not, see <http://www.gnu.org/licenses/>. 
--]]
local LrApplication = import 'LrApplication'
local LrBinding = import 'LrBinding'
local LrDialogs = import 'LrDialogs'
local LrFunctionContext = import 'LrFunctionContext'
local prefs = import 'LrPrefs'.prefsForPlugin() 
local LrView = import 'LrView'
local psList = {}

local function assignPresets()
  LrFunctionContext.callWithContext( "assignPresets", function( context )

      for _,fold in pairs(LrApplication.developPresetFolders()) do
        local foldname = fold:getName()
        for _,pst in pairs(fold:getDevelopPresets()) do
          table.insert(psList,{title = foldname..'->'..pst:getName(), value = pst:getUuid()})
        end
      end

      local props = LrBinding.makePropertyTable( context )
      if (prefs.presetsbyUuid == null) then
        prefs.presetsbyUuid = {}
      end
      props.presetsbyUuid = prefs.presetsbyUuid -- start with saved presets

      local f = LrView.osFactory()
      local font = "<system/small>"
      LrDialogs.presentModalDialog {title = "Assign Presets", resizable = true,
        contents = f:column {
          bind_to_object = props,
          f:static_text {title = "Available Presets", font = font},
          f:simple_list {font = font, items = psList},
        }
      }
      prefs.presetsbyUuid = {} -- replace with table derived from dialog
    end )
end

assignPresets() --execute
