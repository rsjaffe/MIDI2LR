--[[----------------------------------------------------------------------------

Presets.lua

Manages develop presets for plugin

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
local LrTasks       = import 'LrTasks'
local LrDialogs     = import 'LrDialogs'
local LrView        = import 'LrView'

local number_of_presets = 80
local currentpreset = 1
local lastchange = 0

local function StartDialog(obstable,f)
  --populate table with presets
  for i = 1,number_of_presets do
    obstable['preset'..i] = {}
    obstable['preset'..i][1] = ProgramPreferences.Presets[i]
  end
  -- find presets stored in Lightroom and put in table name and UUID
  local psList = {}
  for _,fold in pairs(LrApplication.developPresetFolders()) do
    local foldname = fold:getName()
    for _,pst in pairs(fold:getDevelopPresets()) do
      psList[#psList+1] = {title = foldname..'\226\134\146'..pst:getName(), value = pst:getUuid()}
    end -- '\226\134\146' is right arrow in utf8
  end
  -- following variable set up number of rows and columns
  -- row*col must equal number_of_presets
  local group_rows, group_cols = 4,20
  local button_rows, button_cols = 4,20
  -- set up buttons on the right of the presets selection dialog
  local buttonpresets = {}
  for i = 1, button_cols do
    buttonpresets[i] = {
      spacing = f:control_spacing(),
      f:spacer { height = f:control_spacing() * 2}
    }
    for j = 1, button_rows do
      local k = button_rows * (i - 1) + j

      buttonpresets[i][#buttonpresets[i]+1] = f:push_button {fill_horizontal = 1, width_in_chars = 40, truncation = 'head',
        action = function() obstable['preset'..k] = nil end,
        title = LrView.bind { key = 'preset'..k,
          transform = function(value) return k..' '..(LrApplication.developPresetByUuid(value[1]):getName()) end
        },  -- title
      } -- push_button
    end
  end
  -- set up group boxes on left of selection dialog
  local grouppresets = {}
  for i = 1, group_cols do
    grouppresets[i] = {}
    for j = 1, group_rows do
      local k = group_rows * (i - 1) + j
      grouppresets[i][#grouppresets[i]+1] = f:simple_list {items = psList, allows_multiple_selection = false, value = LrView.bind ('preset'..k) }
    end
  end
  -- set up tabs
  local tabs = {}
  for i = 1,group_cols do
    local j = math.floor((i*group_rows-1)/button_rows)+1 --to determine which list of selected presets to include
    local label = (i-1)*group_rows+1 ..'-'..i*group_rows --must have space after 1 before ..
    tabs[i] = f:tab_view_item {title = label,
      identifier = 'tabview-'..label,
      f:row{
        f:column(grouppresets[i]),
        f:column(buttonpresets[j]) --for some reason, only shows in first group of each 'j' even though it is properly assigned
      } -- row
    } -- tabviewitem
  end
  return f:tab_view(tabs)
end

local function EndDialog(obstable, status)
  if status == 'ok' then
    ProgramPreferences.Presets = {} -- empty out prior settings
    for i = 1,number_of_presets do
      if type(obstable['preset'..i])=='table' then -- simple_list returns a table
        ProgramPreferences.Presets[i] = obstable['preset'..i][1]
      end
    end
  end
end

local function ApplyPreset(presetnumber)
  local presetUuid = ProgramPreferences.Presets[presetnumber]
  if presetUuid == nil or LrApplication.activeCatalog():getTargetPhoto() == nil then return end
  currentpreset = presetnumber -- for next/prev preset
  local preset = LrApplication.developPresetByUuid(presetUuid)
  LrTasks.startAsyncTask ( function ()
          --[[-----------debug section, enable by adding - to beginning this line
    LrMobdebug.on()
    --]]-----------end debug section
      LrApplication.activeCatalog():withWriteAccessDo(
        'Apply preset '..preset:getName(),
        function()
          if ProgramPreferences.ClientShowBezelOnChange then
            LrDialogs.showBezel(preset:getName())
          end
          LrApplication.activeCatalog():getTargetPhoto():applyDevelopPreset(preset)
        end,
        { timeout = 4,
          callback = function() LrDialogs.showError(LOC("$$$/AgCustomMetadataRegistry/UpdateCatalog/Error=The catalog could not be updated with additional module metadata.")..'PastePreset.') end,
          asynchronous = true }
      )
    end )
end

local function fApplyPreset(presetnumber)
  return function()
    local presetUuid = ProgramPreferences.Presets[presetnumber]
    if presetUuid == nil or LrApplication.activeCatalog():getTargetPhoto() == nil then return end
    currentpreset = presetnumber -- for next/prev preset
    local preset = LrApplication.developPresetByUuid(presetUuid)
    LrTasks.startAsyncTask ( function ()
            --[[-----------debug section, enable by adding - to beginning this line
    LrMobdebug.on()
    --]]-----------end debug section
        LrApplication.activeCatalog():withWriteAccessDo(
          'Apply preset '..preset:getName(),
          function()
            if ProgramPreferences.ClientShowBezelOnChange then
              LrDialogs.showBezel(preset:getName())
            end
            LrApplication.activeCatalog():getTargetPhoto():applyDevelopPreset(preset)
          end,
          { timeout = 4,
            callback = function() LrDialogs.showError(LOC("$$$/AgCustomMetadataRegistry/UpdateCatalog/Error=The catalog could not be updated with additional module metadata.")..'PastePreset.') end,
            asynchronous = true }
        )
      end )
  end
end

local function NextPreset()
  local testpreset
  if lastchange + 0.5 > os.clock() then
    return
  end
  lastchange = os.clock()
  for i = 1, number_of_presets do
    testpreset = currentpreset + i
    if testpreset > number_of_presets then
      testpreset = testpreset - number_of_presets
    end
    if ProgramPreferences.Presets[testpreset] then
      ApplyPreset(testpreset)
      break
    end
  end
end

local function PreviousPreset()
  local testpreset
    if lastchange + 0.5 > os.clock() then
    return
  end
  lastchange = os.clock()
  for i = 1, number_of_presets do
    testpreset = currentpreset - i
    if testpreset < 1 then
      testpreset = testpreset + number_of_presets
    end
    if ProgramPreferences.Presets[testpreset] then
      ApplyPreset(testpreset)
      break
    end
  end
end

return {
  EndDialog = EndDialog,
  NextPreset = NextPreset,
  PreviousPreset = PreviousPreset,
  StartDialog = StartDialog,
  fApplyPreset = fApplyPreset,
}