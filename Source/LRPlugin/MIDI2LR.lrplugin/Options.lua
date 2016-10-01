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

local Keys              = require 'Keys'
local Limits            = require 'Limits' 
local OU                = require 'OptionsUtilities'
local Paste             = require 'Paste'
local Preferences       = require 'Preferences'
local Presets           = require 'Presets'
local Profiles          = require 'Profiles'
local LrBinding         = import 'LrBinding'
local LrDialogs         = import 'LrDialogs'
local LrFunctionContext = import 'LrFunctionContext'
local LrView            = import 'LrView'

local LrDevelopController = import 'LrDevelopController'
local listoflocaladjustments = {{"local_Temperature",LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments")..' '..LOC("$$$/AgDevelop/Localized/Temperature=Temp.").." (PV2012)"},
  {"local_Tint",LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments")..' '..LOC("$$$/AgDevelop/Localized/Tint=Tint").." (PV2012)"},
  {"local_Exposure",LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments")..' '..LOC("$$$/AgDevelop/Localized/Exposure=Exposure").." (PV2010 and PV2012)"},
  {"local_Contrast",LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments")..' '..LOC("$$$/AgDevelop/Localized/Contrast=Contrast").." (PV2010 and PV2012)"},
  {"local_Highlights",LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments")..' '..LOC("$$$/AgDevelop/Localized/Highlights=Highlights").." (PV2012)"},
  {"local_Shadows",LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments")..' '..LOC("$$$/AgDevelop/Localized/Shadows=Shadows").." (PV2012)"},
  {"local_Whites2012",false,false,true,true,false,true,LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments")..' '..LOC("$$$/AgDevelop/Localized/Whites=Whites").." (PV2012)"},
  {"local_Blacks2012",false,false,true,true,false,true,LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments")..' '..LOC("$$$/AgDevelop/Localized/Blacks=Blacks").." (PV2012)"},
  {"local_Clarity",LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments")..' '..LOC("$$$/AgDevelop/Localized/Clarity=Clarity").." (PV2010 and PV2012)"},
  {"local_Dehaze",false,false,true,true,false,true,LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments")..' '..LOC("$$$/AgDevelop/Localized/Dehaze=Dehaze").." (PV2012)"},
  {"local_Saturation",LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments")..' '..LOC("$$$/AgDevelop/Localized/Saturation=Saturation").." (PV2010 and PV2012)"},
  {"local_Sharpness",LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments")..' '..LOC("$$$/AgDevelop/Localized/Sharpness=Sharpness").." (PV2010 and PV2012)"},
  {"local_LuminanceNoise",LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments")..' '..LOC("$$$/AgDevelop/Localized/LuminanceNoiseReduction=Luminence Noise Reduction").." (PV2012)"},
  {"local_Moire",LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments")..' '..LOC("$$$/AgDevelop/Localized/MoireReduction=Moire").." (PV2012)"},
  {"local_Defringe",LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments")..' '..LOC("$$$/AgDevelop/Localized/Defringe=Defringe").." (PV2012)"},
  {"local_ToningLuminance","Local Toning Luminance (PV2010)"},
}

local innercolumn = {}
for k,v in pairs(listoflocaladjustments) do
  local low,high = LrDevelopController.getRange(k)
  table.insert(innercolumn, OU.slider(f,properties,v,'sliderset1',k,low,high,(low+high)/2))
end

  

local function setOptions()
  LrFunctionContext.callWithContext( "assignPresets", function( context )
      local f = LrView.osFactory()
      local properties = LrBinding.makePropertyTable( context )
      --following not managed by another module
      properties.ClientShowBezelOnChange = ProgramPreferences.ClientShowBezelOnChange
      properties.TrackingDelay = ProgramPreferences.TrackingDelay

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
          f:tab_view_item{
            title = LOC ("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments"),
            identifier = 'localadjustments',
            unpack (innercolumn),
          },
          f:tab_view_item {
            title = LOC("$$$/AgPrint/ProfileMenu/Other=Other..."),
            identifier = 'othersettings',
            f:view(Limits.StartDialog(properties,f)),
            f:separator {fill_horizontal = 0.9},
            f:checkbox {title = LOC("$$$/AgDocument/ModulePicker/Settings/ShowStatusAndActivity=Show status and activity"), value = LrView.bind('ClientShowBezelOnChange')},
            OU.slider(f,properties,LOC("$$$/MIDI2LR/Options/TrackingDelay=Tracking Delay"),'slidersets','TrackingDelay',0,3,2),
            f:separator {fill_horizontal = 0.9},
            Keys.StartDialog(properties,f),
          }, -- tab_view_item
        }, -- tab_view
      } -- view

      -- display dialog
      local result = LrDialogs.presentModalDialog {
        title = LOC('$$$/MIDI2LR/Options/dlgtitle=Set MIDI2LR options'),
        contents = contents,
      }
      Keys.EndDialog(properties,result)
      Limits.EndDialog(properties,result)
      Presets.EndDialog(properties,result)
      Paste.EndDialog(properties,result)
      Profiles.EndDialog(properties,result)
      if result == 'ok' then
        --following not managed by another module
        ProgramPreferences.ClientShowBezelOnChange = properties.ClientShowBezelOnChange
        ProgramPreferences.TrackingDelay = properties.TrackingDelay
        if ProgramPreferences.TrackingDelay ~= nil then
          local LrDevelopController = import 'LrDevelopController'
          LrDevelopController.setTrackingDelay(ProgramPreferences.TrackingDelay)
        end
        --then save preferences
        Preferences.Save()
      end -- if result ok
      -- finished with assigning values from dialog
    end
  )
end
setOptions() --execute
