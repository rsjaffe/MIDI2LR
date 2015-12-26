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
local Ut                  = require 'Utilities'
local prefs               = import 'LrPrefs'.prefsForPlugin() 
local serpent             = require 'serpent'

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
    t[k] = setmetatable({param = k,},metalimit2)
    changed = true
    return t[k]
  end,
}

local Limits, Preferences = {},{}

local function useDefaults()
  Limits = setmetatable({},metalimit1)
  Preferences.Limits = Limits
  Limits['Temperature'][50000] = {3000,9000}
end


-- public

local function Save()
  if not changed then return end
  prefs[version] = serpent.dump(Preferences)
end

local function load0() --load version 0
  useDefaults()
  --then load whatever is available from version 0
end

local function Load()
  if type(prefs)=='table' then
    if type(prefs[1])=='string' then
      local ok
      ok,Preferences = serpent.load(prefs[1])
      if not ok then
        useDefaults()
        LrDialogs.message(LOC("$$$/MIDI2LR/Preferences/cantload=Unable to load preferences. Using default settings."))
      end
    else -- not current version
      load0() --load prior version
    end
  else
    useDefaults()
  end
end


return {
  Limits = Limits,
  Load = Load,
  Preferences = Preferences,
  Save = Save,
}


