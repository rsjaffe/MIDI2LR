--------------------------------------------------------------------------------
-- Provides utility methods for MIDI2LR.
-- @module Utilities
-- @license GNU GPLv3
--------------------------------------------------------------------------------

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
--All variables and functions defined here must be local, except for return table.
--The only global identifiers used in this module are 'import'.
--In ZeroBrane IDE, check for global identifiers by pressing shift-F7 while 
--viewing this file.

--imports
local LrApplication = import 'LrApplication'
local LrPathUtils   = import 'LrPathUtils'
local LrFileUtils   = import 'LrFileUtils'

local function LrVersion74orMore()
  local vers = LrApplication.versionTable()
  if vers.major < 7 then return false end
  if vers.major == 7 and vers.minor < 4 then return false end
  return true
end

LrVersion74orMore = LrVersion74orMore()

local function LrVersion66orMore()
    local vers = LrApplication.versionTable()
  if vers.major < 6 then return false end
  if vers.major == 6 and vers.minor < 6 then return false end
  return true
end

LrVersion66orMore = LrVersion66orMore()

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
  LrFileUtils.createAllDirectories( ret )
  return ret
end


--- @export
return { --table of exports, setting table member name and module function it points to
  appdatapath = appdatapath,
  applogpath = applogpath,
  LrVersion74orMore = LrVersion74orMore,
  LrVersion66orMore = LrVersion66orMore,
}
