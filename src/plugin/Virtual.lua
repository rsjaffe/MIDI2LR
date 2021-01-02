--[[----------------------------------------------------------------------------

Virtual.lua

Defines virtual commands.
 
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

local LrDialogs           = import 'LrDialogs'
local LrStringUtils       = import 'LrStringUtils'
local CU                  = require 'ClientUtilities'
local Database            = require 'Database'

local SaturationAdjustments = {
  "SaturationAdjustmentRed",
  "SaturationAdjustmentOrange",
  "SaturationAdjustmentYellow",
  "SaturationAdjustmentGreen",
  "SaturationAdjustmentAqua",
  "SaturationAdjustmentBlue",
  "SaturationAdjustmentPurple",
  "SaturationAdjustmentMagenta"
}

return {
  AllSaturationAdjustment = function(value, UpdateParam)
    local midi_value = tonumber(value)
    for _, param in ipairs(SaturationAdjustments) do
      UpdateParam(param, midi_value, true)
    end
    if ProgramPreferences.ClientShowBezelOnChange then
      local display_value = CU.MIDIValueToLRValue("SaturationAdjustmentRed", midi_value)
      local bezelname = (Database.CmdTrans.AllSaturationAdjustment and Database.CmdTrans.AllSaturationAdjustment[Database.LatestPVSupported]) or "AllSaturationAdjustment"
      LrDialogs.showBezel(bezelname..'  '..LrStringUtils.numberToStringWithSeparators(display_value, 0))
    end
    return "AllSaturationAdjustment"
  end,
}
