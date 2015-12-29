--[[ put preferences module here

Preferences.Limits table. Key = parameter short name. 
        Value = {param = parameter short name, label = friendly name, 
        order = rank order, [maxvalue] = table {[1]=low,[2]=high}}
Preferences.Limits metatables. undefined Value will set key of shortname =
        {param=parameter short name, label = friendly name, order = rank order}
Preferences.Limits metatables. undefined maxvalue will set 
         maxvalue={[1]=low,[2]=high} but will return nil if not in develop
Preferences.Load function. Save to LR storage.
Preferences.Preferences table. Key = subtable name (e.g., 'Limits', 'Presets').
Preferences.Presets table. 
Preferences.Reset function. Sets Preferences.Preferences to default settings.
Preferences.Save function. Load from LR storage.

load for current version will call load for prior version if current version not there and table in place. last load (load0)
will do a blank initialization of preferences if none is found.
load will do blank initialization if prefs is not a table.

===========still need to add selective paste prefs to load0

Preferences.lua
Manages application preferences.
 
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
local LrApplication       = import 'LrApplication'
local LrApplicationView   = import 'LrApplicationView'
local LrDevelopController = import 'LrDevelopController'
local LrDialogs           = import 'LrDialogs'
local Parameters          = require 'Parameters'
local prefs               = import 'LrPrefs'.prefsForPlugin() 
local serpent             = require 'serpent'


-- hidden

local changed = false
local version = 1

local metalimit3 = {
}

local metalimit2 = {
  __index = function(t,k) -- key is high value for the range
    if LrApplicationView.getCurrentModuleName() ~= 'develop' or LrApplication.activeCatalog():getTargetPhoto() == nil then return nil,nil end

    local lo,hi = LrDevelopController.getRange(t.param)
    if k ~= hi then
      return nil
    end
    if t.param == 'Temperature' and k == 50000 then
      t[k] = setmetatable({3000,9000},metalimit3) 
    else
      t[k] = setmetatable({lo,hi},metalimit3) 
    end
    changed = true
    return t[k]
  end,
}

local metalimit1 = {
  __index = function(t,k)--key is the name of the parameter
    t[k] = setmetatable({param = k,label = Parameters.Names[k][1], order = Parameters.Names[k][2]},metalimit2) 
    changed = true
    return t[k]
  end,
}
-- public
-- preferences table
local Preferences = {}

local function useDefaults()
  Preferences = {}
  Preferences = {Limits = setmetatable({},metalimit1), Presets = {} }
  Preferences.Limits['Temperature'][50000] = {3000,9000}
end



local function Save()
  if not changed then return end
  prefs[version] = serpent.dump(Preferences)
end

local function load0() --load version 0 --still need to add paste selective settings
  local loaded = false
  useDefaults()
  --then load whatever is available from version 0
  for k,v in pairs(prefs) do
    if type(k)=='string' then
      local historic = {Temperature = 50000, Tint = 150, Exposure = 5}
      local startlo = k:find('Low',1,true)
      local starthi = k:find('High',1,true)
      if startlo then --start starthi--first attempt to understand index string
        local prefname = v:sub(1,startlo-1)
        if type(v) == 'number' and historic[prefname] then --dealing with older version of preferences that doesn't include rangemax
          Preferences.Limits[prefname][historic[prefname]][1] = v --low limit added
          loaded = true
        elseif type(v) == table then--newer style
          for i,p in v do -- pull out low for each rangemax, i=rangemax p = limit
            Preferences.Limits[prefname][i] = p
            loaded = true
          end
        end
      elseif starthi then --end startlo, start starthi
        local prefname = v:sub(1,starthi-1)
        if type(v) == 'number' and historic[prefname] then --dealing with older version of preferences that doesn't include rangemax
          Preferences.Limits[prefname][historic[prefname]][2] = v --high limit added
          loaded = true
        elseif type(v) == table then--newer style
          for i,p in v do -- pull out low for each rangemax, i=rangemax p = limit
            Preferences.Limits[prefname][2] = p
            loaded = true
          end
        end
      elseif k == 'Presets' then --end starthi, start presets
        loaded = true
        for i,p in pairs(k) do
          Preferences.Presets[i] = k --avoid assigning tables to make a true copy
        end
      end --processing presets
    end  --if string -- all processing occurs inside here--anything added would be an elseif after presets
  end --for k,v in prefs
  if loaded == false then
    LrDialogs.message(LOC("$$$/MIDI2LR/Preferences/cantload=Unable to load preferences. Using default settings."))
  end
  return loaded
end

local function Load()
  local loaded = false
  if type(prefs)=='table' then
    if type(prefs[1])=='string' then
      loaded,Preferences = serpent.load(prefs[1])
      if loaded ~= true then
        useDefaults()
        LrDialogs.message(LOC("$$$/MIDI2LR/Preferences/cantload=Unable to load preferences. Using default settings."))
      end
    else -- not current version
      return load0() --load prior version, proper tail call
    end
  else
    useDefaults()
  end
  return loaded
end


return {
  Limits = Preferences.Limits,
  Load = Load,
  Preferences = Preferences,
  Presets = Preferences.Presets,
  Reset = useDefaults,
  Save = Save,
}


