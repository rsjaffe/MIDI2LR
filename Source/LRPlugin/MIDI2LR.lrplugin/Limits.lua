--------------------------------------------------------------------------------
-- Provides methods to manage control limits.
-- This allows limited-range MIDI controls (0-127) to control a LR parameter
-- with reasonable accuracy. It limits the total range that particular MIDI
-- control affects.
-- @module Limits
-- @license GNU GPLv3
--------------------------------------------------------------------------------

--[[
This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>. 
--]]
--All variables and functions defined here must be local, except for return table.
--The only global identifiers used in this module are 'import', 'LOC' and 'MIDI2LR'.
--In ZeroBrane IDE, check for global identifiers by pressing shift-F7 while 
--viewing this file.
--Limits are now stored in the form Limits.Temperature[50000]{3000,9000} in the Preferences module

--imports
local LrApplication       = import 'LrApplication'
local LrApplicationView   = import 'LrApplicationView'
local LrDevelopController = import 'LrDevelopController'
local LrView              = import 'LrView'
local ParamList           = require 'ParamList'

--hidden 
local DisplayOrder           = {'Temperature','Tint','Exposure'}

--public--each must be in table of exports

--------------------------------------------------------------------------------
-- Table listing parameter names managed by Limits module.
--------------------------------------------------------------------------------
local LimitParameters           = {}--{Temperature = true, Tint = true, Exposure = true}
for _, p in ipairs(DisplayOrder) do
  LimitParameters[p] = true
end

--clean up the preferences of discarded Limits and fix label and order
--has to be done after init and after loading full limits file

for k in pairs(ProgramPreferences.Limits) do
  if LimitParameters[k] then
    ProgramPreferences.Limits[k]['label'] = ParamList.LimitEligible[k][1]
    ProgramPreferences.Limits[k]['order'] = ParamList.LimitEligible[k][2]
  else
    ProgramPreferences.Limits[k] = nil --erase unused
  end
end

--------------------------------------------------------------------------------
-- Limits a given parameter to the min,max set up.
-- This function is used to avoid the bug in pickup mode, in which the parameter's
-- value may be too low or high to be picked up by the limited control range. By
-- clamping value to min-max range, this forces parameter to be in the limited
-- control range. This function immediately returns without doing anything if the
-- Develop module isn't active or the parameter is not limited.
-- @param Parameter to clamp to limits.
-- @return nil.
--------------------------------------------------------------------------------
local function ClampValue(param)
  if LimitParameters[param] == nil or LrApplicationView.getCurrentModuleName() ~= 'develop' or LrApplication.activeCatalog():getTargetPhoto() == nil then return nil end 
  local _, rangemax = LrDevelopController.getRange(param)
  local min, max = unpack(ProgramPreferences.Limits[param][rangemax])
  local value = LrDevelopController.getValue(param)
  if value < min then      
    MIDI2LR.PARAM_OBSERVER[param] = min
    LrDevelopController.setValue(param, min)
  elseif value > max then
    MIDI2LR.PARAM_OBSERVER[param] = max
    LrDevelopController.setValue(param, max)
  end
  return nil
end

--------------------------------------------------------------------------------
-- Provide rows of controls for dialog boxes.
-- For the current photo type (HDR, raw, jpg, etc) will produce
-- rows that allow user to set limits. Must be in develop module and must
-- have photo selected before calling this function.
-- @param f The LrView.osfactory to use.
-- @param obstable The observable table to bind to dialog controls.
-- @return Table of f:rows populated with the needed dialog controls.
--------------------------------------------------------------------------------
local function OptionsRows(f,obstable)
  local retval = {}
  for _, p in ipairs(DisplayOrder) do
    local low,high = LrDevelopController.getRange(p)
    local integral = high - 5 > low
    table.insert( retval,
      f:row { 
        f:static_text {
          title = ProgramPreferences.Limits[p]['label']..' '..LOC('$$$/MIDI2LR/Limits/Limits=Limits'),
          width = LrView.share('limit_label'),
        }, -- static_text
        f:slider {
          value = LrView.bind('Limits'..p..'Low'),
          min = low, 
          max = high,
          integral = integral,
          width = LrView.share('limit_slider'),
        }, -- slider
        f:static_text {
          title = LrView.bind('Limits'..p..'Low'),
          alignment = 'right',
          width = LrView.share('limit_reading'),  
        }, -- static_text
        f:slider {
          value = LrView.bind('Limits'..p..'High'),
          min = low ,
          max = high,
          integral = integral,
          width = LrView.share('limit_slider'),
        }, -- slider
        f:static_text {
          title = LrView.bind('Limits'..p..'High'),
          alignment = 'right',
          width = LrView.share('limit_reading'),                
        }, -- static_text
        f:push_button {
          title = LOC("$$$/AgLibrary/CameraRawView/PresetMenu/DefaultSettings=Default settings"),
          action = function ()
            if p == 'Temperature' and low > 0 then
              obstable.TemperatureLow = 3000
              obstable.TemperatureHigh = 9000
            else
              obstable['Limits'..p..'Low'] = low
              obstable['Limits'..p..'High'] = high
            end
          end,
        }, -- push_button
      } -- row
    ) -- table.insert
  end
  return retval -- array of rows
end

--------------------------------------------------------------------------------
-- Provides min and max for given parameter and mode.
-- @param param Which parameter is being adjusted.
-- @return min for given param and mode.
-- @return max for given param and mode.
--------------------------------------------------------------------------------
local function GetMinMax(param)
  if LimitParameters[param] then
    return ProgramPreferences.Limits[param][rangemax][1], ProgramPreferences.Limits[param][rangemax][2]
  else
    return LrDevelopController.getRange(param)
  end
end


local function StartDialog(obstable,f)
  local limitsCanBeSet = (LrApplication.activeCatalog():getTargetPhoto() ~= nil) and (LrApplicationView.getCurrentModuleName() == 'develop')
  if limitsCanBeSet then
    for _,p in ipairs(DisplayOrder) do
      local min,max = GetMinMax(p)
      obstable['Limits'..p..'Low'] = min
      obstable['Limits'..p..'High'] = max
    end
    return OptionsRows(f,obstable)
  end
  return {visible = false}
end

local function EndDialog(obstable, status)
  if status == 'ok' then
    local limitsCanBeSet = (LrApplication.activeCatalog():getTargetPhoto() ~= nil) and (LrApplicationView.getCurrentModuleName() == 'develop')
    --assign limits
    if limitsCanBeSet then -- do NOT empty out prior settings, this is setting for one type picture only
      for _,p in ipairs(DisplayOrder) do
        if obstable['Limits'..p..'Low'] ~= nil and obstable['Limits'..p..'High'] ~= nil then
          if obstable['Limits'..p..'Low'] > obstable['Limits'..p..'High'] then --swap values
            obstable['Limits'..p..'Low'], obstable['Limits'..p..'High'] = obstable['Limits'..p..'High'], obstable['Limits'..p..'Low']
          end
          local _,max = LrDevelopController.getRange(p) --limitsCanBeSet only when in Develop module, no need to check again
          ProgramPreferences.Limits[p][max] = {obstable['Limits'..p..'Low'], obstable['Limits'..p..'High']}
        end
      end
    end --if limitsCanBeSet
  end
end

--- @export
return { --table of exports, setting table member name and module function it points to
  ClampValue  = ClampValue,
  EndDialog   = EndDialog,
  GetMinMax   = GetMinMax,
  Parameters  = LimitParameters,
  StartDialog = StartDialog,
}