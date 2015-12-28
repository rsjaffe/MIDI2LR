--[[ put preferences module here

single table
subtable limits, which has a metatable that adds a parameter to the table when __index. one value in the parameter is {param = NameOfParameter} added by __index function
that parameter has a metatable that adds range-specific limit pair __index. __index uses self.param to identify parameter name

load for current version will call load for prior version if current version not there and table in place. last load (load0)
will do a blank initialization of preferences if none is found.
load will do blank initialization if prefs is not a table.

--]]
local LrDevelopController = import 'LrDevelopController'
local LrDialogs           = import 'LrDialogs'
local Parameters          = require 'Parameters'
local prefs               = import 'LrPrefs'.prefsForPlugin() 
local serpent             = require 'serpent'
local Ut                  = require 'Utilities'

-- hidden

local changed = false
local version = 1

local metalimit3 = {
}

local metalimit2 = {
  __index = function(t,k) -- key is high value for the range
    local lo,hi = Ut.execFCM(LrDevelopController.getRange,t.param)
    if k ~= hi then
      return nil
    end
    t[k] = setmetatable({lo,hi},metalimit3)
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
      if startlo then
        local prefname = v:sub(1,startlo-1)
        if type(v) == 'number' and historic[prefname] then --dealing with older version of preferences that doesn't include rangemax
          Preferences.Limits[prefname][historic[prefname]][1] = v --low limit added
          loaded = true
        elseif type(v) == table --newer style
        for i,p in v do -- pull out low for each rangemax, i=rangemax p = limit
          Preferences.Limits[prefname][i] = p
          loaded = true
        end
      elseif starthi then
        local prefname = v:sub(1,starthi-1)
        if type(v) == 'number' and historic[prefname] then --dealing with older version of preferences that doesn't include rangemax
          Preferences.Limits[prefname][historic[prefname]][2] = v --high limit added
          loaded = true
        elseif type(v) == table --newer style
        for i,p in v do -- pull out low for each rangemax, i=rangemax p = limit
          Preferences.Limits[prefname][2] = p
          loaded = true
        end
      end --processing for control limits
      if k == 'Presets' then
        loaded = true
        for i,p in pairs(k) do
          Preferences.Presets[i] = [k] --avoid assigning tables to make a true copy
        end
      end --processing preses
    end  --if string
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


