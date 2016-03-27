--[[ 

Preferences.lua
Manages application preferences.

ProgramPreferences.Limits table. Key = parameter short name. 
        Value = {param = parameter short name, label = friendly name, 
        order = rank order, [maxvalue] = table {[1]=low,[2]=high}}
ProgramPreferences.Limits metatables. undefined Value will set key of shortname =
        {param=parameter short name, label = friendly name, order = rank order}
ProgramPreferences.Limits metatables. undefined maxvalue will set 
         maxvalue={[1]=low,[2]=high} but will return nil if not in develop
Preferences.Save function. Save to LR storage.
ProgramPreferences.Preferences table. Key = subtable name (e.g., 'Limits', 'Presets').
ProgramPreferences.Presets table. 
Preferences.Reset function. Sets Preferences.Preferences to default settings.
Preferences.Load function. Load from LR storage.
 
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
local Init                = require 'Init'
local LrDialogs           = import 'LrDialogs'
local prefs               = import 'LrPrefs'.prefsForPlugin() 
local serpent             = require 'serpent'
-- hidden
local version = 1

-- public
-- preferences table
ProgramPreferences = {}

local function UseDefaults()
  ProgramPreferences = {}
  Init.UseDefaultsKeys()
  Init.UseDefaultsLimits()
  Init.UseDefaultsPaste()
  Init.UseDefaultsPresets()
  Init.UseDefaultsProfiles()
end

local function Save(ClearOld) --clear old optional parameter
  if ClearOld then
    for k in prefs:pairs() do
      prefs[k] = nil
    end
  end
  prefs[version] = serpent.dump(ProgramPreferences)
end

local function load0() --load version 0 --still need to test paste selective settings -- also test change in starthi startlo tests
  local loaded = false
  UseDefaults()
  --then load whatever is available from version 0
  for k,v in prefs:pairs() do
    if type(k)=='string' then
      local historic = {Temperature = 50000, Tint = 150, Exposure = 5}
      local startlo = k:find('Low',1,true)
      local starthi = k:find('High',1,true)
      if startlo == k:len()-2 then --start starthi--first attempt to understand index string
        local prefname = k:sub(1,startlo-1)
        if type(v) == 'number' and historic[prefname] then --dealing with older version of preferences that doesn't include rangemax
          ProgramPreferences.Limits[prefname][historic[prefname]][1] = v --low limit added
          loaded = true
        elseif type(v) == 'table' then--newer style
          for i,p in pairs(v) do -- pull out low for each rangemax, i=rangemax p = limit
            ProgramPreferences.Limits[prefname][i][1] = p
            loaded = true
          end
        end
      elseif starthi == k:len()-3 then --end startlo, start starthi
        local prefname = k:sub(1,starthi-1)
        if type(v) == 'number' and historic[prefname] then --dealing with older version of preferences that doesn't include rangemax
          ProgramPreferences.Limits[prefname][historic[prefname]][2] = v --high limit added
          loaded = true
        elseif type(v) == 'table' then--newer style
          for i,p in pairs(v) do -- pull out high for each rangemax, i=rangemax p = limit
            ProgramPreferences.Limits[prefname][i][2] = p
            loaded = true
          end
        end
      elseif k == 'Presets' then --end starthi, start presets
        loaded = true
        for i,j in pairs(v) do
          ProgramPreferences.Presets[i] = j --avoid assigning tables to make a true copy
        end
      elseif k == 'PasteList' then --end Presets, start Pastelist
        loaded = true
        for i,j in pairs(v) do
          ProgramPreferences.PasteList[i] = j
        end
      end -- end pastelist
    end  --if string -- all processing occurs inside here--anything added would be an elseif after presets
  end --for k,v in prefs
  if not loaded then
    UseDefaults()
    Save()
    LrDialogs.message(LOC("$$$/MIDI2LR/Preferences/cantload=Unable to load preferences. Using default settings."))
  end
  return loaded
end

local function Load()
  local loaded = false
  if type(prefs)=='table' then
    if type(prefs[version])=='string' then
      loaded,ProgramPreferences = serpent.load(prefs[version])
      if loaded ~= true then
        UseDefaults()
        LrDialogs.message(LOC("$$$/MIDI2LR/Preferences/cantload=Unable to load preferences. Using default settings."))
      end
    else -- not current version
      return load0() --load prior version, proper tail call
    end
  else
    UseDefaults()
  end
  return loaded
end

local function LoadShell() --encapsulates all loading, allowing post-processing
  local loaded = Load()
  if loaded then
    Init.LoadedKeys()
    Init.LoadedLimits()
    Init.LoadedPaste()
    Init.LoadedPresets()
    Init.LoadedProfiles()
    Save()
  end
end



return { --commented out unused exports
  Load = LoadShell,
  Save = Save,
}