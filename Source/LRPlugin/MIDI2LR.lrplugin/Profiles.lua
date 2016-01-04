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

local defaults = {
  loupe = {ToolModulePanel = 'Tool', presetFile = '', friendlyName = LOC("$$$/AgPhotoBin/ViewMode/Develop/Loupe=Loupe"),},
  crop = {ToolModulePanel = 'Tool', presetFile = '', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Crop"),},
  dust = {ToolModulePanel = 'Tool', presetFile = '', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/SpotRemoval=Spot Removal"),},
  redeye = {ToolModulePanel = 'Tool', presetFile = '', friendlyName = LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Redeye=Red-eye Correction"},
  gradient = {ToolModulePanel = ''Tool, presetFile = '', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/GraduatedFilters=Graduated Filters"),},
  circularGradient = {ToolModulePanel = 'Tool', presetFile = '', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/RadialFilters=Radial Filters"),},
  localized = {ToolModulePanel = 'Tool', presetFile = '', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/BrushAdjustments=Brush Adjustments"),},
  adjustPanel = {ToolModulePanel = 'Panel', presetFile = '', friendlyName = LOC("$$$/AgDevelop/Panel/BasicAdjustments=Basic Adjustments"),},
  tonePanel = {ToolModulePanel = 'Panel', presetFile = '', friendlyName = LOC("$$$/AgDevelop/Panel/ToneCurve=Tone Curve"),},
  mixerPanel = {ToolModulePanel = 'Panel', presetFile = '', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/ColorAdjustments=Color Adjustments"),},
  splitToningPanel = {ToolModulePanel = 'Panel', presetFile = '', friendlyName = LOC("$$$/AgDevelop/Panel/SplitToning=Split Toning"),},
  detailPanel = {ToolModulePanel = 'Panel', presetFile = '', friendlyName = LOC("$$$/AgDevelop/Panel/Detail=Details")},
  lensCorrectionsPanel = {ToolModulePanel = 'Panel', presetFile = '', friendlyName = LOC("$$$/AgDevelop/Panel/LensCorrections=Lens Corrections"),},
  effectsPanel = {ToolModulePanel = 'Panel', presetFile = '', friendlyName = LOC("$$$/AgDevelop/Panel/Effects=Effects"),},
  calibratePanel = {ToolModulePanel = 'Panel', presetFile = '', friendlyName = LOC("$$$/AgDevelop/Panel/Calibration=Camera Calibration"),},
  library = {ToolModulePanel = 'Module', presetFile = '', friendlyName = LOC("$$$/AgLibraryModule/ModuleTitle=Library"),},
  develop = {ToolModulePanel = 'Module', presetFile = '', friendlyName = LOC("$$$/SmartCollection/Criteria/Heading/Develop=Develop"),},
  map = {ToolModulePanel = 'Module', presetFile = '', friendlyName = LOC("$$$/AgLocation/ModuleTitle=Map"),},
  book = {ToolModulePanel = 'Module', presetFile = '', friendlyName = LOC("$$$/Ag/Layout/Book/ModuleTitle=Book"),},
  slideshow = {ToolModulePanel = 'Module', presetFile = '', friendlyName = LOC("$$$/AgApplication/Menu/Window/SecondMonitor/Slideshow=Slideshow"),},
  print = {ToolModulePanel = 'Module', presetFile = '', friendlyName = LOC("$$$/AgPrint/Menu/File/Print=Print"),},
  web = {ToolModulePanel = 'Module', presetFile = '', friendlyName = LOC("$$$/WPG/Help/Shortcuts/WebHeader=Web"),},
  profile1 = {ToolModulePanel = '', presetFile = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 1',},
  profile2 = {ToolModulePanel = '', presetFile = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 2',},
  profile3 = {ToolModulePanel = '', presetFile = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 3',},
  profile4 = {ToolModulePanel = '', presetFile = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 4',},
  profile5 = {ToolModulePanel = '', presetFile = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 5',},
  profile6 = {ToolModulePanel = '', presetFile = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 6',},
  profile7 = {ToolModulePanel = '', presetFile = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 7',},
  profile8 = {ToolModulePanel = '', presetFile = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 8',},
  profile9 = {ToolModulePanel = '', presetFile = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 9',},
  profile10 = {ToolModulePanel = '', presetFile = '', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 10',},
}

local current {Tool = '', Module = '', Panel = ''}


local function useDefaults()
  ProgramPreferences.Profiles = {}
  for k,v in pairs(defaults) do
    ProgramPreferences.Profiles[k] = v
  end
end

local function changeProfile(profilename, ignoreCurrent)
  local changed = false
  if defaults[profilename] ~= null then
    local newprofile_file = ProgramPreferences.Profiles[profilename]['presetFile']
    local newToolModulePanel = ProgramPreferences.Profiles[profilename]['ToolModulePanel']
    if newprofile_file and 
    (newprofile_file ~= '') and 
    (ignoreCurrent == true or newToolModulePanel = '' or current[newToolModulePanel] ~= profilename) then
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
  if changeProfile(newmod) ~= true and newmod == 'develop' then
    changeProfile(getSelectedTool())
  end
end

local function StartDialog(obstable,f)
end

local function EndDialog(obstable, status)
  if status == 'ok' then
    useDefaults() -- empty out prior settings
    -- put other stuff here
  end
end

return {
  useDefaults = useDefaults,
  changeProfile = changeProfile,
  checkProfile = checkProfile,
  StartDialog = StartDialog,
  EndDialog = EndDialog,
}