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
local LrFileUtils    = import 'LrFileUtils'
local LrLocalization = import 'LrLocalization'
local LrPathUtils    = import 'LrPathUtils'
local LrSystemInfo   = import 'LrSystemInfo'

local function writeDebug()
  local testfile = LrPathUtils.child(_PLUGIN.path,'Client.lua')
  local writeable = ''
  if not LrFileUtils.isWritable(testfile) then
    writeable = '\nError: plugin directory is not writeable'
  end

  local mess = '----------- \LIGHTROOM -----------'
  .. '\nOperating system ' .. LrSystemInfo.summaryString()
  .. '\nVersion ' .. LrApplication.versionString()
  .. '\nLanguage ' .. LrLocalization.currentLanguage() 
  .. '\nPreferences path ' .. LrPathUtils.getStandardFilePath ('appPrefs') 
  .. '\nLApplication data path ' .. LrPathUtils.getStandardFilePath ('appData')
  .. '\n----------- PLUGIN -----------' 
  .. '\nVersion ' .. Info.VERSION.major  .. '.' .. Info.VERSION.minor .. '.' .. Info.VERSION.revision .. '.' .. Info.VERSION.build 
  .. '\nPath ' .._PLUGIN.path .. writeable
  if (type(Info.AppInfo) == 'table') then
    mess = mess .. '\n----------- APP -----------'
    for _,v in ipairs(Info.AppInfo) do
      mess = mess .. '\n' .. v
    end
  end
  local datafile = LrPathUtils.child(LrPathUtils.parent(_PLUGIN.path), 'MIDI2LRinfo.txt')
  local file = assert(io.open(datafile,'w'),'Error writing to ' .. datafile)
  file:write(mess)
  file:close()
end

return {
  write = writeDebug,
}