--[[----------------------------------------------------------------------------

ActionSeries.lua

Manages action series for plugin
 
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

local LrView        = import 'LrView'
local Database      = require 'Database'

Database.ValidActions.Pause = true --this is additional to the menu list

local function ValidateActions(_,value)
  local stack = nil 
  for a in value:gmatch("[%w_]+") do
    if not Database.ValidActions[a] then
      if not stack then
        stack = a
      else
        stack = stack .. ', ' .. a
      end
    end
  end
  if stack then
    return false,value,LOC("$$$/MIDI2LR/Options/ActionSeriesInvalid=Invalid commands") .. ': '  .. stack
  else
    return true,value
  end
end

local numseries = 9

local function StartDialog(obstable,f)
  for i = 1,numseries do
    obstable['ActionSeries'..i] = ProgramPreferences.ActionSeries[i]
  end
  local dlgrows = {}
  for i=1, numseries do
    dlgrows[i] = f:row{
      bind_to_object = obstable, -- default bound table
      f:static_text{title = i},
      f:edit_field{
        value = LrView.bind('ActionSeries'..i),
        height_in_lines = 2,
        width_in_chars = 80,
        validate = ValidateActions,
      }
    }
  end 
  return 
  f:column(dlgrows)
end


local function EndDialog(obstable, status)
  if status == 'ok' then
    ProgramPreferences.ActionSeries = {} -- empty out prior settings
    for i = 1,numseries do
      ProgramPreferences.ActionSeries[i] = obstable['ActionSeries'..i]
    end
  end
end



return {
  StartDialog = StartDialog,
  EndDialog = EndDialog,
}
