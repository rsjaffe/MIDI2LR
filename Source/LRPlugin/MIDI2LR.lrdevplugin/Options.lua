--[[----------------------------------------------------------------------------

Options.lua

Manages options for plugin
 
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
local LrBinding = import 'LrBinding'
local LrDialogs = import 'LrDialogs'
local LrFunctionContext = import 'LrFunctionContext'
local prefs = import 'LrPrefs'.prefsForPlugin() 
local LrView = import 'LrView'
local bind = LrView.bind -- shortcut for bind() method
local retval

local function setOptions()
  LrFunctionContext.callWithContext( "assignPresets", function( context )
      local psList = {}
      for _,fold in pairs(LrApplication.developPresetFolders()) do
        local foldname = fold:getName()
        for _,pst in pairs(fold:getDevelopPresets()) do
          table.insert(psList,{title = foldname..'->'..pst:getName(), value = pst:getUuid()})
        end
      end

      local f = LrView.osFactory()
      local properties = LrBinding.makePropertyTable( context )
      for i = 1,20 do
        properties['preset'..i][1] = prefs.Presets[i]
      end

      local contents = 
      f:view{
        place = 'horizontal',
        f:column {
          fill_horizontal = .75, 
          spacing = f:control_spacing(),
          bind_to_object = properties, -- default bound table
          f:tab_view {
            f:tab_view_item {
              title = 'Presets 1-5',
              identifier = 'presets-1-5',
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset1'
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset2'          
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset3'          
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset4'          
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset5'          
              },
            },
            f:tab_view_item {
              title = 'Presets 6-10',
              identifier = 'presets-6-10',
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset6'
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset7'          
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset8'          
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset9'          
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset10'          
              },
            },
            f:tab_view_item {
              title = 'Presets 11-15',
              identifier = 'presets-11-15',
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset11'
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset12'          
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset13'          
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset14'          
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset15'          
              },
            },
            f:tab_view_item {
              title = 'Presets 16-20',
              identifier = 'presets-16-20',
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset16'
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset17'          
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset18'          
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset19'          
              },
              f:simple_list {
                items = psList,
                allows_multiple_selection = false,
                value = bind 'preset20'          
              },
            },

          }
        },
        f:column {
          fill_horizontal = .25, 
          spacing = f:control_spacing(),
          bind_to_object = properties, -- default bound table
          f:static_text {
            fill_horizontal = 1,
            title = bind 'preset1',
          },
        }

      }

      local result = LrDialogs.presentModalDialog (
        {
          title = 'Set MIDI2LR options',
          contents = contents,
        }
      )
      retval = properties.preset1
    end)

end
setOptions() --execute
return retval[1] --for debugging