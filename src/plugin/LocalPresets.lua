--[[------------------------------------------------------------------------------------

LocalPresets.lua

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
---------------------------------------------------------------------------------------]]

--Values from a local preset with all sliders set to maximum values
--[[
 blacks2012 = 1,
 clarity = 0,
 clarity2012 = 1,
 contrast = 0,
 contrast2012 = 1,
 defringe = 1,
 dehaze = 1,
 exposure = 0,
 exposure2012 = 1,
 highlights2012 = 1,
 hue = 1,
 luminanceNoise = 1,
 moire = 1,
 saturation = 1,
 shadows2012 = 1,
 sharpness = 1,
 temperature = 1,
 texture = 1,
 tint = 1,
 toningHue = 327,
 toningLuminance = 0,
 toningSaturation = 1,
 whites2012 = 1,
--]]

local LrApplicationView   = import 'LrApplicationView'
local LrDevelopController = import 'LrDevelopController'
local LrDialogs     = import 'LrDialogs'
local LrFileUtils   = import 'LrFileUtils'
local LrPathUtils   = import 'LrPathUtils'
local LrView        = import 'LrView'

local LocalAdjustmentPresetsPath = LrPathUtils.child(LrPathUtils.getStandardFilePath ('appData') , 'Local Adjustment Presets')

local LocalPresets = {}  --Store presets in table when reqested by user : key = filename, value = preset values table

local localPresetMap = {
  blacks2012 = "local_Blacks",
  clarity = "",
  clarity2012 = "local_Clarity",
  contrast = "",
  contrast2012 = "local_Contrast",
  defringe = "local_Defringe",
  dehaze = "local_Dehaze",
  exposure = "",
  exposure2012 = "local_Exposure",
  highlights2012 = "local_Highlights",
  hue = "local_Hue",
  luminanceNoise = "local_LuminanceNoise",
  moire = "local_Moire",
  saturation = "local_Saturation",
  shadows2012 = "local_Shadows",
  sharpness = "local_Sharpness",
  temperature = "local_Temperature",
  texture = "local_Texture",
  tint = "local_Tint",
  toningHue = "", --"local_ToningHue" doesn't exsist
  toningLuminance = "", -- "local_ToningLuminance" doesn't exist
  toningSaturation = "", --"local_ToningSaturation" doesn't exsist
  whites2012 = "local_Whites"
}

local function GetPresetFilenames()
  local filenames = { { title='', value='' }, }
  --Extract filename only from full paths
  for afile in LrFileUtils.recursiveDirectoryEntries ( LocalAdjustmentPresetsPath ) do
    if LrFileUtils.fileAttributes(afile).fileSize and LrPathUtils.extension(afile) == 'lrtemplate'
    then
      table.insert (filenames, { title=string.gsub(LrPathUtils.removeExtension(LrPathUtils.leafName(afile)),'-','|'),value=afile } )
    end
  end
  table.sort(filenames, function (a,b) return (a.title < b.title) end)
  return filenames
end

local function ApplyLocalPreset(LocalPresetFilename)  --LocalPresetName eg: 'Burn (Darken).lrtemplate'
  local LocalPresetName = LrPathUtils.removeExtension(LrPathUtils.leafName(LocalPresetFilename))
  if LrApplicationView.getCurrentModuleName() == 'develop' and LrDevelopController.getSelectedTool() == 'masking' then
    if LocalPresetFilename == '' or LocalPresetFilename == nil then
      return
    end
    if not LrFileUtils.exists(LocalPresetFilename) then
      LrDialogs:message(LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SettingsString/ConstructionWithColon=^1: ^2",LOC("$$$/AgImageIO/Errors/FileNotFound=File not found"),LocalPresetName),'warning')
      return
    end
    --Check to see if preset is already loaded by checking table...if so do not reload file.
    --Reloading template file on each request would however allow the user to update and save local preset settings in lightroom.
    if LocalPresets[tostring(LocalPresetName)] == nil then
      local f = io.open(LocalPresetFilename)
      local strPreset = f:read("*a")
      f:close()

      --I had to remove 'ZSTR' from the built-in .lrtemplate preset files as it would not load/execute file
      local LocalPresetFile = loadstring(string.gsub(strPreset,'ZSTR',''))  --Loads into a function which is later called
      LocalPresetFile() --Execute the loaded file string as lua code.  This will give access to the variable 's'

      LocalPresets[tostring(LocalPresetName)] = s['value'] --Add the currently selected preset to the table of presets
    end
    if ProgramPreferences.ClientShowBezelOnChange then
      LrDialogs.showBezel (LocalPresetName)
    end
    --Apply preset to LR
    for param, MappedParam in pairs(localPresetMap) do
      local value = LocalPresets[LocalPresetName][param]
      if value == nil then value=0; end
      if MappedParam == 'local_Exposure' then
        value = value * 4
      else
        value = value * 100
      end
      MIDI2LR.PARAM_OBSERVER[MappedParam] = value
      LrDevelopController.setValue(MappedParam, value)
    end
  end
end

local numseries = 50 -- number of presets allowed

local function StartDialog(obstable,f)
  local PresetFileNames = GetPresetFilenames()
  for i = 1,numseries do
    obstable['LocalPresets'..i] = ProgramPreferences.LocalPresets[i]
  end
  local dlgrows = {}
  local numdiv2 = numseries / 2
  for i=1, numdiv2 do
    dlgrows[i] = f:row{
      bind_to_object = obstable, -- default bound table
      f:static_text{title = LOC("$$$/MIDI2LR/LocalPresets/Presets=Local adjustments presets").." "..i,
        width = LrView.share('local_presets_label')},
      f:popup_menu{
        items = PresetFileNames,
        value = LrView.bind('LocalPresets'..i)
      }
    }
  end
  local dlgrows1 = {}
    for i=numdiv2 + 1, numseries do
    dlgrows1[i - numdiv2] = f:row{
      bind_to_object = obstable, -- default bound table
      f:static_text{title = LOC("$$$/MIDI2LR/LocalPresets/Presets=Local adjustments presets").." "..i,
        width = LrView.share('local_presets_label')},
      f:popup_menu{
        items = PresetFileNames,
        value = LrView.bind('LocalPresets'..i)
      }
    }
  end
  return f:row{f:column(dlgrows),f:column(dlgrows1)}
end

local function EndDialog(obstable, status)
  if status == 'ok' then
    ProgramPreferences.LocalPresets = {} -- empty out prior settings
    for i = 1,numseries do
      ProgramPreferences.LocalPresets[i] = obstable['LocalPresets'..i]
    end
  end
end

return {
  ApplyLocalPreset   = ApplyLocalPreset,
  EndDialog          = EndDialog,
  GetPresetFilenames = GetPresetFilenames,
  StartDialog        = StartDialog,
}
