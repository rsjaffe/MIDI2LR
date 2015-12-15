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
local LrApplication     = import 'LrApplication'
local LrBinding         = import 'LrBinding'
local LrDialogs         = import 'LrDialogs'
local LrFunctionContext = import 'LrFunctionContext'
local prefs             = import 'LrPrefs'.prefsForPlugin() 
local LrView            = import 'LrView'
local bind              = LrView.bind -- shortcut for bind() method

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
      properties['ExposureLow'] = prefs['ExposureLow'] or -5 --value will be -10 to 10 for HDR DNGs, will handle in code
      properties['ExposureHigh'] = prefs['ExposureHigh'] or 5
      properties.PasteList = prefs.PasteList

      -- set up adjustments columns
      local adjustmentscol = {}
      adjustmentscol[1] = {}
      adjustmentscol[2] = {}
      adjustmentscol[3] = {}
      do 
        local breakpoint = math.floor(#DEVELOP_PARAMS / 3)
        for col = 1,3 do
          for i = ((col-1)*breakpoint+1),(breakpoint*col) do
            table.insert(
              adjustmentscol[col], 
              f:checkbox { title = DEVELOP_PARAMS[i], value = bind ('PasteList.'..DEVELOP_PARAMS[i]) } 
            )
          end
        end
      end
      -- set up presets list for the groupbox on the right of the presets selection dialog
      local groupboxpresets = {title = 'Selected presets'} 
      for i=1,20 do
        table.insert( 
          groupboxpresets, 
          f:static_text {fill_horizontal = 1,
            title = bind { key = 'preset'..i,
              transform = function(value) return 'Preset '..i..' '..(LrApplication.developPresetByUuid(value[1]):getName()) end
            },  -- title
          } -- static_text
        )
      end
      -- set up groups of preset listings
      local tabviewitems = {} 
      for group=1, 4 do
        tabviewitems[group] = f:tab_view_item {title = ('Presets '..(group*5-4)..'-'..(group*5)), identifier = ('presets-'..(group*5-4)..'-'..(group*5)),}
        for i=-4,0 do
          table.insert(tabviewitems[group],f:simple_list {items = psList, allows_multiple_selection = false, value = bind ('preset'..(group*5+i)) })
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
                f:tab_view { -- for choosing the presets
                  f:tab_view_item (tabviewitems[1]), -- tab_view_item
                  f:tab_view_item (tabviewitems[2]), -- tab_view_item
                  f:tab_view_item (tabviewitems[3]), -- tab_view_item
                  f:tab_view_item (tabviewitems[4]), -- tab_view_item
                }, -- tab_view
              }, -- column
              f:column{ -- for the display of chosen presets
                spacing = f:control_spacing(),
                f:spacer {
                  height = f:control_spacing() * 2,
                }, -- spacer
                f:group_box (groupboxpresets), -- group_box
              }, -- column
            }, -- row
          }, -- tab_view_item
          f:tab_view_item {
            title = 'Paste selections',
            identifier = 'pasteselections',
            f:row{ -- all available adjustments
              f:column (adjustmentscol[1]),
              f:column (adjustmentscol[2]), 
              f:column (adjustmentscol[3]),
            }, --row
          }, -- tab_view_item
          f:tab_view_item {
            title = 'Other settings',
            identifier = 'othersettings',
            f:row { --Temperature row
              f:static_text{
                title = 'Temperature Limits',
                width = LrView.share('limit_label'),
              }, -- static_text
              f:slider {
                value = bind 'TemperatureLow',
                min = templow, 
                max = temphigh,
                integral = true,
                enabled = templow >= 0,
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
                enabled = templow >= 0,
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
                end,
                enabled = templow >= 0,
              }, -- push_button
            }, -- row
            f:row { --Tint row
              f:static_text{
                title = 'Tint Limits',
                width = LrView.share('limit_label'),
              }, -- static_text
              f:slider {
                value = bind 'TintLow',
                min = tintlow, 
                max = tinthigh,
                integral = true,
                enabled = templow >= 0,
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
                enabled = templow >= 0,
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
                end,
                enabled = templow >= 0,
              }, -- push_button
            }, -- row
            f:row { --Exposure row
              f:static_text{
                title = 'Exposure Limits',
                width = LrView.share('limit_label'),
              }, -- static_text
              f:slider {
                value = bind 'ExposureLow',
                min = -5, 
                max = 5,
                integral = true,
              }, -- slider
              f:static_text {
                title = bind 'ExposureLow',
                alignment = 'right',
                width = LrView.share('limit_reading'),                 
              }, -- static_text
              f:slider {
                value = bind 'ExposureHigh',
                min = -5 ,
                max = 5,
                integral = true,
              }, -- slider
              f:static_text {
                title = bind 'ExposureHigh',
                alignment = 'right',
                width = LrView.share('limit_reading'),                 
              }, -- static_text
              f:push_button {
                title = 'Reset to defaults',
                action = function ()
                  properties.ExposureLow = -5
                  properties.ExposureHigh = 5
                end,
              }, -- push_button
            }, -- row
            f:row { --explanation of exposure
              f:static_text {
                title = 'Note that exposure limits will be doubled for HDR images. For example, if the limits were set at -3 and +3, for HDR images it would be -6 and +6',
              }, -- static_text
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
        ------assign presets
        for i = 1,20 do
          if properties['preset'..i] then
            prefs.Presets[i] = properties['preset'..i][1]
          end
        end
        prefs.Presets = prefs.Presets --to ensure that preferences in LR get updated for deep updates
        MIDI2LR.Presets = prefs.Presets -- read only global to access preferences
        ------assign Temperature and Tint
        for _,v in ipairs { 'Temperature', 'Tint', 'Exposure' } do
          if properties[v..'Low'] > properties[v..'High'] then --swap values
            properties[v..'Low'], properties[v..'High'] = properties[v..'High'], properties[v..'Low']
          end
          prefs[v..'Low'] = properties[v..'Low']
          prefs[v..'High'] = properties[v..'High']
          MIDI2LR[v..'Low'] = properties[v..'Low']
          MIDI2LR[v..'High'] = properties[v..'High']
        end
        ------assign PasteList
        prefs.PasteList, MIDI2LR.PasteList = {},{} -- empty out prior settings
        for k,v in pairs(properties.PasteList) do --use iterator--simple assignment causes issue (probably due to bound table iterator issues)
          prefs.PasteList[k] = v
          MIDI2LR.PasteList[k] = v
        end

      end
    end)
end
setOptions() --execute
