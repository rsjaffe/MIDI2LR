--[[----------------------------------------------------------------------------

ActionSeries.lua

Manages action series for plugin
 
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

local CU                  = require 'ClientUtilities'
local Database            = require 'Database'
local LrDevelopController = import 'LrDevelopController'
local LrTasks             = import 'LrTasks'
local LrView              = import 'LrView'

Database.ValidActions.Pause = true --this is in addition to the menu list

local function ValidateActions(_,value)
  local stack = nil 
  for a in value:gmatch("[%w_]+") do
    if not Database.ValidActions[a] then
      if not stack then
        stack = a
      else
        stack = stack..LOC("$$$/Application/listSeparator=, ")..a
      end
    end
  end
  if stack then
    return false,value,LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SettingsString/ConstructionWithColon=^1: ^2",LOC("$$$/MIDI2LR/Options/ActionSeriesInvalid=Invalid commands"),stack)
  else
    return true,value
  end
end

local numseries = 16

local function StartDialog(obstable,f)
  for i = 1,numseries do
    obstable['ActionSeries'..i] = ProgramPreferences.ActionSeries[i]
  end
  local dlgrows = {}
  for i=1, numseries do
    dlgrows[i] = f:row{
      bind_to_object = obstable, -- default bound table
      f:static_text{title = i,  width = LrView.share('command_series_label')},
      f:edit_field{
        value = LrView.bind('ActionSeries'..i),
        height_in_lines = 2,
        width_in_chars = 80,
        validate = ValidateActions,
      }
    }
  end 
  return 
  f:column(dlgrows)
end

local function EndDialog(obstable, status)
  if status == 'ok' then
    ProgramPreferences.ActionSeries = {} -- empty out prior settings
    for i = 1,numseries do
      ProgramPreferences.ActionSeries[i] = obstable['ActionSeries'..i]
    end
  end
end

local function RunActionSeries(strarg1,actarray)
  if strarg1 == nil or strarg1 == '' then 
    MIDI2LR.SERVER:send('Log Empty string in RunActionSeries\n')
    return 
  end
  local strarg = strarg1 -- make argument available to async task
  LrTasks.startAsyncTask(
    function()
      --[[-----------debug section, enable by adding - to beginning this line
          LrMobdebug.on()
          --]]-----------end debug section
      --currently only accepts items in Database assigned 'button' and saved in Database.ValidActions
      --will have to parse into command and value if want to go to parameterized commands (e.g., slider change)
      for i in strarg:gmatch("[%w_]+") do
        if actarray[i] then -- perform a one time action
          actarray[i]()
        elseif i:sub(1,5) == 'Reset' then -- perform a reset other than those explicitly coded in ACTIONS array
          local resetparam = i:sub(6)
          CU.execFOM(LrDevelopController.resetToDefault,resetparam)
          if ProgramPreferences.ClientShowBezelOnChange then
            local lrvalue = LrDevelopController.getValue(resetparam)
            CU.showBezel(resetparam,lrvalue)
          end
        end
        LrTasks.sleep(0.01) --pause between actions to allow synchronization with slow LR responses and keystrokes from app
      end
    end 
  )
end

return {
  Run = RunActionSeries,
  StartDialog = StartDialog,
  EndDialog = EndDialog,
}
