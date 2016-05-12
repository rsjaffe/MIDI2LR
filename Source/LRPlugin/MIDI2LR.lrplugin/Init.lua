--[[----------------------------------------------------------------------------

Init.lua

Contains routines from modules that must be run before the full module
can be included. Because Lua requires that the entire module be
'compiled' when it is included, phased initialization requires separation
of some of the init routines from the body. This holds init routines for
several modules and is required by the Preferences module, which handles 
initialization.

Each type needs to have a Loaded and UseDefaults function.
 
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

local LrApplication       = import 'LrApplication'
local LrApplicationView   = import 'LrApplicationView'
local LrDevelopController = import 'LrDevelopController'
local ParamList           = require 'ParamList'
local ProfileTypes        = require 'ProfileTypes'

--Limits.lua
--------------note: test if can use t in place of t.param in metalimit2
local metalimit2 = { --assumes only new table members for each parameter are numeric, representing ranges
  __index = function(t,k) -- key is high value for the range -- return t[k] or nil
    if LrApplicationView.getCurrentModuleName() == 'develop' and LrApplication.activeCatalog():getTargetPhoto() ~= nil then 
      t[k] = {}
      local lo,hi = LrDevelopController.getRange(t.param)
      if k == hi then
        if t.param == 'Temperature' and k == 50000 then
          t[k] = {3000,9000}
        else
          t[k] = {lo,hi}
        end
      end
      return t[k]
    end
    return nil,nil --don't initialize t[k] so that __index reruns on next access
  end,
}

local metalimit1 = {
  __index = function(t,k)--key is the name of the parameter
    t[k] = setmetatable({param = k,label = ParamList.LimitEligible[k][1], order = ParamList.LimitEligible[k][2]},metalimit2) 
    return t[k]
  end,
}

local function UseDefaultsLimits()
  ProgramPreferences.Limits = setmetatable({},metalimit1)
  ProgramPreferences.Limits.Temperature= {[50000] = {3000,9000}}
end
local function LoadedLimits()
  if ProgramPreferences.Limits == nil or type(ProgramPreferences.Limits)~='table' then
    ProgramPreferences.Limits = {}
    ProgramPreferences.Limits.Temperature = {[50000] = {3000,9000}}
  else --following is just in cases we loaded historic limits, which may have a missing value--no harm in checking carefully
    for _,v in pairs(ProgramPreferences.Limits) do--for each Limit type _ (e.g., Temperature) look at v(table) (highlimits)
      for kk,vv in pairs(v) do -- for each highlimittable kk, look at vv(limit values table) 
        if type(kk)=='number' and (vv[1]==nil or vv[2]==nil) then -- table for each parameter has limits and other data (param,label.order)--ignore other data
          vv[1],vv[2]=nil,nil --guard against corrupted data when only one bound gets initialized
        end
      end
    end
  end
  if getmetatable(ProgramPreferences.Limits)==nil then
    setmetatable(ProgramPreferences.Limits,metalimit1)
    for k in pairs(ProgramPreferences.Limits) do -- k is parameter name, v is table under name
      setmetatable(ProgramPreferences.Limits[k],metalimit2)
    end
  end
end

--Paste.lua
local function UseDefaultsPaste()
  ProgramPreferences.PasteList = {}
  ProgramPreferences.PastePopup = false
end
local function LoadedPaste()
  if type(ProgramPreferences.PasteList) ~= 'table' then
    UseDefaultsPaste()
  end
end

--Presets.lua
local function UseDefaultsPresets()
  ProgramPreferences.Presets = {}
end
local function LoadedPresets()
  if type(ProgramPreferences.Presets) ~= 'table' then
    UseDefaultsPresets()
  end
end

--Profiles.lua
local function UseDefaultsProfiles()
  ProgramPreferences.Profiles = {}
  for k in pairs(ProfileTypes) do
    ProgramPreferences.Profiles[k] = ''
  end
end
local function LoadedProfiles()
  if type(ProgramPreferences.Profiles) ~= 'table' then
    UseDefaultsProfiles()
  end
end

--Keys.lua
local function UseDefaultsKeys()
  ProgramPreferences.Keys = {}
  for i = 1, 40 do
    ProgramPreferences.Keys[i] = {control = false, alt = false, shift = false, key = ''}
  end
end
local function LoadedKeys()
  if type(ProgramPreferences.Keys) ~= 'table' then
    UseDefaultsKeys()
  end
end


return {
  LoadedKeys          = LoadedKeys,
  LoadedLimits        = LoadedLimits,
  LoadedPaste         = LoadedPaste,
  LoadedPresets       = LoadedPresets,
  LoadedProfiles      = LoadedProfiles,
  UseDefaultsKeys     = UseDefaultsKeys,
  UseDefaultsLimits   = UseDefaultsLimits,
  UseDefaultsPaste    = UseDefaultsPaste,
  UseDefaultsPresets  = UseDefaultsPresets,
  UseDefaultsProfiles = UseDefaultsProfiles,
}
