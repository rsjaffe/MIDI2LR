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
  adjustPanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/BasicAdjustments=Basic Adjustments"),},
  book = {TMP = 'Module', friendlyName = LOC("$$$/Ag/Layout/Book/ModuleTitle=Book"),},
  calibratePanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/Calibration=Camera Calibration"),},
  circularGradient = {TMP = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/RadialFilters=Radial Filters"),},
  crop = {TMP = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Crop"),},
  detailPanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/Detail=Details")},
  develop = {TMP = 'Module', friendlyName = LOC("$$$/SmartCollection/Criteria/Heading/Develop=Develop"),},
  dust = {TMP = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/SpotRemoval=Spot Removal"),},
  effectsPanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/Effects=Effects"),},
  gradient = {TMP = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/GraduatedFilters=Graduated Filters"),},
  lensCorrectionsPanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/LensCorrections=Lens Corrections"),},
  library = {TMP = 'Module', friendlyName = LOC("$$$/AgLibraryModule/ModuleTitle=Library"),},
  localized = {TMP = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/BrushAdjustments=Brush Adjustments"),},
  loupe = {TMP = 'Tool', friendlyName = LOC("$$$/AgPhotoBin/ViewMode/Develop/Loupe=Loupe"),},
  map = {TMP = 'Module', friendlyName = LOC("$$$/AgLocation/ModuleTitle=Map"),},
  mixerPanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/ColorAdjustments=Color Adjustments"),},
  print = {TMP = 'Module', friendlyName = LOC("$$$/AgPrint/Menu/File/Print=Print"),},
  profile1 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 1',},
  profile10 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 10',},
  profile2 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 2',},
  profile3 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 3',},
  profile4 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 4',},
  profile5 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 5',},
  profile6 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 6',},
  profile7 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 7',},
  profile8 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 8',},
  profile9 = {TMP = 'Profile', friendlyName = LOC("$$$/AgDevelop/CameraRawPanel/Profile=Profile:")..' 9',},
  redeye = {TMP = 'Tool', friendlyName = LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Redeye=Red-eye Correction"),},
  slideshow = {TMP = 'Module', friendlyName = LOC("$$$/AgApplication/Menu/Window/SecondMonitor/Slideshow=Slideshow"),},
  splitToningPanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/SplitToning=Split Toning"),},
  tonePanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/ToneCurve=Tone Curve"),},
  transformPanel = {TMP = 'Panel', friendlyName = LOC("$$$/AgDevelop/Panel/Transform=Transform"),},
  web = {TMP = 'Module', friendlyName = LOC("$$$/WPG/Help/Shortcuts/WebHeader=Web"),},
}

return ProfileTypes