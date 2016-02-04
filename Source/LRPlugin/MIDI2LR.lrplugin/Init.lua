--[[----------------------------------------------------------------------------

Init.lua

Contains routines from modules that must be run before the full module
can be included. Because Lua requires that the entire module be
'compiled' when it is included, phased initialization requires separation
of some of the init routines from the body. This holds init routines for
several modules and is required by the Preferences module, which handles 
initialization.
 
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
  __index = function(t,k) -- key is high value for the range -- always return t[k]!
    t[k] = {}
    if LrApplicationView.getCurrentModuleName() == 'develop' and LrApplication.activeCatalog():getTargetPhoto() ~= nil then 
      local lo,hi = LrDevelopController.getRange(t.param)
      if k == hi then
        if t.param == 'Temperature' and k == 50000 then
          t[k] = {3000,9000}
        else
          t[k] = {lo,hi}
        end
      end
    end
    return t[k]
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
  if ProgramPreferences.Limits == nil then
    ProgramPreferences.Limits = {}
    ProgramPreferences.Limits.Temperature = {[50000] = {3000,9000}}
  end
  if getmetatable(ProgramPreferences.Limits)==nil then
    setmetatable(ProgramPreferences.Limits,metalimit1)
    for k in pairs(ProgramPreferences.Limits) do -- k is parameter name, v is table under name
      setmetatable(ProgramPreferences.Limits[k],metalimit2)
    end
  end
end

--Profiles.lua
local function UseDefaultsProfiles()
  ProgramPreferences.Profiles = {}
  for k in pairs(ProfileTypes) do
    ProgramPreferences.Profiles[k] = ''
  end
end

return {
  UseDefaultsLimits = UseDefaultsLimits,
  UseDefaultsProfiles = UseDefaultsProfiles,
  LoadedLimits = LoadedLimits,
}
