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
        properties['preset'..i] = {}
        properties['preset'..i][1] = prefs.Presets[i]
      end

      local contents = 
      f:view{
        f:row {
          f:tab_view {
            f:tab_view_item {
              title = 'Presets',
              identifier = 'presets',
              f:column {
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
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset2'          
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset3'          
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset4'          
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset5'          
                    }, -- simple_list
                  }, -- tab_view_item
                  f:tab_view_item {
                    title = 'Presets 6-10',
                    identifier = 'presets-6-10',
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset6'
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset7'          
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset8'          
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset9'          
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset10'          
                    }, -- simple_list
                  }, -- tab_view_item
                  f:tab_view_item {
                    title = 'Presets 11-15',
                    identifier = 'presets-11-15',
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset11'
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset12'          
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset13'          
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset14'          
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset15'          
                    }, -- simple_list
                  }, -- tab_view_item
                  f:tab_view_item {
                    title = 'Presets 16-20',
                    identifier = 'presets-16-20',
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset16'
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset17'          
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset18'          
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset19'          
                    }, -- simple_list
                    f:simple_list {
                      items = psList,
                      allows_multiple_selection = false,
                      value = bind 'preset20'          
                    }, -- simple_list
                  }, -- tab_view_item
                }, -- tab_view
              }, -- column
              f:column{
                spacing = f:control_spacing(),
                bind_to_object = properties, 
                f:group_box {
                  title = 'Selected presets',
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset1',
                      transform = function(value)
                        return 'Preset 1 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset2',
                      transform = function(value)
                        return 'Preset 2 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset3',
                      transform = function(value)
                        return 'Preset 3 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset4',
                      transform = function(value)
                        return 'Preset 4 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset5',
                      transform = function(value)
                        return 'Preset 5 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset6',
                      transform = function(value)
                        return 'Preset 6 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset7',
                      transform = function(value)
                        return 'Preset 7 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset8',
                      transform = function(value)
                        return 'Preset 8 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset9',
                      transform = function(value)
                        return 'Preset 9 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset10',
                      transform = function(value)
                        return 'Preset 10 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset11',
                      transform = function(value)
                        return 'Preset 11 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset12',
                      transform = function(value)
                        return 'Preset 12 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset13',
                      transform = function(value)
                        return 'Preset 13 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset14',
                      transform = function(value)
                        return 'Preset 14 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset15',
                      transform = function(value)
                        return 'Preset 15 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset16',
                      transform = function(value)
                        return 'Preset 16 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset17',
                      transform = function(value)
                        return 'Preset 17 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset18',
                      transform = function(value)
                        return 'Preset 18 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset19',
                      transform = function(value)
                        return 'Preset 19 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                  f:static_text {
                    fill_horizontal = 1,
                    title = bind {
                      key = 'preset20',
                      transform = function(value)
                        return 'Preset 20 '..LrApplication.developPresetByUuid(value[1]):getName()
                      end
                    },  -- title
                  }, -- static_text
                }, -- group_box
              }, -- column
            }, -- tab_view_item
            f:tab_view_item {
              title = 'Other settings',
              identifier = 'othersettings',
              f:static_text{ title = 'will place other settings here' }, -- static_text 
            }, -- tab_view_item
          }, -- tab_view
        }, -- row
      } -- view



      local result = LrDialogs.presentModalDialog (
        {
          title = 'Set MIDI2LR options',
          contents = contents,
        }
      )
      if result == 'ok' then
        for i = 1,20 do
          if properties['preset'..i] then
            prefs.Presets[i] = properties['preset'..i][1]
          end
        end
        prefs.Presets = prefs.Presets --to ensure that preferences in LR get updated
        MIDI2LR.Presets = prefs.Presets -- read only global to access preferences
      end
    end)
end
setOptions() --execute
