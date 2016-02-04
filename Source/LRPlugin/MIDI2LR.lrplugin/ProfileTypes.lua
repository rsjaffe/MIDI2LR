--[[----------------------------------------------------------------------------

ProfileTypes.lua

Used in Profiles.lua and Init.lua
 
This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>. 
------------------------------------------------------------------------------]]

local ProfileTypes = {
  loupe = {TMP = 'Tool', friendlyName = LOC("$$$/AgPhotoBin/ViewMode/Develop/Loupe=Loupe"),},
  crop = {TMP = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Crop"),},
  dust = {TMP = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/SpotRemoval=Spot Removal"),},
  redeye = {TMP = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Redeye=Red-eye Correction"),},
  gradient = {TMP = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/GraduatedFilters=Graduated Filters"),},
  circularGradient = {TMP = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/RadialFilters=Radial Filters"),},
  localized = {TMP = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/BrushAdjustments=Brush Adjustments"),},
  adjustPanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/BasicAdjustments=Basic Adjustments"),},
  tonePanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/ToneCurve=Tone Curve"),},
  mixerPanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/ColorAdjustments=Color Adjustments"),},
  splitToningPanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/SplitToning=Split Toning"),},
  detailPanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/Detail=Details")},
  lensCorrectionsPanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/LensCorrections=Lens Corrections"),},
  effectsPanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/Effects=Effects"),},
  calibratePanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/Calibration=Camera Calibration"),},
  library = {TMP = 'Module', friendlyName = LOC("$$$/AgLibraryModule/ModuleTitle=Library"),},
  develop = {TMP = 'Module', friendlyName = LOC("$$$/SmartCollection/Criteria/Heading/Develop=Develop"),},
  map = {TMP = 'Module', friendlyName = LOC("$$$/AgLocation/ModuleTitle=Map"),},
  book = {TMP = 'Module', friendlyName = LOC("$$$/Ag/Layout/Book/ModuleTitle=Book"),},
  slideshow = {TMP = 'Module', friendlyName = LOC("$$$/AgApplication/Menu/Window/SecondMonitor/Slideshow=Slideshow"),},
  print = {TMP = 'Module', friendlyName = LOC("$$$/AgPrint/Menu/File/Print=Print"),},
  web = {TMP = 'Module', friendlyName = LOC("$$$/WPG/Help/Shortcuts/WebHeader=Web"),},
  profile1 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 1',},
  profile2 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 2',},
  profile3 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 3',},
  profile4 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 4',},
  profile5 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 5',},
  profile6 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 6',},
  profile7 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 7',},
  profile8 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 8',},
  profile9 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 9',},
  profile10 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 10',},
}

return ProfileTypes