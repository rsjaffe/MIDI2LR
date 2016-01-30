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
local LrApplication       = import 'LrApplication'
local LrApplicationView   = import 'LrApplicationView'
local LrDevelopController = import 'LrDevelopController'
local LrDialogs           = import 'LrDialogs'
local ParamList           = require 'ParamList'
local prefs               = import 'LrPrefs'.prefsForPlugin() 
local Profiles            = require 'Profiles'
local serpent             = require 'serpent'
-- hidden
local changed = false
local version = 1

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
        changed = true
      end
    end
    return t[k]
  end,
}

local metalimit1 = {
  __index = function(t,k)--key is the name of the parameter
    t[k] = setmetatable({param = k,label = ParamList.LimitEligible[k][1], order = ParamList.LimitEligible[k][2]},metalimit2) 
    changed = true
    return t[k]
  end,
}
-- public
-- preferences table
ProgramPreferences = {}

local function useDefaults()
  ProgramPreferences = {}
  ProgramPreferences = {Limits = setmetatable({},metalimit1), Presets = {}, PasteList = {}, Profiles = {}, }
  ProgramPreferences.Limits['Temperature'][50000] = {3000,9000}
  Profiles.useDefaults()
  changed = true
end

local function Save(ClearOld) --clear old optional parameter
  -- Limits.DiscardExcess() -- call for each 'class' currently only Limits
  if ClearOld then
    for k in prefs:pairs() do
      prefs[k] = nil
    end
  end
  prefs[version] = serpent.dump(ProgramPreferences)
  changed = false
end

local function load0() --load version 0 --still need to test paste selective settings -- also test change in starthi startlo tests
  local loaded = false
  useDefaults()
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
  if loaded then -- final cleanup after load
    for _,v in pairs(ProgramPreferences.Limits) do--for each Limit type _ (e.g., Temperature) look at v(table) (highlimits)
      for kk,vv in pairs(v) do -- for each highlimittable kk, look at vv(limit values table) 
        if type(kk)=='number' and (vv[1]==nil or vv[2]==nil) then -- table for each parameter has limits and other data (param,label.order)--ignore other data
          vv[1],vv[2]=nil,nil --guard against corrupted data when only one bound gets initialized
        end
      end
    end
    Save()--in new format
  else --failed to load
    useDefaults()
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
        useDefaults()
        LrDialogs.message(LOC("$$$/MIDI2LR/Preferences/cantload=Unable to load preferences. Using default settings."))
      else --need to add back in the metatables. serpent doesn't serialize metatables
        setmetatable(ProgramPreferences.Limits,metalimit1)
        for k in pairs(ProgramPreferences.Limits) do -- k is parameter name, v is table under name
          setmetatable(ProgramPreferences.Limits[k],metalimit2)
        end
      end
    else -- not current version
      return load0() --load prior version, proper tail call
    end
  else
    useDefaults()
  end
  return loaded
end

local function ClearAll()
  for k in prefs:pairs() do
    prefs[k] = nil
  end
  useDefaults()
  Save()
end

return { --commented out unused exports
--  ClearAll = ClearAll,
  Load = Load,
--  Reset = useDefaults,
  Save = Save,
}