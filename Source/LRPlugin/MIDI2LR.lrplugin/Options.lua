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

local Parameters        = require 'Parameters'
local Paste             = require 'Paste'
local Preferences       = require 'Preferences'
local Limits            = require 'Limits' 
local LrApplication     = import 'LrApplication'
local LrApplicationView = import 'LrApplicationView'
local LrBinding         = import 'LrBinding'
local LrDialogs         = import 'LrDialogs'
local LrDevelopController = import 'LrDevelopController'
local LrFunctionContext = import 'LrFunctionContext'
local LrView            = import 'LrView'

local LrMobdebug = import 'LrMobdebug'

local function setOptions()
  LrFunctionContext.callWithContext( "assignPresets", function( context )
      LrMobdebug.on() --debugging
      -- initialize variables needed throughout this function
      local limitsCanBeSet = (LrApplication.activeCatalog():getTargetPhoto() ~= nil) and (LrApplicationView.getCurrentModuleName() == 'develop')
      local f = LrView.osFactory()
      --------------------------bound property table setup begins
      --make bound property tables
      local properties = LrBinding.makePropertyTable( context )
      --populate table with presets
      for i = 1,20 do
        properties['preset'..i] = {}
        properties['preset'..i][1] = ProgramPreferences.Presets[i]
      end
      --populate table with limits
      if limitsCanBeSet then
        for p in pairs(Limits.Parameters) do
          local min,max = Limits.GetMinMax(p)
          properties[p..'Low'] = min
          properties[p..'High'] = max
        end
      end

      for k,v in pairs(ProgramPreferences.PasteList) do
        properties['Paste'..k] = v 
      end 
      --------------------------bound property table setup ends

      --------------------------dialog box setup begins
      ---------------presets dialog setup begins
      -- find presets stored in Lightroom and put in table name and UUID
      local psList = {}
      for _,fold in pairs(LrApplication.developPresetFolders()) do
        local foldname = fold:getName()
        for _,pst in pairs(fold:getDevelopPresets()) do
          table.insert(psList,{title = foldname..'\226\134\146'..pst:getName(), value = pst:getUuid()})
        end -- '\226\134\146' is right arrow in utf8
      end
      -- set up presets list for the groupbox on the right of the presets selection dialog
      local groupboxpresets = {title = LOC("$$$/AgIdentityPlates/MainDialog/Choose=Choose")..' '..LOC("$$$/SmartCollection/Criteria/DevelopPreset=develop preset")} 
      for i=1,20 do
        table.insert( 
          groupboxpresets, 
          f:static_text {fill_horizontal = 1,
            width_in_chars = 50,
            truncation = 'head',
            title = LrView.bind { key = 'preset'..i,
              transform = function(value) return LOC("$$$/SmartCollection/Criteria/DevelopPreset=Develop preset")..' '..i..' '..(LrApplication.developPresetByUuid(value[1]):getName()) end
            },  -- title
          } -- static_text
        )
      end
      -- set up groups of preset listings
      local tabviewitems = {} 
      local psrows, pscolumns = 4,5
      for group=1, pscolumns do
        tabviewitems[group] = f:tab_view_item {title = (LOC("$$$/SmartCollection/Criteria/DevelopPreset=Develop preset")..' '..(group*psrows-pscolumns)..'-'..(group*psrows)), identifier = ('presets-'..(group*psrows-pscolumns)..'-'..(group*psrows)),}
        for i=(1-psrows),0 do
          table.insert(tabviewitems[group],f:simple_list {items = psList, allows_multiple_selection = false, value = LrView.bind ('preset'..(group*psrows+i)) })
        end
        tabviewitems[group] = f:tab_view_item (tabviewitems[group]) -- prepare for use in f:tabview below
      end -- for group
      ---------------presets dialog setup ends

      ---------------selective paste dialog setup begins
      -- set up adjustments columns
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
      ---------------selective paste dialog setup ends

      ---------------other settings dialog setup begins
      -- set up other settings column
      local othercolumn = {
        title = 'Other settings',
        identifier = 'othersettings',
      }
      if limitsCanBeSet then -- don't set up limits if photo isn't selected
        for _,v in ipairs(Limits.OptionsRows(f,properties)) do
          table.insert(othercolumn,v)
        end
      end
      ---------------other settings dialog setup ends
      --------------------------dialog box setup ends

      -- final assembly of dialog box contents
      local contents = 
      f:view{
        bind_to_object = properties, -- default bound table
        f:tab_view {
          f:tab_view_item {
            title = LOC("$$$/SmartCollection/Criteria/DevelopPreset=Develop Preset"),
            identifier = 'presets',
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
                f:group_box (groupboxpresets), -- group_box
              }, -- column
            }, -- row
          }, -- tab_view_item
          f:tab_view_item {
            title = LOC('$$$/MIDI2LR/Options/pastesel=Paste selections'),
            identifier = 'pasteselections',
            f:row (selectivepastecol),
            f:row{
              f:push_button {
                title = LOC("$$$/AgCameraRawNamedSettings/NamedSettingsControls/CheckNone=Check none"),
                action = function ()
                  for i,v in ipairs(Parameters.Order) do
                    properties['Paste'..v] = false
                  end 
                end,
              }, -- push_button
              f:push_button {
                title = LOC("$$$/AgCameraRawNamedSettings/NamedSettingsControls/CheckAll=Check all"
                ),
                action = function ()
                  for i,v in ipairs(Parameters.Order) do
                    properties['Paste'..v] = true
                  end 
                end,
              } ,-- push_button
            } --row of pushbuttons
          }, -- tab_view_item
          f:tab_view_item (othercolumn), -- tab_view_item
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
        ProgramPreferences.Presets = {} -- empty out prior settings
        for i = 1,20 do
          if type(properties['preset'..i])=='table' then -- simple_list should return a table
            ProgramPreferences.Presets[i] = properties['preset'..i][1]
          end
        end
        --assign PasteList
        ProgramPreferences.PasteList = {} -- empty out prior settings
        for i,v in ipairs(Parameters.Order) do
          ProgramPreferences.PasteList[v] = properties['Paste'..v]
        end 

        --assign limits
        if limitsCanBeSet then -- do NOT empty out prior settings, this is setting for one type picture only
          for p in pairs(Limits.Parameters) do
            if properties[p..'Low'] > properties[p..'High'] then --swap values
              properties[p..'Low'], properties[p..'High'] = properties[p..'High'], properties[p..'Low']
            end
            local _,max = LrDevelopController.getRange(p) --limitsCanBeSet only when in Develop module, no need to check again
            ProgramPreferences.Limits[p][max] = {properties[p..'Low'], properties[p..'High']}
          end
        end --if limitsCanBeSet
        Preferences.Save()
      end -- if result ok
      -- finished with assigning values from dialog
    end)
end
setOptions() --execute
