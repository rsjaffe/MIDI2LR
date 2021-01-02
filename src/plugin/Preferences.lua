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

local Init                = require 'Init'
local serpent             = require 'serpent'
local LrDialogs           = import 'LrDialogs'
local LrFileUtils         = import 'LrFileUtils'
local prefs               = import 'LrPrefs'.prefsForPlugin() 

-- hidden
local version = 1

-- public
-- preferences table
ProgramPreferences = {}

local function UseDefaults()
  ProgramPreferences = {}
  Init.UseDefaultsAll()
end

local function Save(filename)
  local argtype = type(filename)
  if argtype == 'string' then
    local savestring = {}
    savestring[version] = ProgramPreferences
    local f = assert(io.open(filename,"w"),LOC("$$$/AgImageIO/Errors/WriteFile=The file could not be written.")..' '..filename)
    f:write(serpent.dump(savestring))
    f:close()
  elseif argtype ~= 'nil' then
    LrDialogs.message(LOC("$$$/AgNetIO/Exceptions/BAD_PARAMETERS=The entered parameters are invalid"))
  else
    prefs[version] = serpent.dump(ProgramPreferences)
  end
end

local function Load()
  local loaded = false
  if type(prefs)=='table' and type(prefs[version])=='string' then
    loaded,ProgramPreferences = serpent.load(prefs[version])
  end
  if loaded ~= true then
    UseDefaults()
    LrDialogs.message(LOC("$$$/MIDI2LR/Preferences/cantload=Unable to load preferences. Using default settings."))
  end
  return loaded
end

local function LoadFile(filename)
  local itemsread = LrFileUtils.readFile(filename)
  local loaded,tempload = serpent.load(itemsread)
  if loaded and type(tempload[version]) == 'table' then
    ProgramPreferences = tempload[version]
  else
    loaded = false
    LrDialogs.message(LOC("$$$/AgImageIO/Errors/ReadFile=The file could not be read."))
  end
  return loaded
end

local function LoadShell(filename) --encapsulates all loading, allowing post-processing
  local loaded = false
  local argtype = type(filename)
  if argtype == 'string' then
    loaded = LoadFile(filename)
  elseif argtype ~= 'nil' then
    LrDialogs.message(LOC("$$$/AgNetIO/Exceptions/BAD_PARAMETERS=The entered parameters are invalid"))    
  else
    loaded = Load()
  end
  if loaded then
    Init.LoadedAll()
    Save()
  end
end

return { --commented out unused exports
  Load = LoadShell,
  Save = Save,
}