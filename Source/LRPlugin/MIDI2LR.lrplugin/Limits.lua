--------------------------------------------------------------------------------
-- Provides methods to manage control limits.
-- This allows limited-range MIDI controls (0-127) to control a LR parameter
-- with reasonable accuracy. It limits the total range that particular MIDI
-- control affects.
-- @module Limits
-- @license GNU GPLv3
--------------------------------------------------------------------------------

--[[
This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

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
local Preferences         = require 'Preferences'

--hidden 
local DisplayOrder           = {'Temperature','Tint','Exposure'}

--public--each must be in table of exports

--------------------------------------------------------------------------------
-- Table listing parameter names managed by Limits module.
--------------------------------------------------------------------------------
local Parameters           = {}--{Temperature = true, Tint = true, Exposure = true}
for _, p in ipairs(DisplayOrder) do
  Parameters[p] = true
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
  if LrApplicationView.getCurrentModuleName() ~= 'develop' or Parameters[param] == nil or LrApplication.activeCatalog():getTargetPhoto() == nil then return nil end 
  local _, rangemax = LrDevelopController.getRange(param)
  local min, max = unpack(Preferences.Limits[param][rangemax])
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
-- rows that allow user to set limits.
-- @param f The LrView.osfactory to use.
-- @param obstable The observable table to bind to dialog controls.
-- @return Table of f:rows populated with the needed dialog controls.
--------------------------------------------------------------------------------
local function OptionsRows(f,obstable)
  local retval = {}
  local currentModule = LrApplicationView.getCurrentModuleName()
  if currentModule ~= 'develop' then
    LrApplicationView.switchToModule('develop') -- for getRange
  end
  for _, p in ipairs(DisplayOrder) do
    local low,high = LrDevelopController.getRange(p)
    local integral = high - 5 > low
    table.insert( retval,
      f:row { 
        f:static_text {
          title = p..' '..LOC('$$$/MIDI2LR/Limits/Limits=Limits'),
          width = LrView.share('limit_label'),
        }, -- static_text
        f:slider {
          value = LrView.bind(p..'Low'),
          min = low, 
          max = high,
          integral = integral,
          width = LrView.share('limit_slider'),
        }, -- slider
        f:static_text {
          title = LrView.bind(p..'Low'),
          alignment = 'right',
          width = LrView.share('limit_reading'),  
        }, -- static_text
        f:slider {
          value = LrView.bind(p..'High'),
          min = low ,
          max = high,
          integral = integral,
          width = LrView.share('limit_slider'),
        }, -- slider
        f:static_text {
          title = LrView.bind(p..'High'),
          alignment = 'right',
          width = LrView.share('limit_reading'),                
        }, -- static_text
        f:push_button {
          title = LOC('$$$/MIDI2LR/Limits/reset=Reset to defaults'),
          action = function ()
            if p == 'Temperature' and low > 0 then
              obstable.TemperatureLow = 3000
              obstable.TemperatureHigh = 9000
            else
              obstable[p..'Low'] = low
              obstable[p..'High'] = high
            end
          end,
        }, -- push_button
      } -- row
    ) -- table.insert
  end
  if currentModule ~= 'develop' then
    LrApplicationView.switchToModule(currentModule)
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
  if LrApplicationView.getCurrentModuleName() ~= 'develop' or LrApplication.activeCatalog():getTargetPhoto() == nil then return nil,nil end
  local _, rangemax = LrDevelopController.getRange(param)
  return unpack(Preferences.Limits[param][rangemax])
end

--- @export
return { --table of exports, setting table member name and module function it points to
  ClampValue = ClampValue,
  GetMinMax = GetMinMax,
  OptionsRows = OptionsRows,
}