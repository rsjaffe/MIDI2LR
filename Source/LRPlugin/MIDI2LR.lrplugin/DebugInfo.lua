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
  .. LrSystemInfo.summaryString() .. '\nPlugin version ' .. Info.VERSION.major 
  .. '.' .. Info.VERSION.minor .. '.' .. Info.VERSION.revision .. '.' .. Info.VERSION.build 
  if Info.AppVersion then
    mess = mess .. '\nApp version ' .. Info.AppVersion
  end
  mess = mess .. '\nLightroom language ' .. LrLocalization.currentLanguage() 
  if Info.AppLocale then
    mess = mess .. '\nSystem language ' .. Info.AppLocale
  end
  mess = mess .. '\nPlugin path ' .._PLUGIN.path 
  if Info.AppPath then
    mess = mess .. '\nApp path ' .. Info.AppPath
  end
  mess = mess .. '\nPreferences path ' .. LrPathUtils.getStandardFilePath ('appPrefs') 
  .. '\nApplication data path ' .. LrPathUtils.getStandardFilePath ('appData')


  local datafile = LrPathUtils.child(_PLUGIN.path, 'debug.txt')
  local file = assert(io.open(datafile,'w'),'Error writing to ' .. datafile)
  file:write(mess)
  file:close()
end

return {
  write = writeDebug,
}