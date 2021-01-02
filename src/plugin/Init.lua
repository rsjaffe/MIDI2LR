--[[----------------------------------------------------------------------------

Init.lua

Contains routines from modules that must be run before the full module
can be included. Because Lua requires that the entire module be
'compiled' when it is included, phased initialization requires separation
of some of the init routines from the body. This holds init routines for
several modules and is required by the Preferences module, which handles 
initialization.

Each type needs to have a Loaded and UseDefaults function.
 
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

local Database = require 'Database'
local ProfileTypes        = require 'ProfileTypes'

--ActionSeries.lua
local function UseDefaultsActionSeries()
  ProgramPreferences.ActionSeries = {}
end
local function LoadedActionSeries()
  if type(ProgramPreferences.ActionSeries) ~= 'table' then
    ProgramPreferences.ActionSeries = {}
  end
end

--Filters.lua
local function UseDefaultsFilters()
  ProgramPreferences.Filters = {}
end
local function LoadedFilters()
  if type(ProgramPreferences.Filters) ~= 'table' then
    UseDefaultsFilters()
  end
end

--Keywords.lua
local function UseDefaultsKeywords()
  ProgramPreferences.Keywords = {}
end
local function LoadedKeywords()
  if type(ProgramPreferences.Keywords) ~= 'table' then
    UseDefaultsKeywords()
  end
end

--Limits.lua
local function UseDefaultsLimits()
  ProgramPreferences.Limits = {}
  ProgramPreferences.Limits.Temperature = {param = 'Temperature', label = Database.CmdTrans.Temperature[Database.LatestPVSupported], 
    [50000] = {3000,9000}}
end

local function LoadedLimits()
  if ProgramPreferences.Limits == nil or type(ProgramPreferences.Limits)~='table' then
    UseDefaultsLimits()
  else --following is just in cases we loaded historic limits, which may have a missing value--no harm in checking carefully
    for t,v in pairs(ProgramPreferences.Limits) do--for each Limit type _ (e.g., Temperature) look at v(table) (highlimits)
      local corrupt = false
      for kk,vv in pairs(v) do -- for each highlimittable kk, look at vv(limit values table) 
        if type(kk)=='number' and (vv[1]==nil or vv[2]==nil) then -- table for each parameter has limits and other data (param,label.order)--ignore other data
          corrupt = true
        end
      end
      if corrupt == true then
        ProgramPreferences.Limits[t] = nil
        ProgramPreferences.Limits.Temperature = {param = 'Temperature', label = Database.CmdTrans.Temperature[Database.LatestPVSupported], 
          [50000] = {3000,9000}}
      end
    end
  end
end

--LocalPresets.lua
local function UseDefaultsLocalPresets()
  ProgramPreferences.LocalPresets = {}
end

local function LoadedLocalPresets()
  if type(ProgramPreferences.LocalPresets) ~= 'table' then
    UseDefaultsLocalPresets()
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

local function LoadedShowActions()
  if ProgramPreferences.RevealAdjustedControls == nil then
    ProgramPreferences.RevealAdjustedControls = true
  end
  if ProgramPreferences.ProfilesShowBezelOnChange == nil then
    ProgramPreferences.ProfilesShowBezelOnChange = true
  end
  if ProgramPreferences.ClientShowBezelOnChange == nil then
    ProgramPreferences.ClientShowBezelOnChange = true
  end
end

local function UseDefaultsShowActions()
  ProgramPreferences.RevealAdjustedControls = true
  ProgramPreferences.ProfilesShowBezelOnChange = true
  ProgramPreferences.ClientShowBezelOnChange = true
end

local function LoadedAll()
  LoadedActionSeries()
  LoadedFilters()
  LoadedKeys()
  LoadedKeywords()
  LoadedLimits()
  LoadedLocalPresets()
  LoadedPaste()
  LoadedPresets()
  LoadedProfiles()
  LoadedShowActions()
end

local function UseDefaultsAll()
  UseDefaultsActionSeries()
  UseDefaultsFilters()
  UseDefaultsKeys()
  UseDefaultsKeywords()
  UseDefaultsLimits()
  UseDefaultsLocalPresets()
  UseDefaultsPaste()
  UseDefaultsPresets()
  UseDefaultsProfiles()
  UseDefaultsShowActions()
end

return {
  LoadedActionSeries  = LoadedActionSeries,
  LoadedFilters       = LoadedFilters,
  LoadedKeys          = LoadedKeys,
  LoadedKeywords      = LoadedKeywords,
  LoadedLimits        = LoadedLimits,
  LoadedLocalPresets  = LoadedLocalPresets,
  LoadedPaste         = LoadedPaste,
  LoadedPresets       = LoadedPresets,
  LoadedProfiles      = LoadedProfiles,
  LoadedShowActions   = LoadedShowActions,
  UseDefaultsActionSeries = UseDefaultsActionSeries,
  UseDefaultsFilters  = UseDefaultsFilters,
  UseDefaultsKeys     = UseDefaultsKeys,
  UseDefaultsKeywords = UseDefaultsKeywords,
  UseDefaultsLimits   = UseDefaultsLimits,
  UseDefaultsLocalPresets = UseDefaultsLocalPresets,
  UseDefaultsPaste    = UseDefaultsPaste,
  UseDefaultsPresets  = UseDefaultsPresets,
  UseDefaultsProfiles = UseDefaultsProfiles,
  UseDefaultsShowActions = UseDefaultsShowActions,
  LoadedAll           = LoadedAll,
  UseDefaultsAll      = UseDefaultsAll,
}
