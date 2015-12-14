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
--The only global identifiers used in this module are 'import' and 'MIDI2LR'.
--In ZeroBrane IDE, check for global identifiers by pressing shift-F7.

--imports
local LrDevelopController = import 'LrDevelopController'
local prefs               = import 'LrPrefs'.prefsForPlugin() 
local LrView              = import 'LrView'

--hidden 
local DisplayOrder           = {'Temperature','Tint','Exposure'}



--public--each must be in table of exports

--------------------------------------------------------------------------------
-- Table listing parameter names managed by Limits module.
--------------------------------------------------------------------------------
local Parameters           = {Temperature = true, Tint = true, Exposure = true}

--------------------------------------------------------------------------------
-- Limits a given parameter to the min,max set up.
-- This function is used to avoid the bug in pickup mode, in which the parameter's
-- value may be too low or high to be picked up by the limited control range. By
-- clamping value to min-max range, this forces parameter to be in the limited
-- control range.
-- @param Parameter to clamp to limits.
-- @return nil.
--------------------------------------------------------------------------------
local function ClampValue(param)
  local _, rangemax = LrDevelopController.getRange(param)
  if Parameters[param] and (type(MIDI2LR[param..'Low']) == 'table') and MIDI2LR[param..'Low'][rangemax] then
    local min, max = MIDI2LR[param..'Low'][rangemax], MIDI2LR[param..'High'][rangemax]
    local value = LrDevelopController.getValue(param)
    if value < min then      
      MIDI2LR.PARAM_OBSERVER[param] = min
      LrDevelopController.setValue(param, min)
    elseif value > max then
      MIDI2LR.PARAM_OBSERVER[param] = max
      LrDevelopController.setValue(param, max)
    end
  end
end


--------------------------------------------------------------------------------
-- Get temperature limit preferences.
-- Returns all saved settings, including those for modes not currently in use
-- (e.g., jpg, raw, HDR). Will always return valid values for current mode,
-- and will update preferences if current mode was undefined.
-- @return Table containing preferences in form table['TemperatureLow'][50000],
-- where second index identifies max vaule for mode (jpg, raw, HDR).
--------------------------------------------------------------------------------
local function GetPreferences()
  local retval = {}
  -- following for people with defaults from version 0.7.0 or earlier
  local historic = {Temperature = 50000, Tint = 150, Exposure = 5}
  for p in pairs(Parameters) do
    local controlmin, controlmax = LrDevelopController.getRange(p)    
    --  prefs[p..'Low'] = prefs[p..'Low'] or {} -- if uninitialized
    if type(prefs[p..'Low']) ~= 'table' then -- need to wipe old preferences or initialize
      --make it into table and slot old value (if it exists) into proper place
      prefs[p..'Low'], prefs[p..'Low'][historic(p)] = {}, prefs[p..'Low'] 
    end
    retval[p..'Low'] = {}
    for i,v in pairs(prefs[p..'Low']) do--run through all saved ranges
      retval[p..'Low'][i] = v
    end
    retval[p..'Low'][controlmax] = retval[p..'Low'][controlmax] or controlmin
    --   prefs[p..'High'] = prefs[p..'High'] or {} -- if uninitialized
    if type(prefs[p..'High']) ~= 'table' then -- need to wipe old preferences or initialize
      --make it into table and slot old value (if it exists) into proper place      
      prefs[p..'High'], prefs[p..'High'][historic(p)] = {}, prefs[p..'High']
    end
    retval[p..'High'] = {}
    for i,v in pairs(prefs[p..'High']) do--run through all saved ranges
      retval[p..'High'][i] = v
    end
    retval[p..'High'][controlmax] = retval[p..'High'][controlmax] or controlmax
  end
  if retval.TemperatureLow[historic.Temperature] == nil then --defaults for temperature
    retval.TemperatureLow[historic.Temperature], retval.TemperatureHigh[historic.Temperature] = 3000, 9000
  end
  return retval
end

--------------------------------------------------------------------------------
-- Get temperature limit preferences for current mode.
-- Returns all saved settings, for the mode currently in use.
-- (e.g., jpg, raw, HDR). Will always return valid values,
-- and will update preferences if current mode was undefined.
-- @return Table containing preferences in form table['TemperatureLow'].
--------------------------------------------------------------------------------
local function GetPreferencesCurrentMode()
  local retval = {}
  -- following for people with defaults from version 0.7.0 or earlier
  local historic = {Temperature = 50000, Tint = 150, Exposure = 5}
  for p in pairs(Parameters) do
    local controlmin, controlmax = LrDevelopController.getRange(p)    
    if type(prefs[p..'Low']) ~= 'table' then -- need to wipe old preferences or initialize
      --make it into table and slot old value (if it exists) into proper place
      prefs[p..'Low'], prefs[p..'Low'][historic(p)] = {}, prefs[p..'Low'] 
    end
    retval[p..'Low'] = prefs[p..'Low'][controlmax] or controlmin
    if type(prefs[p..'High']) ~= 'table' then -- need to wipe old preferences or initialize
      --make it into table and slot old value (if it exists) into proper place      
      prefs[p..'High'], prefs[p..'High'][historic(p)] = {}, prefs[p..'High']
    end
    retval[p..'High'] = prefs[p..'High'][controlmax] or controlmax
  end
  return retval
end

--------------------------------------------------------------------------------
-- Save temperature limit preferences
-- Ignores any preferences not in the provided table.
-- @param saveme Table of preferences in form table['TemperatureLow'][50000],
-- where second index identifies max vaule for mode (jpg, raw, HDR).
-- @return nil.
--------------------------------------------------------------------------------
local function SavePreferences(saveme)
  for p in pairs(Parameters) do
    prefs[p..'Low'] = prefs[p..'Low'] or {} -- if uninitialized
    for i,v in pairs(saveme[p..'Low']) do
      prefs[p..'Low'][i] = v
    end
    prefs[p..'High'] = prefs[p..'High'] or {} -- if uninitialized
    for i,v in pairs(saveme[p..'High']) do
      prefs[p..'High'][i] = v
    end
  end
  prefs = prefs --force save -- LR may not notice changes otherwise
end

--------------------------------------------------------------------------------
-- Save temperature limit preferences
-- Ignores any preferences not in the provided table. Saves to plugin's preferences
-- unless destination parameter is set.
-- @param saveme Table of preferences in form table['TemperatureLow'].
-- @param destination Optional. If given, table to put values in. Otherwise, saves
-- them in preferences.
-- @return nil.
--------------------------------------------------------------------------------
local function SavePreferencesOneMode(saveme, destination)
  destination = destination or prefs
  for p in pairs(Parameters) do
    local _,highlimit = LrDevelopController.getRange(p)
    destination[p..'Low'] = destination[p..'Low'] or {} -- if uninitialized
    destination[p..'Low'][highlimit] = saveme[p..'Low']
    destination[p..'High'] = destination[p..'High'] or {} -- if uninitialized
    destination[p..'High'][highlimit] = saveme[p..'High']
  end
  if destination == prefs then
    prefs = prefs --force save -- LR may not notice changes otherwise
  end
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
  for _, p in ipairs(DisplayOrder) do
    local low,high = LrDevelopController.getRange(p)
    table.insert(
      retval,
      f:row { 
        f:static_text {
          title = p..' Limits',
          width = LrView.share('limit_label'),
        }, -- static_text
        f:slider {
          value = LrView.bind(p..'Low'),
          min = low, 
          max = high,
          integral = false,
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
          integral = false,
          width = LrView.share('limit_slider'),
        }, -- slider
        f:static_text {
          title = LrView.bind(p..'High'),
          alignment = 'right',
          width = LrView.share('limit_reading'),                
        }, -- static_text
        f:push_button {
          title = 'Reset to defaults',
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
  return retval -- array of rows
end

--------------------------------------------------------------------------------
-- Provides min and max for given parameter and mode.
-- @param param Which parameter is being adjusted.
-- @return min for given param and mode.
-- @return max for given param and mode.
--------------------------------------------------------------------------------
local function GetMinMax(param)
  if Parameters[param] and MIDI2LR[param..'High'] then
    local _, rangemax = LrDevelopController.getRange(param)
    return MIDI2LR[param..'Low'][rangemax], MIDI2LR[param..'High'][rangemax]
  else
    return LrDevelopController.getRange(param)
  end
end

--- @export
return { --table of exports, setting table member name and module function it points to
  ClampValue = ClampValue,
  GetMinMax = GetMinMax,
  GetPreferences = GetPreferences,
  GetPreferencesCurrentMode = GetPreferencesCurrentMode,
  OptionsRows = OptionsRows,
  Parameters = Parameters,
  SavePreferences = SavePreferences,
  SavePreferencesOneMode = SavePreferencesOneMode,
}