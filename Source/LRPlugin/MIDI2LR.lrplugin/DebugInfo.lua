--[[ 

DebugInfo.lua
Shows debug info
 
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
local LrApplication  = import 'LrApplication'
local LrDialogs      = import 'LrDialogs'
local LrLocalization = import 'LrLocalization'
local LrPathUtils    = import 'LrPathUtils'
local LrSystemInfo   = import 'LrSystemInfo'

local function writeDebug()
  local mess = 'Lightroom version ' .. LrApplication.versionString() .. '\n'
  .. LrSystemInfo.summaryString() .. '\nLightroom language ' .. LrLocalization.currentLanguage() 
  .. '\nMIDI2LR version ' .. Info.VERSION.major .. '.' .. Info.VERSION.minor .. '.' .. Info.VERSION.revision .. '.' .. Info.VERSION.build 
  .. '\nPlugin location ' .._PLUGIN.path .. '\nPreferences location ' .. LrPathUtils.getStandardFilePath ('appPrefs') 
  .. '\nApplication data location ' .. LrPathUtils.getStandardFilePath ('appData')
  if Info.AppLocale then
    mess = mess .. '\nSystem locale is ' .. Info.AppLocale
  end
  if Info.AppVersion then
    mess = mess .. '\nApp version is ' .. Info.AppVersion
  end
  if Info.AppPath then
    mess = mess .. '\nApp path is ' .. Info.AppPath
  end
  local datafile = LrPathUtils.child(_PLUGIN.path, 'debug.txt')
  local file = assert(io.open(datafile,'w'),'Error writing to ' .. datafile)
  file:write(mess)
  file:close()
end

return {
  write = writeDebug,
}