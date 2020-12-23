--[[ 

DebugInfo.lua
Saves debug info to a file
 
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

local Info           = require 'Info'
local Ut             = require 'Utilities'
local LrApplication  = import 'LrApplication'
local LrFileUtils    = import 'LrFileUtils'
local LrLocalization = import 'LrLocalization'
local LrPathUtils    = import 'LrPathUtils'
local LrSystemInfo   = import 'LrSystemInfo'

local testfile = LrPathUtils.child(_PLUGIN.path,'Client.lua')
local writeable = ''
if not LrFileUtils.isWritable(testfile) then
  writeable = '\nPlugin directory is not writeable'
end

local function gatherInformation()
  local longest = 40
  local mess = '----------- LIGHTROOM -----------'
  ..'\nOperating system '..LrSystemInfo.summaryString()
  ..'\nVersion '..LrApplication.versionString()
  ..'\nLanguage '..LrLocalization.currentLanguage() 
  ..'\nPreferences path '..LrPathUtils.getStandardFilePath ('appPrefs') 
  ..'\nApplication data path '..LrPathUtils.getStandardFilePath ('appData')
  ..'\n----------- PLUGIN -----------' 
  ..'\nVersion '..Info.VERSION.major..'.'..Info.VERSION.minor..'.'..Info.VERSION.revision..'.'..Info.VERSION.build 
  ..'\nPath '.._PLUGIN.path..writeable
  ..'\nMIDI2LR log path '..Ut.applogpath()
  ..'\nMIDI2LR data path '..Ut.appdatapath()
  local lines = 11 -- update if change above message
  if type(Info.AppInfo) == 'table' then
    mess = mess..'\n----------- APP -----------'
    lines = lines + 1
    for _,v in ipairs(Info.AppInfo) do
      longest = math.max(longest, v:len())
      lines = lines + 1
      mess = mess..'\n'..v
    end
  end
  return mess, lines, longest
end

local function writeDebug()
  local datafile = LrPathUtils.child(Ut.applogpath(), 'MIDI2LRinfo.txt')
  local file = assert(io.open(datafile,'w'),LOC("$$$/AgImageIO/Errors/WriteFile=The file could not be written.")..' '..datafile)
  file:write(gatherInformation())
  file:close()
end

local function sendLog()
  MIDI2LR.SERVER:send('Log Operating system '..LrSystemInfo.summaryString()..'\n')
  MIDI2LR.SERVER:send('Log Lightroom Version '..LrApplication.versionString()..'\n')
  MIDI2LR.SERVER:send('Log Lightroom Language '..LrLocalization.currentLanguage()..'\n')
  MIDI2LR.SERVER:send('Log Lightroom Preferences path '..LrPathUtils.getStandardFilePath ('appPrefs')..'\n')
  MIDI2LR.SERVER:send('Log Lightroom Application data path '..LrPathUtils.getStandardFilePath ('appData')..'\n')
  MIDI2LR.SERVER:send('Log Plugin version '..Info.VERSION.major..'.'..Info.VERSION.minor..'.'..Info.VERSION.revision..'.'..Info.VERSION.build..'\n')
  MIDI2LR.SERVER:send('Log Plugin path '.._PLUGIN.path..writeable..'\n')
  MIDI2LR.SERVER:send('Log Plugin log path '..Ut.applogpath()..'\n')
  MIDI2LR.SERVER:send('Log Plugin data path '..Ut.appdatapath()..'\n')
end

return {
  gatherInformation = gatherInformation,
  sendLog = sendLog,
  write = writeDebug,
}