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

require 'Develop_Params.lua' -- global table of develop params for selection when pasting
local LrApplication = import 'LrApplication'
local LrBinding = import 'LrBinding'
local LrDialogs = import 'LrDialogs'
local LrFunctionContext = import 'LrFunctionContext'
local prefs = import 'LrPrefs'.prefsForPlugin() 
local LrView = import 'LrView'
local bind = LrView.bind -- shortcut for bind() method

prefs = prefs or {}
prefs.Presets = prefs.Presets or {}
prefs.PasteList = prefs.PasteList or {}

--[[ FOR DEBUGGING ONLY. REMOVE - to comment out
MIDI2LR = {}
--]]


local function setOptions()
  LrFunctionContext.callWithContext( "assignPresets", function( context )
      local LrDevelopController = import 'LrDevelopController'
      local psList = {}
      for _,fold in pairs(LrApplication.developPresetFolders()) do
        local foldname = fold:getName()
        for _,pst in pairs(fold:getDevelopPresets()) do
          table.insert(psList,{title = foldname..'\226\134\146'..pst:getName(), value = pst:getUuid()})
        end -- '\226\134\146' is right arrow in utf8
      end

      local f = LrView.osFactory()
      local properties = LrBinding.makePropertyTable( context )
      for i = 1,20 do
        properties['preset'..i] = {}
        properties['preset'..i][1] = prefs.Presets[i]
      end

      local templow,temphigh = LrDevelopController.getRange('Temperature') 
      properties['TemperatureLow'] = prefs['TemperatureLow'] or 3000
      properties['TemperatureHigh'] = prefs['TemperatureHigh'] or 9000
      local tintlow,tinthigh = LrDevelopController.getRange('Tint') 
      properties['TintLow'] = prefs['TintLow'] or tintlow
      properties['TintHigh'] = prefs['TintHigh'] or tinthigh

      local adjustmentscol1, adjustmentscol2, adjustmentscol3 = {}, {}, {}
      properties.PasteList = prefs.PasteList

      do -- set up adjustments columns
        local breakpoint = math.floor(#DEVELOP_PARAMS / 3)
        for i = 1,breakpoint do
          table.insert(
            adjustmentscol1, 
            f:checkbox { title = DEVELOP_PARAMS[i], value = bind ('PasteList.'..DEVELOP_PARAMS[i]) } 
          )
        end
        for i = breakpoint + 1, breakpoint * 2 do
          table.insert(
            adjustmentscol2, 
            f:checkbox { title = DEVELOP_PARAMS[i], value = bind ('PasteList.'..DEVELOP_PARAMS[i]) } 
          )
        end
        for i = (breakpoint * 2) + 1, #DEVELOP_PARAMS do
          table.insert(
            adjustmentscol3, 
            f:checkbox { title = DEVELOP_PARAMS[i], value = bind ('PasteList.'..DEVELOP_PARAMS[i]) } 
          )
        end 
      end


      local contents = 
      f:view{
        bind_to_object = properties, -- default bound table
        f:tab_view {
          f:tab_view_item {
            title = 'Presets',
            identifier = 'presets',
            f:row {
              f:column {
                spacing = f:control_spacing(),
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
                f:spacer {
                  height = f:control_spacing() * 2,
                }, -- spacer
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
            }, -- row
          }, -- tab_view_item
          f:tab_view_item {
            title = 'Paste selections',
            identifier = 'pasteselections',
            f:row{
              f:column (adjustmentscol1),
              f:column (adjustmentscol2) , 
              f:column (adjustmentscol3),
            }, --row
          }, -- tab_view_item
          f:tab_view_item {
            title = 'Other settings',
            identifier = 'othersettings',
            f:row {
              f:static_text{
                title = 'Temperature Limits',
                width = LrView.share('limit_label'),
              }, -- static_text
              f:slider {
                value = bind 'TemperatureLow',
                min = templow, 
                max = temphigh,
                integral = true,
              }, -- slider
              f:static_text {
                title = bind 'TemperatureLow',
                alignment = 'right',
                width = LrView.share('limit_reading'),  
              }, -- static_text
              f:slider {
                value = bind 'TemperatureHigh',
                min = templow ,
                max = temphigh,
                integral = true,
              }, -- slider
              f:static_text {
                title = bind 'TemperatureHigh',
                alignment = 'right',
                width = LrView.share('limit_reading'),                
              }, -- static_text
              f:push_button {
                title = 'Reset to defaults',
                action = function ()
                  properties.TemperatureLow = 3000
                  properties.TemperatureHigh = 9000
                end
              }, -- push_button
            }, -- row
            f:row {
              f:static_text{
                title = 'Tint Limits',
                width = LrView.share('limit_label'),
              }, -- static_text

              f:slider {
                value = bind 'TintLow',
                min = tintlow, 
                max = tinthigh,
                integral = true,
              }, -- slider
              f:static_text {
                title = bind 'TintLow',
                alignment = 'right',
                width = LrView.share('limit_reading'),                 
              }, -- static_text
              f:slider {
                value = bind 'TintHigh',
                min = tintlow ,
                max = tinthigh,
                integral = true,
              }, -- slider
              f:static_text {
                title = bind 'TintHigh',
                alignment = 'right',
                width = LrView.share('limit_reading'),                 
              }, -- static_text
              f:push_button {
                title = 'Reset to defaults',
                action = function ()
                  properties.TintLow = tintlow
                  properties.TintHigh = tinthigh
                end
              }, -- push_button
            }, -- row
          }, -- tab_view_item
        }, -- tab_view
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
        if properties.TemperatureLow > properties.TemperatureHigh then -- swap values
          properties.TemperatureLow, properties.TemperatureHigh = properties.TemperatureHigh, properties.TemperatureLow
        end
        if properties.TintLow > properties.TintHigh then -- swap values
          properties.TintLow, properties.TintHigh = properties.TintHigh, properties.TintLow
        end

        for _,v in ipairs { 'Temperature', 'Tint' } do
          prefs[v..'Low'] = properties[v..'Low']
          prefs[v..'High'] = properties[v..'High']
          MIDI2LR[v..'Low'] = properties[v..'Low']
          MIDI2LR[v..'High'] = properties[v..'High']
        end
        prefs.PasteList = properties.PasteList
        MIDI2LR.PasteList = prefs.PasteList
        prefs.Presets = prefs.Presets --to ensure that preferences in LR get updated for deep updates
        MIDI2LR.Presets = prefs.Presets -- read only global to access preferences
      end
    end)
end
setOptions() --execute
