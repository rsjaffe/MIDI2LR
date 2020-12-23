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

local LrApplication       = import 'LrApplication'
local LrApplicationView   = import 'LrApplicationView'
local LrDevelopController = import 'LrDevelopController'
local LrView              = import 'LrView'
local Database            = require 'Database'

--hidden 
local DisplayOrder           = {'Temperature','Tint','Exposure','straightenAngle'}

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
    ProgramPreferences.Limits[k]['label'] = Database.CmdTrans[k][Database.LatestPVSupported]
  else
    ProgramPreferences.Limits[k] = nil --erase unused
  end
end

--------------------------------------------------------------------------------
-- Checks whether in Develop module and whether a photo is selected. This should
-- guard any calls that can reach into GetMinMax. Following is where calls should
-- be guarded. Choices were made based upon effect on program responsiveness.
-- Guard multiple calls to GetMinMax at one spot rather than further down the chain.
-- FullRefresh has test for LimitsCanBeSet, and can be called without checking.
-- MIDIValuetoLRValue has no test; all calls must check LimitsCanBeSet.
-- LRValueToMIDIValue has no test; all calls must check LimistCanBeSet.
-- ClampValue has no guard; all calls must be guarded.
-- doprofilechange is guarded.
-- @return bool as result of test
--------------------------------------------------------------------------------
local function LimitsCanBeSet()
  return 
  (LrApplication.activeCatalog():getTargetPhoto() ~= nil) and
  (LrApplicationView.getCurrentModuleName() == 'develop')
end

--------------------------------------------------------------------------------
-- Provides min and max for given parameter and mode. Must be called in Develop
-- module with photo selected.
-- @param param Which parameter is being adjusted.
-- @return min for given param and mode.
-- @return max for given param and mode.
--------------------------------------------------------------------------------
local function GetMinMax(param)
  local low,rangemax = LrDevelopController.getRange(param)
  if LimitParameters[param] then --should have limits
    if type(ProgramPreferences.Limits[param]) == 'table' and rangemax ~= nil then -- B&W picture may not have temperature, tint. This avoids indexing a nil rangemax and blowing up the metatable _index
      if type(ProgramPreferences.Limits[param][rangemax]) == 'table' then
        return ProgramPreferences.Limits[param][rangemax][1], ProgramPreferences.Limits[param][rangemax][2]
      else
        ProgramPreferences.Limits[param][rangemax] = {low, rangemax}
      end
    else
      ProgramPreferences.Limits[param] = {param = param, 
        label = Database.CmdTrans[param][Database.LatestPVSupported],
        rangemax = {low,rangemax}}
    end
  end
  return low, rangemax
end

--------------------------------------------------------------------------------
-- Limits a given parameter to the min,max set up.
-- This function is used to avoid the bug in pickup mode, in which the parameter's
-- value may be too low or high to be picked up by the limited control range. By
-- clamping value to min-max range, this forces parameter to be in the limited
-- control range. This function should not be called unless in develop with
-- photo selected. Also check for existence of limits before calling.
-- @param Parameter to clamp to limits.
-- @return nil.
--------------------------------------------------------------------------------
local function ClampValue(param)
  local min, max = GetMinMax(param)
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
    retval[#retval+1] = f:row { 
      f:static_text {
        title = ProgramPreferences.Limits[p]['label']..' '..LOC('$$$/MIDI2LR/Limits/Limits=Limits'),
        width = LrView.share('limit_label'),
      }, -- static_text
      f:static_text {
        title = LrView.bind('Limits'..p..'Low'),
        alignment = 'right',
        width = LrView.share('limit_reading'),  
      }, -- static_text
      f:slider {
        value = LrView.bind('Limits'..p..'Low'),
        min = low, 
        max = high,
        integral = integral,
        width = LrView.share('limit_slider'),
      }, -- slider
      f:static_text {
        title = LrView.bind('Limits'..p..'High'),
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
      f:push_button {
        title = LOC("$$$/AgLibrary/CameraRawView/PresetMenu/DefaultSettings=Default settings"),
        action = function ()
          if p == 'Temperature' and low > 0 then
            obstable.LimitsTemperatureLow = 3000
            obstable.LimitsTemperatureHigh = 9000
          else
            obstable['Limits'..p..'Low'] = low
            obstable['Limits'..p..'High'] = high
          end
        end,
      }, -- push_button
    } -- row
  end
  return retval -- array of rows
end

local function StartDialog(obstable,f)
  if LimitsCanBeSet() then
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
  if status == 'ok' and LimitsCanBeSet() then
    --assign limits
    for _,p in ipairs(DisplayOrder) do
      if obstable['Limits'..p..'Low'] ~= nil and obstable['Limits'..p..'High'] ~= nil then
        if obstable['Limits'..p..'Low'] > obstable['Limits'..p..'High'] then --swap values
          obstable['Limits'..p..'Low'], obstable['Limits'..p..'High'] = obstable['Limits'..p..'High'], obstable['Limits'..p..'Low']
        end
        local _,max = LrDevelopController.getRange(p) --limitsCanBeSet only when in Develop module, no need to check again
        ProgramPreferences.Limits[p][max] = {obstable['Limits'..p..'Low'], obstable['Limits'..p..'High']}
      end
    end
  end
end

return {
  ClampValue  = ClampValue,
  EndDialog   = EndDialog,
  GetMinMax   = GetMinMax,
  LimitsCanBeSet = LimitsCanBeSet,
  Parameters  = LimitParameters,
  StartDialog = StartDialog,
}