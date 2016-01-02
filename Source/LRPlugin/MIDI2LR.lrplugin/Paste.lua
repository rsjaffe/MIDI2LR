--[[----------------------------------------------------------------------------

Paste.lua

Manages paste settings for plugin
 
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
local Parameters = require 'Parameters'
local LrView = import 'LrView'

local Duplicates = { -- will set each to its duplicate in EndDialog
  Exposure2012 = 'Exposure',
  Highlights2012 = 'Highlights',
  Shadows2012 = 'Shadows',
  Contrast2012 = 'Contrast',
  Whites2012 = 'Whites',
  Blacks2012 = 'Blacks',
  Clarity2012 = 'Clarity',
  ToneCurveName2012 = 'ToneCurveName',
  ToneCurvePV2012 = 'ToneCurve',
}

local function StartDialog(obstable,f)
  for k,v in pairs(ProgramPreferences.PasteList) do
    obstable['Paste'..k] = v 
  end 
  obstable.PastePopup = ProgramPreferences.PastePopup
  local selectivepastecol = {}
  do 
    local numberofcolumns = 4
    local breakpoint = math.floor(#Parameters.Order / numberofcolumns)
    for col = 1,numberofcolumns do
      selectivepastecol[col] = {}
      for i = ((col-1)*breakpoint+1),(breakpoint*col) do
        table.insert(
          selectivepastecol[col], 
          f:checkbox { title = Parameters.Names[i][1], value = LrView.bind ('Paste'..Parameters.Order[i]) } 
        )
      end
      selectivepastecol[col] = f:column (selectivepastecol[col]) -- prepare for use in f:row below
    end -- for col
  end
  return 
  f:row(selectivepastecol),
  f:row{ 
    f:row{
      f:push_button {
        title = LOC("$$$/AgCameraRawNamedSettings/NamedSettingsControls/CheckNone=Check none"),
        action = function ()
          for _,v in ipairs(Parameters.Order) do
            obstable['Paste'..v] = false
          end 
        end,
      }, -- push_button
      f:push_button {
        title = LOC("$$$/AgCameraRawNamedSettings/NamedSettingsControls/CheckAll=Check all"
        ),
        action = function ()
          for _,v in ipairs(Parameters.Order) do
            obstable['Paste'..v] = true
          end 
        end,
      } ,-- push_button 
    },-- set of pushbuttons
    f:checkbox {title = 'Ask each time', value = LrView.bind('PastePopup')}
  } --row with pushbuttons and checkbox
end

local function EndDialog(obstable, status)
  if status == 'ok' then
    --assign PasteList
    ProgramPreferences.PasteList = {} -- empty out prior settings
    for i,v in ipairs(Parameters.Order) do
      ProgramPreferences.PasteList[v] = obstable['Paste'..v]
    end 
    for k,v in pairs(Duplicates) do
      ProgramPreferences.PasteList[k] = ProgramPreferences.PasteList[v]
    end
    ProgramPreferences.PastePopup = obstable.PastePopup
  end
end

return {
  StartDialog = StartDialog,
  EndDialog = EndDialog,
}