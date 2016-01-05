--[[----------------------------------------------------------------------------

Profiles.lua

Manages profile changes for plugin
 
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

local LrApplicationView   = import 'LrApplicationView'
local LrDevelopController = import 'LrDevelopController'

local ProfileTypes = {
  loupe = {ToolModulePanel = 'Tool', friendlyName = LOC("$$$/AgPhotoBin/ViewMode/Develop/Loupe=Loupe"),},
  crop = {ToolModulePanel = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Crop"),},
  dust = {ToolModulePanel = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/SpotRemoval=Spot Removal"),},
  redeye = {ToolModulePanel = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Redeye=Red-eye Correction"),},
  gradient = {ToolModulePanel = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/GraduatedFilters=Graduated Filters"),},
  circularGradient = {ToolModulePanel = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/RadialFilters=Radial Filters"),},
  localized = {ToolModulePanel = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/BrushAdjustments=Brush Adjustments"),},
  adjustPanel = {ToolModulePanel = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/BasicAdjustments=Basic Adjustments"),},
  tonePanel = {ToolModulePanel = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/ToneCurve=Tone Curve"),},
  mixerPanel = {ToolModulePanel = 'Panel', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/ColorAdjustments=Color Adjustments"),},
  splitToningPanel = {ToolModulePanel = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/SplitToning=Split Toning"),},
  detailPanel = {ToolModulePanel = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/Detail=Details")},
  lensCorrectionsPanel = {ToolModulePanel = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/LensCorrections=Lens Corrections"),},
  effectsPanel = {ToolModulePanel = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/Effects=Effects"),},
  calibratePanel = {ToolModulePanel = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/Calibration=Camera Calibration"),},
  library = {ToolModulePanel = 'Module', friendlyName = LOC("$$$/AgLibraryModule/ModuleTitle=Library"),},
  develop = {ToolModulePanel = 'Module', friendlyName = LOC("$$$/SmartCollection/Criteria/Heading/Develop=Develop"),},
  map = {ToolModulePanel = 'Module', friendlyName = LOC("$$$/AgLocation/ModuleTitle=Map"),},
  book = {ToolModulePanel = 'Module', friendlyName = LOC("$$$/Ag/Layout/Book/ModuleTitle=Book"),},
  slideshow = {ToolModulePanel = 'Module', friendlyName = LOC("$$$/AgApplication/Menu/Window/SecondMonitor/Slideshow=Slideshow"),},
  print = {ToolModulePanel = 'Module', friendlyName = LOC("$$$/AgPrint/Menu/File/Print=Print"),},
  web = {ToolModulePanel = 'Module', friendlyName = LOC("$$$/WPG/Help/Shortcuts/WebHeader=Web"),},
  profile1 = {ToolModulePanel = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 1',},
  profile2 = {ToolModulePanel = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 2',},
  profile3 = {ToolModulePanel = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 3',},
  profile4 = {ToolModulePanel = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 4',},
  profile5 = {ToolModulePanel = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 5',},
  profile6 = {ToolModulePanel = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 6',},
  profile7 = {ToolModulePanel = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 7',},
  profile8 = {ToolModulePanel = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 8',},
  profile9 = {ToolModulePanel = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 9',},
  profile10 = {ToolModulePanel = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 10',},
}

local current = {Tool = '', Module = '', Panel = ''}


local function useDefaults()
  ProgramPreferences.Profiles = {}
  for k in pairs(ProfileTypes) do
    ProgramPreferences.Profiles[k] = ''
  end
end

local function changeProfile(profilename, ignoreCurrent)
  local changed = false
  if profilename and ProfileTypes[profilename] then
    local newprofile_file = ProgramPreferences.Profiles[profilename]
    local newToolModulePanel = ProfileTypes[profilename]['ToolModulePanel']
    if (newprofile_file ~= '') and 
    ((ignoreCurrent == true) or (newToolModulePanel == '') or (current[newToolModulePanel] ~= profilename)) then
      MIDI2LR.SERVER:send('SwitchProfile '..newprofile_file..'\n')
      if newToolModulePanel ~= '' then
        current[newToolModulePanel] = profilename
      end
      changed = true
    end
  end
  return changed
end

local function checkProfile()
  local newmod = LrApplicationView.getCurrentModuleName()
  if newmod == 'develop' then 
    local tool = LrDevelopController.getSelectedTool()
    if ProgramPreferences.Profiles[tool] == '' then
      changeProfile(newmod)
    else
      changeProfile(tool)
    end
  else
    changeProfile(newmod)
  end
end

local function StartDialog(obstable,f)
  for k in pairs(ProfileTypes) do
    obstable['Profile'..k] = ProgramPreferences.Profiles[k]
  end
  --design dialog portion here and return it
end

local function EndDialog(obstable, status)
  if status == 'ok' then
    useDefaults() -- empty out prior settings
    for k in pairs(ProfileTypes) do
      if type(obstable['Profile'..k])=='string' then
        ProgramPreferences.Profiles[k] = obstable['Profile'..k]
      end
    end
  end
end

return {
  useDefaults = useDefaults,
  changeProfile = changeProfile,
  checkProfile = checkProfile,
  StartDialog = StartDialog,
  EndDialog = EndDialog,
}
