--[[
Utilities.lua
Provides utility methods for MIDI2LR

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>. 
--]]

local LrPathUtils   = import 'LrPathUtils'
local LrFileUtils   = import 'LrFileUtils'

local function applogpath()
  local ret
  if WIN_ENV then
    ret=LrPathUtils.child(LrPathUtils.parent(LrPathUtils.parent(LrPathUtils.getStandardFilePath('appData'))),'MIDI2LR')
  else
    ret='~/Library/Logs/MIDI2LR'
  end
  ret = LrPathUtils.standardizePath(ret)
  LrFileUtils.createAllDirectories(ret) 
  return ret
end

local function appdatapath()
  local ret
  if WIN_ENV then
    ret=LrPathUtils.child(LrPathUtils.parent(LrPathUtils.parent(LrPathUtils.getStandardFilePath('appData'))),'MIDI2LR')
  else
    ret='~/Library/Application Support/MIDI2LR'
  end
  ret = LrPathUtils.standardizePath(ret)
  LrFileUtils.createAllDirectories(ret)
  return ret
end

return {
  appdatapath = appdatapath,
  applogpath = applogpath,
}
