--[[ put preferences module here

single table
subtable limits, which has a metatable that adds a parameter to the table when __index. one value in the parameter is {param = NameOfParameter} added by __index function
that parameter has a metatable that adds range-specific limit pair __index. __index uses self.param to identify parameter name

















--]]
LrDevelopController = import 'LrDevelopController'
local prefs = import 'LrPrefs'.prefsForPlugin() 
local Ut = require 'Utilities'
local serpent = require 'serpent'

local changed = false
local version = 1

local metachange = {
  __newindex = function (t,k,v)
    t[k] = v
    changed = true
  end,
}

local metalimit2 = {
  __index = function(tablename,key) -- key is high value for the range
    tablename[key] = setmetatable({Ut.execFCM(LrDevelopController.getRange,tablename.param)},metachange)
  end,
  __newindex = function (t,k,v)
    t[k] = v
    changed = true
  end,
}

local metalimit1 = {
  __index = function(tablename,key)--key is the name of the parameter
    tablename[key] = setmetatable({param = key,},metalimit2)
  end,
  __newindex = function (t,k,v)
    t[k] = v
    changed = true
  end,
}

local Preferences = {}

Preferences.Limits = setmetatable({},metalimit1)

local function save()
  if not changed then return end
  prefs[version] = serpent.dump(Preferences)
end

local function load()
  if type(prefs)=='table' and type(prefs[1])=='string' then
    local ok
    ok,Preferences = serpent.load(prefs[1])
    if not ok then
      --error ?load defaults? and save/overwrite?
    end
  else
    --need to load defaults
  end
end


return {
  save = save,
  load = load,
}


