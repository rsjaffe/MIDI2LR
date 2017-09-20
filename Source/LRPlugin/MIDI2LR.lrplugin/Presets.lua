--[[----------------------------------------------------------------------------

Presets.lua

Manages develop presets for plugin
 
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
local LrApplication = import 'LrApplication'
local LrView        = import 'LrView'

local function StartDialog(obstable,f)
  --populate table with presets
  for i = 1,40 do
    obstable['preset'..i] = {}
    obstable['preset'..i][1] = ProgramPreferences.Presets[i]
  end
  -- find presets stored in Lightroom and put in table name and UUID
  local psList = {}
  for _,fold in pairs(LrApplication.developPresetFolders()) do
    local foldname = fold:getName()
    for _,pst in pairs(fold:getDevelopPresets()) do
      table.insert(psList,{title = foldname..'\226\134\146'..pst:getName(), value = pst:getUuid()})
    end -- '\226\134\146' is right arrow in utf8
  end
  -- set up presets list for the groupbox on the right of the presets selection dialog
  local groupboxpresets = {}
  for i=1,20 do
    table.insert( 
      groupboxpresets, 
      f:push_button {fill_horizontal = 1,
        width_in_chars = 40,
        truncation = 'head',
        action = function() obstable['preset'..i] = nil end,
        title = LrView.bind { key = 'preset'..i,
          transform = function(value) return i..' '..(LrApplication.developPresetByUuid(value[1]):getName()) end
        },  -- title
      } -- static_text
    )
  end
  local groupboxpresets2 = {}
  for i=21,40 do
    table.insert( 
      groupboxpresets2, 
      f:push_button {fill_horizontal = 1,
        width_in_chars = 40,
        truncation = 'head',
        action = function() obstable['preset'..i] = nil end,
        title = LrView.bind { key = 'preset'..i,
          transform = function(value) return i..' '..(LrApplication.developPresetByUuid(value[1]):getName()) end
        },  -- title
      } -- static_text
    )
  end
  -- set up groups of preset listings
  local tabviewitems = {} 
  local psrows, pscolumns = 4,10
  for column=1, pscolumns do
    tabviewitems[column] = f:tab_view_item {title = ((column-1)*psrows+1)..'-'..(column*psrows), identifier = 'presets-'..((column-1)*psrows+1)..'-'..(column*psrows),}
    for row=1, psrows do
      table.insert(tabviewitems[column],f:simple_list {items = psList, allows_multiple_selection = false, value = LrView.bind ('preset'..((column-1)*psrows+row)) })
    end
    tabviewitems[column] = f:tab_view_item (tabviewitems[column]) -- prepare for use in f:tabview below
  end 
  return 
  f:row {
    f:column {
      spacing = f:control_spacing(),
      f:tab_view (tabviewitems), -- tab_view
    }, -- column
    f:column{ -- for the display of chosen presets
      spacing = f:control_spacing(),
      f:spacer {
        height = f:control_spacing() * 2,
      }, -- spacer
      unpack (groupboxpresets), -- list of 20
    }, -- column
    f:column{ -- for the display of chosen presets
      spacing = f:control_spacing(),
      f:spacer {
        height = f:control_spacing() * 2,
      }, -- spacer
      unpack (groupboxpresets2), -- list of 20
    }, -- column
  } -- row
end

local function EndDialog(obstable, status)
  if status == 'ok' then
    ProgramPreferences.Presets = {} -- empty out prior settings
    for i = 1,40 do
      if type(obstable['preset'..i])=='table' then -- simple_list should return a table
        ProgramPreferences.Presets[i] = obstable['preset'..i][1]
      end
    end
  end
end

return {
  StartDialog = StartDialog,
  EndDialog = EndDialog,
}