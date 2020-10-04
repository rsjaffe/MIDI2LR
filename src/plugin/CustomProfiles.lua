--[[----------------------------------------------------------------------------

CustomProfiles.lua

Manages custom camera profiles for plugin
 
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

local LrView              = import 'LrView'

local numprofiles = 20

local function StartDialog(obstable,f)
  for i = 1,numprofiles do
    obstable['CustomProfile'..i] = ProgramPreferences.CustomProfile[i]
  end
  local dlgrows = {}
  for i=1, numprofiles do
    dlgrows[i] = f:row{
      bind_to_object = obstable, -- default bound table
      f:static_text{title = i,  width = LrView.share('cp_label')},
      f:edit_field{
        value = LrView.bind('CustomProfile'..i),
        height_in_lines = 1,
        width_in_chars = 80,
      }
    }
  end 
  return 
  f:column(dlgrows)
end

local function EndDialog(obstable, status)
  if status == 'ok' then
    ProgramPreferences.CustomProfile = {} -- empty out prior settings
    for i = 1,numprofiles do
      ProgramPreferences.CustomProfile[i] = obstable['CustomProfile'..i]
    end
  end
end

return {
  StartDialog = StartDialog,
  EndDialog = EndDialog,
}
