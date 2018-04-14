--[[----------------------------------------------------------------------------

SystemInfo.lua
Display system information for debugging
 
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
local LrDialogs = import 'LrDialogs'
local LrView    = import 'LrView'
local DebugInfo = require 'DebugInfo'
local answer, lines, length = DebugInfo.gatherInformation()
local f = LrView.osFactory()
LrDialogs.presentModalDialog({resizable = true,
    contents = f:edit_field({
        value = answer,
        height_in_lines = lines,
        width_in_chars = length})
  })
