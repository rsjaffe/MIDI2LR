--[[----------------------------------------------------------------------------

Documentation.lua

Takes Database.lua and produces text lists and other tools for documentation
and updating. Has to be run under Lightroom to be properly translated,
but is not used by users of the plugin. Running this also forces a refresh
of the ParamList and MenuList files.
 
This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>. 
------------------------------------------------------------------------------]]

local Database = require 'Database'
local LrPathUtils = import 'LrPathUtils'       
local datafile = LrPathUtils.child(_PLUGIN.path, 'Documentation.txt')
local file = io.open(datafile,'w')
local menulocation = ""

Database.RunTests()

file:write("Control definitions for wiki\n")
for _,v in ipairs(Database.DataBase) do
  if v[4] then
        if v[9] ~= menulocation then
      menulocation = v[9]
      file:write("New menu "..menulocation.."\n")
    end
    local experimental = ""
    if v[7]  then 
      experimental = "\226\128\187"
    end
    file:write("| "..v[8]..experimental.." | "..v[10].." |\n" )
  end
end
file:write("\n\nApplication menu entries\n")

for _,v in ipairs(Database.DataBase) do
  if v[4] then
    if v[9] ~= menulocation then
      menulocation = v[9]
      file:write("New menu "..menulocation.."\n")
    end
    file:write('"'..v[8]..'",\n')
  end
end
file:write("\n\nCommand strings\n")
menulocation = ""
for _,v in ipairs(Database.DataBase) do
  if v[4] then
    if v[9] ~= menulocation then
      menulocation = v[9]
      file:write("/* "..menulocation.." */\n")
    end
    file:write('"'..v[1]..'",\n')
  end
end

file:close()


