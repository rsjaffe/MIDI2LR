--[[----------------------------------------------------------------------------

Filters.lua

Manages catalog filters for plugin
 
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

local LrApplication = import 'LrApplication'
local LrView        = import 'LrView'
local numfilters = 12

local function StartDialog(obstable,f)
  for i = 1,numfilters do
    obstable['filter'..i] = {}
    obstable['filter'..i][1] = ProgramPreferences.Filters[i]
  end
  local allfilters = LrApplication.viewFilterPresets()
  local filtersbyuuid = {}
  local filterlist = {}
  for t,u in pairs(allfilters) do
    filterlist[#filterlist+1] = {title = t, value = u}
    filtersbyuuid[u] = t
  end 
  local groupboxfilters = {spacing = f:control_spacing(),
    f:spacer {
      height = f:control_spacing() * 2,
    }, -- spacer
  }
  for i=1,numfilters do
    groupboxfilters[#groupboxfilters+1] = f:push_button {
      fill_horizontal = 1,
      width_in_chars = 40,
      truncation = 'head',
      action = function() obstable['filter'..i] = nil end,        
      title = LrView.bind { key = 'filter'..i,
        transform = function(value) return i..' '..filtersbyuuid[value[1]] end
      },  -- title
    } -- push_button
  end
  local tabviewitems = {} 
  local filterrows, filtercolumns = 4,3 -- x*y must equal local numfilters, above
  for column=1, filtercolumns do
    tabviewitems[column] = f:tab_view_item {
      title = ((column-1)*filterrows+1)..'-'..(column*filterrows), identifier = 'filters-'..((column-1)*filterrows+1)..'-'..(column*filterrows),
    }
    for row=1, filterrows do
      table.insert(tabviewitems[column],f:simple_list {items = filterlist, allows_multiple_selection = false, value = LrView.bind ('filter'..((column-1)*filterrows+row)) }) 
    end
    tabviewitems[column] = f:tab_view_item (tabviewitems[column]) -- prepare for use in f:tabview below
  end 
  return 
  f:row {
    f:column {
      spacing = f:control_spacing(),
      f:tab_view (tabviewitems), -- tab_view
    }, -- column
    f:column(groupboxfilters), -- column
  } -- row
end

local function EndDialog(obstable, status)
  if status == 'ok' then
    ProgramPreferences.Filters = {} -- empty out prior settings
    for i = 1,numfilters do
      if type(obstable['filter'..i])=='table' then -- simple_list should return a table
        ProgramPreferences.Filters[i] = obstable['filter'..i][1]
      end
    end
  end
end

return {
  StartDialog = StartDialog,
  EndDialog = EndDialog,
}
