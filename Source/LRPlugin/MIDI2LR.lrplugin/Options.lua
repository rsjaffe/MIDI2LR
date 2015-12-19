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
local Limits            = require 'Limits' -- import module
local LrApplication     = import 'LrApplication'
local LrBinding         = import 'LrBinding'
local LrDialogs         = import 'LrDialogs'
local LrFunctionContext = import 'LrFunctionContext'
local prefs             = import 'LrPrefs'.prefsForPlugin() 
local LrView            = import 'LrView'
local bind              = LrView.bind -- shortcut for bind() method

prefs           = prefs or {}
prefs.Presets   = prefs.Presets or {}
prefs.PasteList = prefs.PasteList or {}

local function setOptions()
  LrFunctionContext.callWithContext( "assignPresets", function( context )
      local photoIsSelected = LrApplication.activeCatalog():getTargetPhoto() ~= nil
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

      for k,v in pairs(Limits.GetPreferencesCurrentMode()) do
        properties[k] = v
      end

      properties.PasteList = {}
      for k,v in pairs(prefs.PasteList) do
        properties.PasteList[k] = v --straight assignment seems to cause prefs to update automatically
      end

      -- set up presets list for the groupbox on the right of the presets selection dialog
      local groupboxpresets = {title = LOC('$$$/MIDI2LR/Options/grpboxpresets=Selected presets')} 
      for i=1,20 do
        table.insert( 
          groupboxpresets, 
          f:static_text {fill_horizontal = 1,
            title = bind { key = 'preset'..i,
              transform = function(value) return LOC('$$$/MIDI2LR/Options/Preset=Preset')..' '..i..' '..(LrApplication.developPresetByUuid(value[1]):getName()) end
            },  -- title
          } -- static_text
        )
      end
      -- set up groups of preset listings
      local tabviewitems = {} 
      for group=1, 4 do
        tabviewitems[group] = f:tab_view_item {title = (LOC('$$$/MIDI2LR/Options/Presets=Presets')..' '..(group*5-4)..'-'..(group*5)), identifier = ('presets-'..(group*5-4)..'-'..(group*5)),}
        for i=-4,0 do
          table.insert(tabviewitems[group],f:simple_list {items = psList, allows_multiple_selection = false, value = bind ('preset'..(group*5+i)) })
        end
      end
      -- set up adjustments columns
      local adjustmentscol = {}
      do 
        local breakpoint = math.floor(#DEVELOP_PARAMS / 3)
        for col = 1,3 do
          adjustmentscol[col] = {}
          for i = ((col-1)*breakpoint+1),(breakpoint*col) do
            table.insert(
              adjustmentscol[col], 
              f:checkbox { title = DEVELOP_PARAMS[i], value = bind ('PasteList.'..DEVELOP_PARAMS[i]) } 
            )
          end
        end
      end
      -- set up other settings column
      local parameterscolumn = {
        title = 'Other settings',
        identifier = 'othersettings',
      }
      if photoIsSelected then -- don't set up limits if photo isn't selected
        for _,v in ipairs(Limits.OptionsRows(f,properties)) do
          table.insert(parameterscolumn,v)
        end
      end

      -- final assembly of dialog box contents
      local contents = 
      f:view{
        bind_to_object = properties, -- default bound table
        f:tab_view {
          f:tab_view_item {
            title = LOC('$$$/MIDI2LR/Options/Presets=Presets'),
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
            title = LOC('$$$/MIDI2LR/Options/pastesel=Paste selections'),
            identifier = 'pasteselections',
            f:row{ -- all available adjustments
              f:column (adjustmentscol[1]),
              f:column (adjustmentscol[2]), 
              f:column (adjustmentscol[3]),
            }, --row
            f:row{
              f:push_button {
                title = LOC('$$$/MIDI2LR/Options/clearall=Clear all'),
                action = function ()
                  for _,p in ipairs(DEVELOP_PARAMS) do
                    properties['PasteList.'..p] = false
                  end
                end,
              }, -- push_button
              f:push_button {
                title = LOC('$$$/MIDI2LR/Options/setall=Set all'),
                action = function ()
                  for _,p in ipairs(DEVELOP_PARAMS) do
                    properties['PasteList.'..p] = true
                  end
                end,
              } ,-- push_button
            }, --row
          }, -- tab_view_item
          f:tab_view_item (parameterscolumn), -- tab_view_item
        }, -- tab_view
      } -- view

      -- display dialog
      local result = LrDialogs.presentModalDialog (
        {
          title = LOC('$$$/MIDI2LR/Options/dlgtitle=Set MIDI2LR options'),
          contents = contents,
        }
      )
      -- assign values from dialog if ok is pressed
      if result == 'ok' then
        --assign presets
        for i = 1,20 do
          if properties['preset'..i] then
            prefs.Presets[i] = properties['preset'..i][1]
          end
        end
        prefs.Presets = prefs.Presets --to ensure that preferences in LR get updated for deep updates
        MIDI2LR.Presets = prefs.Presets -- read only global to access preferences
        --assign PasteList
        prefs.PasteList, MIDI2LR.PasteList = {},{} -- empty out prior settings
        for k,v in pairs(properties.PasteList) do --use iterator--simple assignment causes issue (probably due to bound table iterator issues)
          prefs.PasteList[k] = v
          MIDI2LR.PasteList[k] = v
        end
        --assign limits
        if photoIsSelected then
          for p in pairs(Limits.Parameters) do
            if properties[p..'Low'] > properties[p..'High'] then --swap values
              properties[p..'Low'], properties[p..'High'] = properties[p..'High'], properties[p..'Low']
            end
          end
          Limits.SavePreferencesOneMode(properties)
          Limits.SavePreferencesOneMode(properties,MIDI2LR)
        end --if photoIsSelected
      end -- if result ok
    end)
end
setOptions() --execute
