--[[----------------------------------------------------------------------------

Options.lua

Manages options for plugin
 
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

local Limits            = require 'Limits' 
local Paste             = require 'Paste'
local Preferences       = require 'Preferences'
local Presets           = require 'Presets'
local Profiles          = require 'Profiles'
local LrBinding         = import 'LrBinding'
local LrDialogs         = import 'LrDialogs'
local LrFunctionContext = import 'LrFunctionContext'
local LrView            = import 'LrView'

local function setOptions()
  LrFunctionContext.callWithContext( "assignPresets", function( context )
      local f = LrView.osFactory()
      local properties = LrBinding.makePropertyTable( context )

      -- assemble dialog box contents
      local contents = 
      f:view{
        bind_to_object = properties, -- default bound table
        f:tab_view {
          f:tab_view_item {
            title = LOC("$$$/SmartCollection/Criteria/DevelopPreset=Develop Preset"),
            identifier = 'presets',
            Presets.StartDialog(properties,f),
          }, -- tab_view_item 
          f:tab_view_item {
            title = LOC('$$$/MIDI2LR/Options/pastesel=Paste selections'),
            identifier = 'pasteselections',
            Paste.StartDialog(properties,f),
          }, -- tab_view_item
          f:tab_view_item{
            title = LOC("$$$/CRaw/Style/Profiles=Profiles"),
            identifier = 'profiles',
            Profiles.StartDialog(properties,f),
          },
          f:tab_view_item {
            title = LOC("$$$/AgPrint/ProfileMenu/Other=Other..."),
            identifier = 'othersettings',
            Limits.StartDialog(properties,f),
          }, -- tab_view_item
        }, -- tab_view
      } -- view

      -- display dialog
      local result = LrDialogs.presentModalDialog {
        title = LOC('$$$/MIDI2LR/Options/dlgtitle=Set MIDI2LR options'),
        contents = contents,
      }

      Limits.EndDialog(properties,result)
      Presets.EndDialog(properties,result)
      Paste.EndDialog(properties,result)
      Profiles.EndDialog(properties,result)
      if result == 'ok' then
        Preferences.Save()
      end -- if result ok
      -- finished with assigning values from dialog
    end
  )
end
setOptions() --execute
