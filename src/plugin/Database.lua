--[[----------------------------------------------------------------------------

Database.lua
Source of translations and menu for app and plugin

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

--[[----------------------------------------------------------------------------
unicode to UTF8 decimal mapping at http://www.utf8-chartable.de/unicode-utf8-table.pl
local interrobang = '\226\128\189'
local referencemark = '\226\128\187'
local ldquo = '\226\128\156'
local rdquo = '\226\128\157'
local lsquo = '\226\128\152'
local rsquo = '\226\128\153'
local mdash = '\226\128\148'

cmd \226\140\152
alt (2387) \226\142\135
opt (2325) 226 140 165
ctrl 2303 \226\140\131
shift (21e7) \226\135\167
tab (21e5) \226\135\165
left 1f850 \240\159\161\144
right 1f852 \240\159\161\146
up 1f851 \240\159\161\145
down 1f853 \240\159\161\147
enter 23ce \226\143\142
backspace 232b \226\140\171
delete 2326 \226\140\166

use st:gsub('%(%&%a%)',''):gsub('%&', '') to remove ampersands

Command ( string for command )
Type
  button just on/off
  parameter LR parameter
  variable needs slider value
  repeat sends repeated LR commands when twisting knob (see LR_IPC_Out.cpp)

Fields (Wraps, Repeats, experimental and panel are optional)
for wraps, would enter wrap = true for values that wrap around (e.g., color wheel)
for repeats, would enter repeats={cw_key,ccw_key} for what gets sent when turn right/left
Command Type Experimental Translation Group (in app) Explanation Panel (in LR Devel)
also optional are PV1 PV2 PV3 PV4 PV5. These change the bezel display for different process versions
Optional: AltParameter : define for those parameters in which
  LrApplication.activeCatalog():getTargetPhoto():getDevelopSettings() returns several relevant
  names (e.g., Clarity Clarity2012). also define as 'Direct' for those that should be retrieved
  by LrDevelopController.getValue (e.g., local adjustment parameters)
  -----------------------------------------------------------------------------]]

--local function LOC(str) return str end--for debugging
-- if update LatestPVSupported, update CmdTrans at end of this file to add the new PVs
local LatestPVSupported = 5 -- used for update PV to latest and for limits code to get best label

--Common terms used in database translated once here
--local coarse = LOC('$$$/AgPrint/CalibrationDialog/Coarse=Coarse')
--local fine = LOC('$$$/AgPrint/CalibrationDialog/Fine=Fine')
--local viewModes = LOC('$$$/AgDevelop/Toolbar/ViewModesTool=View Modes')
local P1and2 = 'PV 1'..LOC('$$$/AgStringUtils/localizedList/finalSeparatorString= and ')..'PV 2'
local P2and3plus = 'PV 2'..LOC('$$$/AgStringUtils/localizedList/finalSeparatorString= and ')..'PV 3+'
local brush = LOC('$$$/AgDevelop/Menu/View/AdjustmentBrush=Brush')
local developPreset = LOC('$$$/AgLibrary/Filter/BrowserCriteria/DevelopPreset/Single=Develop Preset')
local dgh = LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeHueLo=Defringe Green Hue')
local dph = LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeHueLo=Defringe Purple Hue')
local feather = LOC('$$$/AgDevelop/Localized/Feather=Feather')
local filtexp = '. See [Library Filters](https://github.com/rsjaffe/MIDI2LR/wiki/Plugin-Options-Dialog#library-filter) for more information.'
local key = LOC('$$$/MIDI2LR/Shortcuts/Key=Key')
local keyexp = '. See [Keyboard Shortcuts](https://github.com/rsjaffe/MIDI2LR/wiki/Plugin-Options-Dialog#keyboard-shortcuts) for more information.'
local keyword = LOC('$$$/AgLibrary/Filter/BrowserCriteria/Keyword=Keyword')
local keywordtoggle = LOC('$$$/MIDI2LR/Keyword/Toggle=Toggle Keyword')
local larger = LOC('$$$/AgEmail/FontSize/larger=larger')
local locadj = LOC('$$$/AgDevelop/Setting/LocalAdjustment=Local Adjustments')
local percor = LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/UprightPerspectiveCorrection=Perspective Correction')
local primaryDisplay = LOC('$$$/AgPhotoBin/Tooltip/PrimaryViewMode=Primary Display')
local profile = LOC('$$$/AgDevelop/CameraRawPanel/Profile=Profile:')
local quickdev = LOC('$$$/AgLibrary/Panel/DevelopTitle=Quick develop')
local repeatexp = ' Not suitable for controls with hard stops like faders. This command not usable in [Series of Commands](https://github.com/rsjaffe/MIDI2LR/wiki/Plugin-Options-Dialog#series-of-commands).'
local serexp = 'Sends a series of button commands. See [Series of Commands](https://github.com/rsjaffe/MIDI2LR/wiki/Plugin-Options-Dialog#series-of-commands) for more information.'
local size = LOC('$$$/AgDevelop/Toolbar/Localized/BrushSize=Size')
local smaller = LOC('$$$/AgEmail/FontSize/smaller=smaller')
local whiteBalance = LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/WhiteBalance=White Balance')

--Groups (but translation may be used for specific actions as well
--Listing the groups here makes it easier to understand layout, but doesn't change how the program works
--If the group was listed above this comment, it'd still work ok, but it would be more confusing to the reader
local keyshortcuts = LOC('$$$/AgLayoutShortcuts/Header/UISortcuts=Keyboard Shortcuts for User')
local commandseries = LOC('$$$/MIDI2LR/Shortcuts/SeriesofCommands=Command sequence')
local filter= LOC('$$$/Library/Filter/FilterLabel=Library filter'):gsub(' ?:','')
local view = LOC('$$$/AgLibrary/Help/Shortcuts/Header/ViewShortcuts=View')
local rating = LOC('$$$/AgLibrary/Help/Shortcuts/Header/RatingsShortcuts=Rating')
local flagging = LOC('$$$/AgLibrary/Help/Shortcuts/Header/FlaggingShortcuts=Flagging')
local photos = LOC('$$$/AgLibrary/Help/Shortcuts/Header/ImageShortcuts=Photos')
--local general = LOC('$$$/AgPreferences/TabView/General=General')
--local library = LOC('$$$/AgLibraryModule/ModuleTitle=Library')
local develop = LOC('$$$/SmartCollection/Criteria/Heading/Develop=Develop')
local basicTone = LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/BasicTone=Basic Tone')
local toneCurve = LOC('$$$/AgDevelop/CameraRawPanel/TargetName/ToneCurve=Tone Curve')
local colorAdjustments = LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/ColorAdjustments=Color Adjustments')
local resetColorAdjustments = LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',colorAdjustments)
local colorGrading = LOC('$$$/AgDevelop/Panel/ColorGrading=Color Grading')
local detail = LOC('$$$/AgDevelop/Panel/Detail=Detail')
local lensCorrections = LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/LensCorrections=Lens Corrections')
local locadjpre = LOC('$$$/MIDI2LR/LocalPresets/Presets=Local adjustments presets')
local transform = LOC('$$$/AgDevelop/CameraRawPanel/Transform=Transform')
local effects = LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Effects=Effects')
local calibration = LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Calibration=Calibration')
local developPresets = LOC('$$$/AgLibrary/Filter/BrowserCriteria/DevelopPreset/Plural=Develop Presets')
local keywords = LOC('$$$/AgLibrary/Filter/BrowserCriteria/Keywords=Keywords')
local mask = LOC('$$$/AgDevelop/LoupeView/Tool/MaskGroupTooltip=Masking')
local localizedAdjustments = LOC('$$$/AgDevelop/Setting/LocalAdjustment=Local Adjustments')
local localadjresets = LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj)
--local localpresets = locadjpre
local crop = LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Crop')
local gotoToolModulePanel = LOC('$$$/AgDialogs/Select=Select')..' '..LOC('$$$/AgDevelop/Menu/Tools=Tools'):gsub('%(%&%a%)',''):gsub('%&','')..LOC('$$$/AgStringUtils/localizedList/separatorString=, ')..LOC('$$$/Application/Menu/Window/Modules=Modules:'):gsub(' ?:','')..LOC('$$$/AgStringUtils/localizedList/finalSeparatorString= and ')..LOC('$$$/AgPreferences/Interface/GroupTitle/Panels=Panels')
local secondaryDisplay = LOC('$$$/AgApplication/Menu/Window/SecondaryDisplay=Secondary Display')
local profiles = LOC("$$$/CRaw/Style/ProfileGroup/Profiles=Profiles")

--[[----------------------------------------------------------------------------
  -----------------------------------------------------------------------------]]

local DataBase = {
  --Keyboard shortcuts
  {Command='Key1',Type='button',Translation=key..' 1',Group=keyshortcuts,Explanation='Key 1'..keyexp},
  {Command='Key2',Type='button',Translation=key..' 2',Group=keyshortcuts,Explanation='Key 2'..keyexp},
  {Command='Key3',Type='button',Translation=key..' 3',Group=keyshortcuts,Explanation='Key 3'..keyexp},
  {Command='Key4',Type='button',Translation=key..' 4',Group=keyshortcuts,Explanation='Key 4'..keyexp},
  {Command='Key5',Type='button',Translation=key..' 5',Group=keyshortcuts,Explanation='Key 5'..keyexp},
  {Command='Key6',Type='button',Translation=key..' 6',Group=keyshortcuts,Explanation='Key 6'..keyexp},
  {Command='Key7',Type='button',Translation=key..' 7',Group=keyshortcuts,Explanation='Key 7'..keyexp},
  {Command='Key8',Type='button',Translation=key..' 8',Group=keyshortcuts,Explanation='Key 8'..keyexp},
  {Command='Key9',Type='button',Translation=key..' 9',Group=keyshortcuts,Explanation='Key 9'..keyexp},
  {Command='Key10',Type='button',Translation=key..' 10',Group=keyshortcuts,Explanation='Key 10'..keyexp},
  {Command='Key11',Type='button',Translation=key..' 11',Group=keyshortcuts,Explanation='Key 11'..keyexp},
  {Command='Key12',Type='button',Translation=key..' 12',Group=keyshortcuts,Explanation='Key 12'..keyexp},
  {Command='Key13',Type='button',Translation=key..' 13',Group=keyshortcuts,Explanation='Key 13'..keyexp},
  {Command='Key14',Type='button',Translation=key..' 14',Group=keyshortcuts,Explanation='Key 14'..keyexp},
  {Command='Key15',Type='button',Translation=key..' 15',Group=keyshortcuts,Explanation='Key 15'..keyexp},
  {Command='Key16',Type='button',Translation=key..' 16',Group=keyshortcuts,Explanation='Key 16'..keyexp},
  {Command='Key17',Type='button',Translation=key..' 17',Group=keyshortcuts,Explanation='Key 17'..keyexp},
  {Command='Key18',Type='button',Translation=key..' 18',Group=keyshortcuts,Explanation='Key 18'..keyexp},
  {Command='Key19',Type='button',Translation=key..' 19',Group=keyshortcuts,Explanation='Key 19'..keyexp},
  {Command='Key20',Type='button',Translation=key..' 20',Group=keyshortcuts,Explanation='Key 20'..keyexp},
  {Command='Key21',Type='button',Translation=key..' 21',Group=keyshortcuts,Explanation='Key 21'..keyexp},
  {Command='Key22',Type='button',Translation=key..' 22',Group=keyshortcuts,Explanation='Key 22'..keyexp},
  {Command='Key23',Type='button',Translation=key..' 23',Group=keyshortcuts,Explanation='Key 23'..keyexp},
  {Command='Key24',Type='button',Translation=key..' 24',Group=keyshortcuts,Explanation='Key 24'..keyexp},
  {Command='Key25',Type='button',Translation=key..' 25',Group=keyshortcuts,Explanation='Key 25'..keyexp},
  {Command='Key26',Type='button',Translation=key..' 26',Group=keyshortcuts,Explanation='Key 26'..keyexp},
  {Command='Key27',Type='button',Translation=key..' 27',Group=keyshortcuts,Explanation='Key 27'..keyexp},
  {Command='Key28',Type='button',Translation=key..' 28',Group=keyshortcuts,Explanation='Key 28'..keyexp},
  {Command='Key29',Type='button',Translation=key..' 29',Group=keyshortcuts,Explanation='Key 29'..keyexp},
  {Command='Key30',Type='button',Translation=key..' 30',Group=keyshortcuts,Explanation='Key 30'..keyexp},
  {Command='Key31',Type='button',Translation=key..' 31',Group=keyshortcuts,Explanation='Key 31'..keyexp},
  {Command='Key32',Type='button',Translation=key..' 32',Group=keyshortcuts,Explanation='Key 32'..keyexp},
  {Command='Key33',Type='button',Translation=key..' 33',Group=keyshortcuts,Explanation='Key 33'..keyexp},
  {Command='Key34',Type='button',Translation=key..' 34',Group=keyshortcuts,Explanation='Key 34'..keyexp},
  {Command='Key35',Type='button',Translation=key..' 35',Group=keyshortcuts,Explanation='Key 35'..keyexp},
  {Command='Key36',Type='button',Translation=key..' 36',Group=keyshortcuts,Explanation='Key 36'..keyexp},
  {Command='Key37',Type='button',Translation=key..' 37',Group=keyshortcuts,Explanation='Key 37'..keyexp},
  {Command='Key38',Type='button',Translation=key..' 38',Group=keyshortcuts,Explanation='Key 38'..keyexp},
  {Command='Key39',Type='button',Translation=key..' 39',Group=keyshortcuts,Explanation='Key 39'..keyexp},
  {Command='Key40',Type='button',Translation=key..' 40',Group=keyshortcuts,Explanation='Key 40'..keyexp},
  {Command='Key2Key1',Type='repeat',Translation=key..' 2 – '..key..' 1',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key2 signals to Lightroom, counterclockwise Key1.'..repeatexp,Repeats={'Key2','Key1'}},
  {Command='Key4Key3',Type='repeat',Translation=key..' 4 – '..key..' 3',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key4 signals to Lightroom, counterclockwise Key3.'..repeatexp,Repeats={'Key4','Key3'}},
  {Command='Key6Key5',Type='repeat',Translation=key..' 6 – '..key..' 5',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key6 signals to Lightroom, counterclockwise Key5.'..repeatexp,Repeats={'Key6','Key5'}},
  {Command='Key8Key7',Type='repeat',Translation=key..' 8 – '..key..' 7',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key8 signals to Lightroom, counterclockwise Key7.'..repeatexp,Repeats={'Key8','Key7'}},
  {Command='Key10Key9',Type='repeat',Translation=key..' 10 – '..key..' 9',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key10 signals to Lightroom, counterclockwise Key9.'..repeatexp,Repeats={'Key10','Key9'}},
  {Command='Key12Key11',Type='repeat',Translation=key..' 12 – '..key..' 11',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key12 signals to Lightroom, counterclockwise Key11.'..repeatexp,Repeats={'Key12','Key11'}},
  {Command='Key14Key13',Type='repeat',Translation=key..' 14 – '..key..' 13',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key14 signals to Lightroom, counterclockwise Key13.'..repeatexp,Repeats={'Key14','Key13'}},
  {Command='Key16Key15',Type='repeat',Translation=key..' 16 – '..key..' 15',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key16 signals to Lightroom, counterclockwise Key15.'..repeatexp,Repeats={'Key16','Key15'}},
  {Command='Key18Key17',Type='repeat',Translation=key..' 18 – '..key..' 17',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key18 signals to Lightroom, counterclockwise Key17.'..repeatexp,Repeats={'Key18','Key17'}},
  {Command='Key20Key19',Type='repeat',Translation=key..' 20 – '..key..' 19',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key20 signals to Lightroom, counterclockwise Key19.'..repeatexp,Repeats={'Key20','Key19'}},
  {Command='Key22Key21',Type='repeat',Translation=key..' 22 – '..key..' 21',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key22 signals to Lightroom, counterclockwise Key21.'..repeatexp,Repeats={'Key22','Key21'}},
  {Command='Key24Key23',Type='repeat',Translation=key..' 24 – '..key..' 23',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key24 signals to Lightroom, counterclockwise Key23.'..repeatexp,Repeats={'Key24','Key23'}},
  {Command='Key26Key25',Type='repeat',Translation=key..' 26 – '..key..' 25',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key26 signals to Lightroom, counterclockwise Key25.'..repeatexp,Repeats={'Key26','Key25'}},
  {Command='Key28Key27',Type='repeat',Translation=key..' 28 – '..key..' 27',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key28 signals to Lightroom, counterclockwise Key27.'..repeatexp,Repeats={'Key28','Key27'}},
  {Command='Key30Key29',Type='repeat',Translation=key..' 30 – '..key..' 29',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key30 signals to Lightroom, counterclockwise Key29.'..repeatexp,Repeats={'Key30','Key29'}},
  {Command='Key32Key31',Type='repeat',Translation=key..' 32 – '..key..' 31',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key32 signals to Lightroom, counterclockwise Key31.'..repeatexp,Repeats={'Key32','Key31'}},
  {Command='Key34Key33',Type='repeat',Translation=key..' 34 – '..key..' 33',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key34 signals to Lightroom, counterclockwise Key33.'..repeatexp,Repeats={'Key34','Key33'}},
  {Command='Key36Key35',Type='repeat',Translation=key..' 36 – '..key..' 35',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key36 signals to Lightroom, counterclockwise Key35.'..repeatexp,Repeats={'Key36','Key35'}},
  {Command='Key38Key37',Type='repeat',Translation=key..' 38 – '..key..' 37',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key38 signals to Lightroom, counterclockwise Key37.'..repeatexp,Repeats={'Key38','Key37'}},
  {Command='Key40Key39',Type='repeat',Translation=key..' 40 – '..key..' 39',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key40 signals to Lightroom, counterclockwise Key39.'..repeatexp,Repeats={'Key40','Key39'}},
  --command series
  {Command='ActionSeries1',Type='button',Translation=commandseries..' 1',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries2',Type='button',Translation=commandseries..' 2',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries3',Type='button',Translation=commandseries..' 3',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries4',Type='button',Translation=commandseries..' 4',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries5',Type='button',Translation=commandseries..' 5',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries6',Type='button',Translation=commandseries..' 6',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries7',Type='button',Translation=commandseries..' 7',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries8',Type='button',Translation=commandseries..' 8',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries9',Type='button',Translation=commandseries..' 9',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries10',Type='button',Translation=commandseries..' 10',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries11',Type='button',Translation=commandseries..' 11',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries12',Type='button',Translation=commandseries..' 12',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries13',Type='button',Translation=commandseries..' 13',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries14',Type='button',Translation=commandseries..' 14',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries15',Type='button',Translation=commandseries..' 15',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries16',Type='button',Translation=commandseries..' 16',Group=commandseries,Explanation=serexp},
  --library filters
  {Command='Filter_1',Type='button',Translation=filter..' 1',Group=filter,Explanation='Library filter 1'..filtexp},
  {Command='Filter_2',Type='button',Translation=filter..' 2',Group=filter,Explanation='Library filter 2'..filtexp},
  {Command='Filter_3',Type='button',Translation=filter..' 3',Group=filter,Explanation='Library filter 3'..filtexp},
  {Command='Filter_4',Type='button',Translation=filter..' 4',Group=filter,Explanation='Library filter 4'..filtexp},
  {Command='Filter_5',Type='button',Translation=filter..' 5',Group=filter,Explanation='Library filter 5'..filtexp},
  {Command='Filter_6',Type='button',Translation=filter..' 6',Group=filter,Explanation='Library filter 6'..filtexp},
  {Command='Filter_7',Type='button',Translation=filter..' 7',Group=filter,Explanation='Library filter 7'..filtexp},
  {Command='Filter_8',Type='button',Translation=filter..' 8',Group=filter,Explanation='Library filter 8'..filtexp},
  {Command='Filter_9',Type='button',Translation=filter..' 9',Group=filter,Explanation='Library filter 9'..filtexp},
  {Command='Filter_10',Type='button',Translation=filter..' 10',Group=filter,Explanation='Library filter 10'..filtexp},
  {Command='Filter_11',Type='button',Translation=filter..' 11',Group=filter,Explanation='Library filter 11'..filtexp},
  {Command='Filter_12',Type='button',Translation=filter..' 12',Group=filter,Explanation='Library filter 12'..filtexp},
  {Command='FilterNone',Type='button',Translation=LOC('$$$/AgLibrary/FilterPresets/FiltersOff=Filters off'),Group=filter,Explanation='Library filters off.'},
  --view
  {Command='SwToMlibrary',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',LOC('$$$/AgLibraryModule/ModuleTitle=Library')),Group=view,Explanation='Switch to Library module.'},
  {Command='ShoVwpeople',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgPhotoBin/ViewMode/Library/People=People'),Group=view,Explanation='In the People view, the different faces are organized by people stacks.'},
  {Command='ShoVwgrid',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgPhotoBin/ViewMode/Library/Grid=Grid'),Group=view,Explanation='Displays photos as thumbnails in cells, which can be viewed in compact and expanded sizes.'},
  {Command='GridViewStyle',Type='button',Translation=LOC('$$$/AgLibrary/Help/Shortcuts/CycleGridViews=Cycle grid views'),Group=view,Explanation='Changes the grid view style.'},
  {Command='ShoVwloupe',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgPhotoBin/ViewMode/Library/Loupe=Loupe'),Group=view,Explanation='Displays a single photo. Zoom levels up to 11:1 are available.'},
  {Command='CycleLoupeViewInfo',Type='button',Translation=LOC('$$$/AgLibrary/Menu/View/LoupeOverlay=Loupe')..' '..LOC('$$$/AgLibrary/Menu/View/LoupeViewInfo/CycleViewStyle=Cycle view style'):gsub('%(%&%a%)',''):gsub('%&',''),Group=view,Explanation='Changes the Loupe View Info style. Only works in Library and Develop modules.'},
  {Command='ShoVwcompare',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgPhotoBin/ViewMode/Library/Compare=Compare'),Group=view,Explanation='Displays photos side by side so that you can evaluate them.'},
  {Command='ShoVwsurvey',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgPhotoBin/ViewMode/Library/Survey=Survey'),Group=view,Explanation='Displays the active photo with selected photos so that you can evaluate them. The active photo has the lightest colored cell.'},
  {Command='ShoFullHidePanels',Type='button',Translation=LOC('$$$/Application/Menu/Window/FullScreenHidePanels=Full screen, hide panels'):gsub('%(%&%a%)',''):gsub('%&',''),Group=view,Explanation='Changes the screen mode to Full Screen and Hide Panels.'},
  {Command='ShoFullPreview',Type='button',Translation=LOC('$$$/AgPrintShortcuts/Full_screen_modes=Full screen preview'),Group=view,Explanation='Changes the screen mode to Full Screen Preview.'},
  {Command='NextScreenMode',Type='button',Translation=LOC('$$$/AgLibrary/Help/Shortcuts/CycleToNextScreenMode=Cycle to next screen mode'),Group=view,Explanation='Changes to the next screen mode.'},
  {Command='ToggleLoupe',Type='button',Translation=LOC('$$$/AgLibrary/Menu/View/ToggleLoupeView=Loupe view activate/inactivate'):gsub('%(%&%a%)',''):gsub('%&',''),Group=view,Explanation='Toggle loupe view while in Library.'},
  {Command='ToggleZoomOffOn',Type='button',Translation=LOC('$$$/AgLibrary/Menu/View/ToggleZoomView=Enable/Disable Zoom'):gsub('%(%&%a%)',''):gsub('%&',''),Group=view,Explanation=''},
  {Command='ZoomInOut',Type='repeat',Translation=LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomIn=Zoom In')..' '..LOC('$$$/ImportView/Less=Less')..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomOut=Zoom Out')..' '..LOC('$$$/ImportView/Less=Less'),Group=view,Explanation='Turning knob clockwise zooms in, counterclockwise zooms out.'..repeatexp,Repeats={'ZoomInSmallStep','ZoomOutSmallStep'}},
  {Command='ZoomOutIn',Type='repeat',Translation=LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomOut=Zoom Out')..' '..LOC('$$$/ImportView/Less=Less')..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomIn=Zoom In')..' '..LOC('$$$/ImportView/Less=Less'),Group=view,Explanation='Turning knob clockwise zooms out, counterclockwise zooms in.'..repeatexp,Repeats={'ZoomOutSmallStep','ZoomInSmallStep'}},
  {Command='ZoomInLargeStep',Type='button',Translation=LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomIn=Zoom In'),Group=view,Explanation=''},
  {Command='ZoomInSmallStep',Type='button',Translation=LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomIn=Zoom In')..' '..LOC('$$$/ImportView/Less=Less'),Group=view,Explanation=''},
  {Command='ZoomOutSmallStep',Type='button',Translation=LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomOut=Zoom Out')..' '..LOC('$$$/ImportView/Less=Less'),Group=view,Explanation=''},
  {Command='ZoomOutLargeStep',Type='button',Translation=LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomOut=Zoom Out'),Group=view,Explanation=''},
  {Command='ZoomTo100',Type='button',Experimental=true,Translation=LOC('$$$/AgLibrary/Menu/View/ZoomTo100=Zoom to 100%'),Group=view,Explanation=''},
--rating
  {Command='SetRating0',Type='button',Translation=LOC('$$$/AgLibrary/Filter/Stars=^1 Stars','0'),Group=rating,Explanation=''},
  {Command='SetRating1',Type='button',Translation=LOC('$$$/$$$/AgLibrary/Filter/BrowserCriteria/Rating/Singular=1 star'),Group=rating,Explanation=''},
  {Command='SetRating2',Type='button',Translation=LOC('$$$/$$$/AgLibrary/Filter/BrowserCriteria/Rating/Plural=^1 stars',2),Group=rating,Explanation=''},
  {Command='SetRating3',Type='button',Translation=LOC('$$$/$$$/AgLibrary/Filter/BrowserCriteria/Rating/Plural=^1 stars',3),Group=rating,Explanation=''},
  {Command='SetRating4',Type='button',Translation=LOC('$$$/$$$/AgLibrary/Filter/BrowserCriteria/Rating/Plural=^1 stars',4),Group=rating,Explanation=''},
  {Command='SetRating5',Type='button',Translation=LOC('$$$/$$$/AgLibrary/Filter/BrowserCriteria/Rating/Plural=^1 stars',5),Group=rating,Explanation=''},
  {Command='IncreaseRating',Type='button',Translation=LOC('$$$/AgLibrary/Ops/IncreaseRating=Increase Rating'),Group=rating,Explanation=''},
  {Command='DecreaseRating',Type='button',Translation=LOC('$$$/AgLibrary/Ops/DecreaseRating=Decrease Rating'),Group=rating,Explanation=''},
  {Command='IncreaseDecreaseRating',Type='button',Translation=LOC('$$$/AgLibrary/Ops/IncreaseRating=Increase Rating')..' — '..LOC('$$$/AgLibrary/Ops/DecreaseRating=Decrease Rating'),Group=rating,Explanation='Turning knob clockwise increases the star rating, counterclockwise decreases it.'..repeatexp,Repeats={'IncreaseRating','DecreaseRating'}},
  {Command='SetRating',Type='variable',Translation=LOC('$$$/AgDevelop/Toolbar/Tooltip/SetRating=Set Rating'),Group=rating,Explanation='Unlike *Increase Rating \226\128\148 Decrease Rating*, this relies on the absolute position of the control to set the rating. Best suited for faders. Suggest using *Increase Rating \226\128\148 Decrease Rating* for rotary controls. Note that MIDI2LR does not synchronize the control with the current photo\226\128\153s rating and does not use pickup mode for this control, so the rating will immediately change to the control value when the control is moved, even if the control rating is far from the photo\226\128\153s current rating.'},
  --flagging
  {Command='Pick',Type='button',Translation=LOC('$$$/AgLibrary/Help/Shortcuts/SetPick=Set Pick Flag'),Group=flagging,Explanation=''},
  {Command='Reject',Type='button',Translation=LOC('$$$/AgLibrary/Help/Shortcuts/SetReject=Set Rejected Flag'),Group=flagging,Explanation=''},
  {Command='ToggleFlag',Type='button',Translation=LOC('$$$/AgLibrary/Help/Shortcuts/ToggleFlag=Toggle Flag'),Group=flagging,Explanation=''},
  {Command='RemoveFlag',Type='button',Translation=LOC('$$$/AgLibrary/Filter/BrowserCriteria/Flag/Unflagged=Unflagged'),Group=flagging,Explanation=''},
  {Command='ToggleBlue',Type='button',Translation=LOC('$$$/AgLibrary/Undo/ToggleColorLabel=Label ^1 Enable/Disable',LOC('$$$/LibraryImporter/ColorLabelBlue=Blue')),Group=flagging,Explanation=''},
  {Command='ToggleGreen',Type='button',Translation=LOC('$$$/AgLibrary/Undo/ToggleColorLabel=Label ^1 Enable/Disable',LOC('$$$/LibraryImporter/ColorLabelGreen=Green')),Group=flagging,Explanation=''},
  {Command='ToggleRed',Type='button',Translation=LOC('$$$/AgLibrary/Undo/ToggleColorLabel=Label ^1 Enable/Disable',LOC('$$$/LibraryImporter/ColorLabelRed=Red')),Group=flagging,Explanation=''},
  {Command='TogglePurple',Type='button',Translation=LOC('$$$/AgLibrary/Undo/ToggleColorLabel=Label ^1 Enable/Disable',LOC('$$$/LibraryImporter/ColorLabelPurple=Purple')),Group=flagging,Explanation=''},
  {Command='ToggleYellow',Type='button',Translation=LOC('$$$/AgLibrary/Undo/ToggleColorLabel=Label ^1 Enable/Disable',LOC('$$$/LibraryImporter/ColorLabelYellow=Yellow')),Group=flagging,Explanation=''},
  {Command='ColorLabelNone',Type='button',Translation=LOC('$$$/AgLibrary/LabelCommands/UndoName/RemovedColorLabel=Remove color label'),Group=flagging,Explanation=''},
  --photos
  {Command='AddOrRemoveFromTargetColl',Type='button',Translation=LOC('$$$/Help/Shortcuts/AddToTargetCollection=Add to the target collection'),Group=photos,Explanation='Adds all selected photos to target collection. If the photos are already in the target collection, removes them from it.'},
  {Command='EditPhotoshop',Type='button',Translation=LOC('$$$/AgDevelopShortcuts/Edit_in_Photoshop=Edit in Photoshop'),Group=photos,Explanation='Edit the current photo in Photoshop.'},
  {Command='openExportDialog',Type='button',Translation=LOC('$$$/AgLibrary/Panel/ExportButtonTitle=Export...'),Group=photos,Explanation='Opens export dialog for all selected photos.'},
  {Command='openExportWithPreviousDialog',Type='button',Translation=LOC('$$$/AgLibrary/Menu/Export/ExportAgain=Export again'):gsub('%(%&%a%)',''):gsub('%&',''),Group=photos,Explanation='Exports with previous settings for all selected photos.'},
  {Command='SelectRightLeft',Type='repeat',Translation=LOC('$$$/AgLibrary/Menu/Edit/AddToSelection=Add to Selection')..' '..LOC('$$$/AgWatermarking/Alignment/Left=Left')..' '..LOC('$$$/AgWatermarking/Alignment/Right=Right'),Group=photos,Explanation='Extend selection to right or left. Turning knob clockwise sends select Right signals to Lightroom, counterclockwise select Left.'..repeatexp,Repeats={'Select1Right','Select1Left'}},
  {Command='Select1Left',Type='button',Translation=LOC('$$$/AgLibrary/Menu/Edit/AddToSelection=Add to Selection')..' '..LOC('$$$/AgWatermarking/Alignment/Left=Left'),Group=photos,Explanation='Extend selection one picture to the left.'},
  {Command='Select1Right',Type='button',Translation=LOC('$$$/AgLibrary/Menu/Edit/AddToSelection=Add to Selection')..' '..LOC('$$$/AgWatermarking/Alignment/Right=Right'),Group=photos,Explanation='Extend selection one picture to the right.'},
  {Command='NextPrev',Type='repeat',Translation=LOC('$$$/AgDevelopShortcuts/Next_Photo=Next Photo')..' — '..LOC('$$$/AgDevelopShortcuts/Previous_Photo=Previous Photo'),Group=photos,Explanation='Move to next or previous photo. Turning knob clockwise sends Next signals to Lightroom, counterclockwise Previous.'..repeatexp,Repeats={'Next','Prev'}},
  {Command='Next',Type='button',Translation=LOC('$$$/AgDevelopShortcuts/Next_Photo=Next Photo'),Group=photos,Explanation=''},
  {Command='Prev',Type='button',Translation=LOC('$$$/AgDevelopShortcuts/Previous_Photo=Previous Photo'),Group=photos,Explanation=''},
  {Command='RotateLeft',Type='button',Translation=LOC('$$$/AgDevelopShortcuts/Rotate_left=Rotate left'),Group=photos,Explanation='Rotates all selected photos left.'},
  {Command='RotateRight',Type='button',Translation=LOC('$$$/AgDevelopShortcuts/Rotate_right=Rotate right'),Group=photos,Explanation='Rotates all selected photos right.'},
  {Command='FullRefresh',Type='button',Translation=LOC('$$$/AgLibrary/ViewBar/Sort/RefreshMode/Manual=Manual Update'),Group=photos,Explanation='Force an update of all develop settings in MIDI controller, even if MIDI2LR believes MIDI controller is up to date. Useful if controller out of sync with Lightroom (e.g., with layer changes).'},
  {Command='CloseApp',Type='button',Translation=LOC('$$$/AgPluginManager/Status/HttpServer/StopServer=Stop Server'),Group=photos,Explanation='Closes the MIDI2LR application.'},
  --quick develop
  {Command='QuickDevTempLarge',Type='button',Translation=quickdev..' '..LOC('$$$/AgCameraRawController/TargetAdjustment/Increase=Increase ^1: ^2',LOC('$$$/AgCameraRawUI/Temp=Temperature'),'20'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Temperature by 20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevTempLargeDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgCameraRawController/TargetAdjustment/Decrease=Decrease ^1: ^2',LOC('$$$/AgCameraRawUI/Temp=Temperature'),'-20'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Temperature by -20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevTempSmall',Type='button',Translation=quickdev..' '..LOC('$$$/AgCameraRawController/TargetAdjustment/Increase=Increase ^1: ^2',LOC('$$$/AgCameraRawUI/Temp=Temperature'),'5'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Temperature by 5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevTempSmalDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgCameraRawController/TargetAdjustment/Decrease=Decrease ^1: ^2',LOC('$$$/AgCameraRawUI/Temp=Temperature'),'-5'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Temperature by -5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevTempAdj',Type='repeat',Translation=quickdev..' '..LOC('$$$/AgCameraRawUI/Temp=Temperature'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, turning knob clockwise increases Temperature, counterclockwise decreases Temperature for all selected photos. Works in Library and Develop modules.'..repeatexp,Repeats={'QuickDevTempSmall','QuickDevTempSmalDec'}},
  {Command='QuickDevTintLarge',Type='button',Translation=quickdev..' '..LOC('$$$/AgCameraRawController/TargetAdjustment/Increase=Increase ^1: ^2',LOC('$$$/AgCameraRawUI/Tint=Tint'),'20'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Tint by 20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevTintLargeDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgCameraRawController/TargetAdjustment/Decrease=Decrease ^1: ^2',LOC('$$$/AgCameraRawUI/Tint=Tint'),'-20'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Tint by -20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevTintSmall',Type='button',Translation=quickdev..' '..LOC('$$$/AgCameraRawController/TargetAdjustment/Increase=Increase ^1: ^2',LOC('$$$/AgCameraRawUI/Tint=Tint'),'5'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Tint by 5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevTintSmallDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgCameraRawController/TargetAdjustment/Decrease=Decrease ^1: ^2',LOC('$$$/AgCameraRawUI/Tint=Tint'),'-5'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Tint by -5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevTintAdj',Type='repeat',Translation=quickdev..' '..LOC('$$$/AgCameraRawUI/Tint=Tint'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, turning knob clockwise increases Tint, counterclockwise decreases Tint for all selected photos. Works in Library and Develop modules.'..repeatexp,Repeats={'QuickDevTintSmall','QuickDevTintSmallDec'}},
  {Command='QuickDevExpLarge',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseExposureLg=Increase exposure 1 stop'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Exposure by 1 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevExpLargeDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseExposureLg=Decrease exposure 1 stop'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Exposure by -1 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevExpSmall',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseExposureSm=Increase exposure 1/3 stop'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Exposure by 0.33 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevExpSmallDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseExposureSm=Decrease exposure 1/3 stop'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Exposure by -0.33 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevExpAdj',Type='repeat',Translation=quickdev..' '..LOC('$$$/AgCameraRawUI/Exposure=Exposure'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, turning knob clockwise increases Exposure, counterclockwise decreases Exposure for all selected photos. Works in Library and Develop modules.'..repeatexp,Repeats={'QuickDevExpSmall','QuickDevExpSmallDec'}},
  {Command='QuickDevContrastLarge',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseContrast=Increase contrast')..' 20',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Contrast by 20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevContrastLargeDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseContrast=Decrease contrast')..' -20',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Contrast by -20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevContrastSmall',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseContrast=Increase contrast')..' 5',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Contrast by 5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevContrastSmallDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseContrast=Decrease contrast')..' -5',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Contrast by -5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevContrastAdj',Type='repeat',Translation=quickdev..' '..LOC('$$$/AgCameraRawUI/Contrast=Contrast'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, turning knob clockwise increases Contrast, counterclockwise decreases Contrast for all selected photos. Works in Library and Develop modules.'..repeatexp,Repeats={'QuickDevContrastSmall','QuickDevContrastSmallDec'}},
  {Command='QuickDevHighlightsLarge',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseHighlights=Increase highlights')..' 20',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Highlights by 20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevHighlightsLargeDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseHighlights=Decrease highlights')..' -20',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Highlights by -20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevHighlightsSmall',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseHighlights=Increase highlights')..' 5',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Highlights by 5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevHighlightsSmallDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseHighlights=Decrease highlights')..' -5',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Highlights by -5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevHighlightsAdj',Type='repeat',Translation=quickdev..' '..LOC('$$$/AgCameraRawUI/Highlights=Highlights'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, turning knob clockwise increases Highlights, counterclockwise decreases Highlights for all selected photos. Works in Library and Develop modules.'..repeatexp,Repeats={'QuickDevHighlightsSmall','QuickDevHighlightsSmallDec'}},
  {Command='QuickDevShadowsLarge',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseShadows=Decrease shadows')..' 20',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Shadows by 20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevShadowsLargeDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseShadows=Increase shadows')..' -20',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Shadows by -20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevShadowsSmall',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseShadows=Decrease shadows')..' 5',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Shadows by 5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevShadowsSmallDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseShadows=Increase shadows')..' -5',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Shadows by -5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevShadowsAdj',Type='repeat',Translation=quickdev..' '..LOC('$$$/AgCameraRawUI/Shadows=Shadows'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, turning knob clockwise increases Shadows, counterclockwise decreases Shadows for all selected photos. Works in Library and Develop modules.'..repeatexp,Repeats={'QuickDevShadowsSmall','QuickDevShadowsSmallDec'}},
  {Command='QuickDevWhitesLarge',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseWhiteClipping=Increase white clipping')..' 20',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Whites by 20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevWhitesLargeDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseWhiteClipping=Decrease white clipping')..' -20',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Whites by -20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevWhitesSmall',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseWhiteClipping=Increase white clipping')..' 5',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Whites by 5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevWhitesSmallDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseWhiteClipping=Decrease white clipping')..' -5',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Whites by -5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevWhitesAdj',Type='repeat',Translation=quickdev..' '..LOC('$$$/AgCameraRawUI/Whites=Whites'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, turning knob clockwise increases Whites, counterclockwise decreases Whites for all selected photos. Works in Library and Develop modules.'..repeatexp,Repeats={'QuickDevWhitesSmall','QuickDevWhitesSmallDec'}},
  {Command='QuickDevBlacksLarge',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseBlackClipping=Decrease black clipping')..' 20',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Blacks by 20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevBlacksLargeDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseBlackClipping=Increase black clipping')..' -20',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Blacks by -20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevBlacksSmall',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseBlackClipping=Decrease black clipping')..' 5',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Blacks by 5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevBlacksSmallDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseBlackClipping=Increase black clipping')..' -5',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Blacks by -5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevBlacksAdj',Type='repeat',Translation=quickdev..' '..LOC('$$$/AgCameraRawUI/Blacks=Blacks'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, turning knob clockwise increases Blacks, counterclockwise decreases Blacks for all selected photos. Works in Library and Develop modules.'..repeatexp,Repeats={'QuickDevBlacksSmall','QuickDevBlacksSmallDec'}},
  {Command='QuickDevClarityLarge',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseClarity=Increase clarity')..' 20',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Clarity by 20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevClarityLargeDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseClarity=Decrease clarity')..' -20',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Clarity by -20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevClaritySmall',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseClarity=Increase clarity')..' 5',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Clarity by 5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevClaritySmallDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseClarity=Decrease clarity')..' -5',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Clarity by -5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevClarityAdj',Type='repeat',Translation=quickdev..' '..LOC('$$$/AgCameraRawUI/Clarity=Clarity'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, turning knob clockwise increases Clarity, counterclockwise decreases Clarity for all selected photos. Works in Library and Develop modules.'..repeatexp,Repeats={'QuickDevClaritySmall','QuickDevClaritySmallDec'}},
  {Command='QuickDevVibranceLarge',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseVibrance=Increase vibrance')..' 20',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Vibrance by 20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevVibranceLargeDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseVibrance=Decrease vibrance')..' -20',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Vibrance by -20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevVibranceSmall',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/IncreaseVibrance=Increase vibrance')..' 5',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Vibrance by 5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevVibranceSmallDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/MiniDevelop/DecreaseVibrance=Decrease vibrance')..' -5',Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Vibrance by -5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevVibranceAdj',Type='repeat',Translation=quickdev..' '..LOC('$$$/AgCameraRawUI/Vibrance=Vibrance'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, turning knob clockwise increases Vibrance, counterclockwise decreases Vibrance for all selected photos. Works in Library and Develop modules.'..repeatexp,Repeats={'QuickDevVibranceSmall','QuickDevVibranceSmallDec'}},
  {Command='QuickDevSatLarge',Type='button',Translation=quickdev..' '..LOC('$$$/AgCameraRawController/TargetAdjustment/Increase=Increase ^1: ^2',LOC('$$$/AgCameraRawUI/Saturation=Saturation'),'20'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Saturation by 20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevSatLargeDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgCameraRawController/TargetAdjustment/Decrease=Decrease ^1: ^2',LOC('$$$/AgCameraRawUI/Saturation=Saturation'),'-20'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Saturation by -20 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevSatSmall',Type='button',Translation=quickdev..' '..LOC('$$$/AgCameraRawController/TargetAdjustment/Increase=Increase ^1: ^2',LOC('$$$/AgCameraRawUI/Saturation=Saturation'),'5'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Saturation by 5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevSatSmallDec',Type='button',Translation=quickdev..' '..LOC('$$$/AgCameraRawController/TargetAdjustment/Decrease=Decrease ^1: ^2',LOC('$$$/AgCameraRawUI/Saturation=Saturation'),'-5'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, adjusts Saturation by -5 for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevSatAdj',Type='repeat',Translation=quickdev..' '..LOC('$$$/AgCameraRawUI/Saturation=Saturation'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, turning knob clockwise increases Saturation, counterclockwise decreases Saturation for all selected photos. Works in Library and Develop modules.'..repeatexp,Repeats={'QuickDevSatSmall','QuickDevSatSmallDec'}},
  {Command='QuickDevWBAuto',Type='button',Translation=quickdev..' '..whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Auto=Auto'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets auto white balance for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevWBDaylight',Type='button',Translation=quickdev..' '..whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Daylight=Daylight'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets daylight white balance for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevWBCloudy',Type='button',Translation=quickdev..' '..whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Cloudy=Cloudy'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets cloudy white balance for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevWBShade',Type='button',Translation=quickdev..' '..whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Shade=Shade'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets shade white balance for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevWBTungsten',Type='button',Translation=quickdev..' '..whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Tungsten=Tungsten'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets tungsten white balance for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevWBFluorescent',Type='button',Translation=quickdev..' '..whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Fluorescent=Fluorescent'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets fluorescent white balance for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevWBFlash',Type='button',Translation=quickdev..' '..whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Flash=Flash'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets Auto flash balance for all selected photos. Works in Library and Develop modules.'},
  {Command='SetTreatmentBW',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/SetGrayscale=Set treatment B&W'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets the treatment to B&W for all selected photos. Works in Library and Develop modules.'},
  {Command='SetTreatmentColor',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/SetColor=Set treatment Color'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets the treatment to Color for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevCropAspectOriginal',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/SetCropAspect=Set crop ratio: ^1',LOC('$$$/AgLibrary/CameraRawView/Ops/CropName/Original=Original')),Group=quickdev,Explanation='Set crop ratio to original. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevCropAspectAsShot',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/SetCropAspect=Set crop ratio: ^1',LOC('$$$/AgLibrary/CameraRawView/Ops/CropName/AsShot=As shot')),Group=quickdev,Explanation='Set crop ratio to as shot. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevCropAspect1x1',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/SetCropAspect=Set crop ratio: ^1',LOC('$$$/AgLibrary/CameraRawView/Ops/CropNameConstruction=^1 x ^2','1','1')),Group=quickdev,Explanation='Set crop ratio to 1 by 1. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevCropAspect2x3',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/SetCropAspect=Set crop ratio: ^1',LOC('$$$/AgLibrary/CameraRawView/Ops/CropNameConstruction=^1 x ^2','2','3')),Group=quickdev,Explanation='Set crop ratio to 2 by 3. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevCropAspect3x4',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/SetCropAspect=Set crop ratio: ^1',LOC('$$$/AgLibrary/CameraRawView/Ops/CropNameConstruction=^1 x ^2','3','4')),Group=quickdev,Explanation='Set crop ratio to 3 by 4. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevCropAspect4x5',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/SetCropAspect=Set crop ratio: ^1',LOC('$$$/AgLibrary/CameraRawView/Ops/CropNameConstruction=^1 x ^2','4','5')),Group=quickdev,Explanation='Set crop ratio to 4 by 5. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevCropAspect5x7',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/SetCropAspect=Set crop ratio: ^1',LOC('$$$/AgLibrary/CameraRawView/Ops/CropNameConstruction=^1 x ^2','5','7')),Group=quickdev,Explanation='Set crop ratio to 5 by 7. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevCropAspect85x11',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/SetCropAspect=Set crop ratio: ^1',LOC('$$$/AgLibrary/CameraRawView/Ops/CropNameConstruction=^1 x ^2','8.5','11')),Group=quickdev,Explanation='Set crop ratio to 8.5 by 11. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules.'},
  {Command='QuickDevCropAspect9x16',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/SetCropAspect=Set crop ratio: ^1',LOC('$$$/AgLibrary/CameraRawView/Ops/CropNameConstruction=^1 x ^2','9','16')),Group=quickdev,Explanation='Set crop ratio to 9 by 16. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules.'},

  --Develop
  {Command='SwToMdevelop',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',LOC('$$$/SmartCollection/Criteria/Heading/Develop=Develop')),Group=develop,Explanation='Switch to Develop module.'},
  --develop: copy paste sync
  {Command='LRCopy',Type='button',Translation='Lightroom '..LOC('$$$/AgLibrary/Menu/Develop/CopySettings=Copy Settings'):gsub('%(%&%a%)',''):gsub('%&',''),Group=develop,Explanation='Lightroom Copy (open the selection box). Sends the keystroke <kbd>\226\140\131 Control</kbd>+<kbd>\226\135\167 Shift</kbd>+<kbd>c</kbd> (Windows) or <kbd>\226\140\152 Command</kbd>+<kbd>\226\135\167 Shift</kbd>+<kbd>c</kbd> (OSX) to Lightroom.'},
  {Command='LRPaste',Type='button',Translation='Lightroom '..LOC('$$$/AgCameraRawNamedSettings/Ops/PasteSettings=Paste Settings'),Group=develop,Explanation='Lightroom Paste. Sends the keystroke <kbd>\226\140\131 Control</kbd>+<kbd>\226\135\167 Shift</kbd>+<kbd>v</kbd> (Windows) or <kbd>\226\140\152 Command</kbd>+<kbd>\226\135\167 Shift</kbd>+<kbd>v</kbd> (OSX) to Lightroom.'},
  {Command='VirtualCopy',Type='button',Translation=LOC('$$$/AgLibrary/History/CreateVirtualCopy=Create Virtual Copy'),Group=develop,Explanation='Creates a virtual copy for each of the currently selected photos and videos. The new virtual copies will be selected.'},
  {Command='ResetAll',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/Ops/ResetSettings=Reset Settings'),Group=develop,Explanation='Reset to defaults.'},
  {Command='ResetLast',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/LibraryUpgradeCatalogUtils/CatalogInfo/LastModified/Label=Last Modified'):gsub(' ?:','')),Group=develop,Explanation='Resets the last parameter that was adjusted by an encoder or fader to default.'},
  {Command='ChangeLastDevelopParameter',Type='repeat',Translation=LOC('$$$/ImportView/More=More')..' – '..LOC('$$$/ImportView/Less=Less')..' '..LOC('$$$/LibraryUpgradeCatalogUtils/CatalogInfo/LastModified/Label=Last Modified'),Group=develop,Explanation='Increments or decrements the last parameter that was adjusted by an encoder or fader. Turning knob clockwise sends Increment signals to Lightroom, counterclockwise Decrement.'..repeatexp,Repeats={'IncrementLastDevelopParameter','DecrementLastDevelopParameter'}},
  {Command='IncrementLastDevelopParameter',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Increase=^1 Increase: ^2','',LOC('$$$/LibraryUpgradeCatalogUtils/CatalogInfo/LastModified/Label=Last Modified')):gsub(' ?:',''):gsub('^%s*(.-)%s*$', '%1'),Group=develop,Explanation='Increments the last parameter that was adjusted by an encoder or fader.'},
  {Command='DecrementLastDevelopParameter',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Decrease=^1 Decrease: ^2','',LOC('$$$/LibraryUpgradeCatalogUtils/CatalogInfo/LastModified/Label=Last Modified')):gsub(' ?:',''):gsub('^%s*(.-)%s*$', '%1'),Group=develop,Explanation='Decrements the last parameter that was adjusted by an encoder or fader.'},
  {Command='ChangeCurrentSlider',Type='repeat',Translation=LOC('$$$/ImportView/More=More')..' – '..LOC('$$$/ImportView/Less=Less'),Group=develop,Explanation='Adjusts selected develop slider up or down. Turning knob clockwise sends Increment keystrokes to Lightroom, counterclockwise Decrement.'..repeatexp,Repeats={'SliderIncrease','SliderDecrease'}},
  {Command='SliderIncrease',Type='button',Translation=LOC('$$$/ImportView/More=More'),Group=develop,Explanation='Sends the keystroke to increase the selected develop slider.'},
  {Command='SliderDecrease',Type='button',Translation=LOC('$$$/ImportView/Less=Less'),Group=develop,Explanation='Sends the keystroke to decrease the selected develop slider.'},
  {Command='RedoUndo',Type='repeat',Translation=LOC('$$$/Bezel/RedoTitle=Redo')..' – '..LOC('$$$/AgCameraRawController/SoftProofingVirtualCopyPrompt/Undo=Undo'),Group=develop,Explanation='Turning knob clockwise sends Redo keystrokes (<kbd>\226\140\131 Control</kbd>+<kbd>y</kbd> (Windows) or <kbd>\226\140\152 Command</kbd>+<kbd>\226\135\167 Shift</kbd>+<kbd>z</kbd> (OSX)) to Lightroom, counterclockwise Undo (<kbd>\226\140\131 Control</kbd>+<kbd>z</kbd> (Windows) or <kbd>\226\140\152 Command</kbd>+<kbd>z</kbd> (OSX)).'..repeatexp,Repeats={'Redo','Undo'}},
  {Command='Undo',Type='button',Translation=LOC('$$$/AgCameraRawController/SoftProofingVirtualCopyPrompt/Undo=Undo'),Group=develop,Explanation='Sends the keystroke <kbd>\226\140\131 Control</kbd>+<kbd>z</kbd> (Windows) or <kbd>\226\140\152 Command</kbd>+<kbd>z</kbd> (OSX) to Lightroom.'},
  {Command='Redo',Type='button',Translation=LOC('$$$/Bezel/RedoTitle=Redo'),Group=develop,Explanation='Sends the keystroke <kbd>\226\140\131 Control</kbd>+<kbd>y</kbd> (Windows) or <kbd>\226\140\152 Command</kbd>+<kbd>\226\135\167 Shift</kbd>+<kbd>z</kbd> (OSX) to Lightroom.'},
  {Command='PV1',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/Ops/UpdateAndSetProcessVersionFull=Update process version to ^1',1),Group=develop,Explanation='Sets the Process Version of all selected photos to PV 1.'},
  {Command='PV2',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/Ops/UpdateAndSetProcessVersionFull=Update process version to ^1',2),Group=develop,Explanation='Sets the Process Version of all selected photos to PV 2.'},
  {Command='PV3',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/Ops/UpdateAndSetProcessVersionFull=Update process version to ^1',3),Group=develop,Explanation='Sets the Process Version of all selected photos to PV 3.'},
  {Command='PV4',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/Ops/UpdateAndSetProcessVersionFull=Update process version to ^1',4),Group=develop,Explanation='Sets the Process Version of all selected photos to PV 4.'},
  --{Command='PV5',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/Ops/UpdateAndSetProcessVersionFull=Update process version to ^1',5),Group=develop,Explanation='Sets the Process Version of all selected photos to PV 5.'},
  {Command='PVLatest',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/Ops/UpdateToCurrentProcessVersion=Update to current process version'),Group=develop,Explanation='Sets the Process Version of all selected photos to the latest (currently PV '..LatestPVSupported..'). This Process Version command is recommended if you want the latest version instead of one specific version.'},
  {Command='ShowClipping',Type='button',Translation=LOC('$$$/AgDevelop/Histogram/Menu/ShowClippingIndicators=Show clipping'),Group=develop,Explanation='Toggles clipping indicators on/off. Must be called while the Develop module is active.'},
  --develop: before/after previews
  {Command='ShoVwdevelop_before_after_horiz',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgPhotoBin/ViewMode/Develop/BeforeAfterLR=Before/After Left/Right'),Group=develop,Explanation=''},
  {Command='ShoVwdevelop_before_after_vert',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgPhotoBin/ViewMode/Develop/BeforeAfterTB=Before/After Top/Bottom'),Group=develop,Explanation=''},
  {Command='ShoVwdevelop_before',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgPhotoBin/ViewMode/Develop/Before=Before'),Group=develop,Explanation=''},
  {Command='ShoVwRefHoriz',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgDevelop/Menu/View/ReferenceActiveLeftRight=Reference View — Left/Right'),Group=develop,Explanation=''},
  {Command='ShoVwRefVert',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgDevelop/Menu/View/ReferenceActiveTopBottom=Reference View — Top/Bottom'),Group=develop,Explanation=''},
  {Command='ShoVwdevelop_loupe',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgPhotoBin/ViewMode/Develop/Loupe=Loupe'),Group=develop,Explanation=''},
  --
  --develop: basic tone panel
  --
  {Command='RevealPanelAdjust',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',basicTone),Group=basicTone,Explanation='Open Basic Adjustments Panel in Develop Module.'},
  {Command='WhiteBalanceAs_Shot',Type='button',Experimental=true,Translation=whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/AsShot=As Shot'),Group=basicTone,Explanation='Use Temperature and Tint as determined by camera.',Panel='adjustPanel'},
  {Command='WhiteBalanceAuto',Type='button',Translation=whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Auto=Auto'),Group=basicTone,Explanation='Have Lightroom determine Temperature and Tint.',Panel='adjustPanel'},
  {Command='WhiteBalanceDaylight',Type='button',Experimental=true,Translation=whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Daylight=Daylight'),Group=basicTone,Explanation='Use daylight white balance.',Panel='adjustPanel'},
  {Command='WhiteBalanceCloudy',Type='button',Experimental=true,Translation=whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Cloudy=Cloudy'),Group=basicTone,Explanation='Use cloudy white balance.',Panel='adjustPanel'},
  {Command='WhiteBalanceShade',Type='button',Experimental=true,Translation=whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Shade=Shade'),Group=basicTone,Explanation='Use shade white balance.',Panel='adjustPanel'},
  {Command='WhiteBalanceTungsten',Type='button',Experimental=true,Translation=whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Tungsten=Tungsten'),Group=basicTone,Explanation='Use tungsten white balance.',Panel='adjustPanel'},
  {Command='WhiteBalanceFluorescent',Type='button',Experimental=true,Translation=whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Fluorescent=Fluorescent'),Group=basicTone,Explanation='Use fluorescent white balance.',Panel='adjustPanel'},
  {Command='WhiteBalanceFlash',Type='button',Experimental=true,Translation=whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Flash=Flash'),Group=basicTone,Explanation='Use flash white balance.',Panel='adjustPanel'},
  {Command='AutoTone',Type='button',Translation=LOC('$$$/AgDevelopShortcuts/Auto_Tone=Auto Tone'),Group=basicTone,Explanation='Auto Tone.',Panel='adjustPanel'},
  {Command='Temperature',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Temperature=Temperature'),Group=basicTone,Explanation='Fine-tunes the white balance using the Kelvin color temperature scale. Move the slider to the left to make the photo appear cooler, and right to warm the photo colors.',Panel='adjustPanel'},
  {Command='Tint',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Tint=Tint'),Group=basicTone,Explanation='Fine-tunes the white balance to compensate for a green or magenta tint. Move the slider to the left (negative values) to add green to the photo; move it to the right (positive values) to add magenta.',Panel='adjustPanel'},
  {Command='Exposure',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Exposure=Exposure'),Group=basicTone,Explanation='Sets the overall image brightness. Adjust the slider until the photo looks good and the image is the desired brightness.',Panel='adjustPanel',AltParameter='Exposure2012'},
  {Command='Contrast',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Contrast=Contrast'),Group=basicTone,Explanation='Increases or decreases image contrast, mainly affecting midtones. When you increase contrast, the middle-to-dark image areas become darker, and the middle-to-light image areas become lighter. The image tones are inversely affected as you decrease contrast.',Panel='adjustPanel',AltParameter='Contrast2012'},
  {Command='Highlights',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Highlights=Highlights')..' ('..LOC('$$$/AgCameraRawUI/HighlightRecovery=Highlight Recovery')..' in '..P1and2..')',PV1=LOC('$$$/AgCameraRawUI/HighlightRecovery=Highlight Recovery'),PV3=LOC('$$$/AgCameraRawUI/Highlights=Highlights'),Group=basicTone,Explanation='Adjusts bright image areas. Drag to the left to darken highlights and recover \226\128\156blown out\226\128\157 highlight details. Drag to the right to brighten highlights while minimizing clipping.<br /><br />In '..P1and2..', controls recovery and reduces the tones of extreme highlights and attempts to recover highlight detail lost because of camera overexposure. Lightroom can recover detail in raw image files if one or two channels are clipped.',Panel='adjustPanel',AltParameter='Highlights2012'},
  {Command='Brightness',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Brightness=Brightness'),Group=basicTone,Explanation='Adjusts image brightness, mainly affecting midtones. Adjust Brightness after setting Exposure, Recovery, and Blacks sliders. Large brightness adjustments can affect shadow or highlight clipping, so you may want to readjust the Exposure, Recovery, or Blacks slider after adjusting brightness. No effect unless in PV 1 or PV 2)',Panel='adjustPanel'},
  {Command='Shadows',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Shadows=Shadows')..' ('..LOC('$$$/AgCameraRawUI/FillLight=Fill Light')..' in '..P1and2..')',PV1=LOC('$$$/AgCameraRawUI/FillLight=Fill Light'),PV3=LOC('$$$/AgCameraRawUI/Shadows=Shadows'),Group=basicTone,Explanation='Adjusts dark image areas. Drag to the left to darken shadows while minimizing clipping. Drag to the right to brighten shadows and recover shadow details.<br /><br />In '..P1and2..', controls Fill Light, and lightens shadow to reveal more detail while maintaining blacks. Take care not to over apply the setting and reveal image noise.',Panel='adjustPanel',AltParameter='Shadows2012'},
  {Command='Whites',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Whites=Whites')..' ('..LOC('$$$/AgCameraRawUI/Brightness=Brightness')..' in '..P1and2..')',PV1=LOC('$$$/AgCameraRawUI/Brightness=Brightness'),PV3=LOC('$$$/AgCameraRawUI/Whites=Whites'),Group=basicTone,Explanation='Adjusts white clipping. Drag to the left to reduce clipping in highlights. Drag to the right to increase highlight clipping.',Panel='adjustPanel',AltParameter='Whites2012'},
  {Command='Blacks',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Blacks=Blacks'),Group=basicTone,Explanation='Specifies which image values map to black. Moving the slider to the right increases the areas that become black, sometimes creating the impression of increased image contrast. The greatest effect is in the shadows, with much less change in the midtones and highlights.',Panel='adjustPanel',AltParameter='Blacks2012'},
  {Command='Texture',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Texture=Texture')..' (PV 3+)',PV3=LOC('$$$/AgCameraRawUI/Texture=Texture'),Group=basicTone,Explanation='Adjust the Texture slider negatively to smooth skin and retain fine pore details to ensure natural-looking skin. Increase the Texture amount to accentuate details such as bark or hair without affecting less detailed areas, like the out of focus areas in a photograph, Process Version 3+. Photos with Process Version 3 or 4 will be auto updated to current process version on changing Texture.',Panel='adjustPanel'},
  {Command='Clarity',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Clarity=Clarity'),Group=basicTone,Explanation='Adds depth to an image by increasing local contrast. When using this setting, it is best to zoom in to 100% or greater.',Panel='adjustPanel',AltParameter='Clarity2012'},
  {Command='Dehaze',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/DehazeAmount=Dehaze Amount')..' (PV 3+)',PV3=LOC('$$$/AgCameraRawUI/DehazeAmount=Dehaze Amount'),Group=basicTone,Explanation='Controls the amount of haze in a photograph. Drag to the right to remove haze; drag to the left to add haze. For PV 3+ only.',Panel='adjustPanel'},
  {Command='Vibrance',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Vibrance=Vibrance'),Group=basicTone,Explanation='Adjusts the saturation so that clipping is minimized as colors approach full saturation, changing the saturation of all lower-saturated colors with less effect on the higher-saturated colors. Vibrance also prevents skin tones from becoming over saturated.',Panel='adjustPanel'},
  {Command='Saturation',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Saturation=Saturation'),Group=basicTone,Explanation='Adjusts the saturation of all image colors equally from -100 (monochrome) to +100 (double the saturation).',Panel='adjustPanel'},
  {Command='ResetTemperature',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Temperature=Temperature')),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetTint',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawUI/Tint=Tint')),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetExposure',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawUI/Exposure=Exposure')),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetContrast',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawUI/Contrast=Contrast')),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetHighlights',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawUI/Highlights=Highlights')),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetShadows',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawUI/Shadows=Shadows')),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetWhites',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawUI/Whites=Whites')),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetBlacks',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawUI/Blacks=Blacks')),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetTexture',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawUI/Texture=Texture')),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetClarity',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawUI/Clarity=Clarity')),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetDehaze',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawUI/DehazeAmount=Dehaze Amount')),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetVibrance',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawUI/Vibrance=Vibrance')),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetSaturation',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawUI/Saturation=Saturation')),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  --
  --develop: tone curve panel
  --
  {Command='RevealPanelTone',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',toneCurve),Group=toneCurve,Explanation='Open Tone Curve Panel in Develop Module.'},
  {Command='EnableToneCurve',Type='button',Translation=LOC('$$$/AgDevelop/Settings/ToggleToneCurve=Tone curve enable/disable'),Group=toneCurve,Explanation='Enable or disable tone curve..',Panel='tonePanel'},
  {Command='ParametricDarks',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneDarks=Dark Tones'),Group=toneCurve,Explanation='Adjust darks.',Panel='tonePanel'},
  {Command='ParametricLights',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneLights=Light Tones'),Group=toneCurve,Explanation='Adjust lights.',Panel='tonePanel'},
  {Command='ParametricShadows',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadows=Shadow Tones'),Group=toneCurve,Explanation='Adjust shadows.',Panel='tonePanel'},
  {Command='ParametricHighlights',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlights=Highlight Tones'),Group=toneCurve,Explanation='Adjust highlights.',Panel='tonePanel'},
  {Command='ParametricShadowSplit',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadowSplit=Shadow Split'),Group=toneCurve,Explanation='Move division between shadows and darks.',Panel='tonePanel'},
  {Command='ParametricMidtoneSplit',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneMidtoneSplit=Midtone Split'),Group=toneCurve,Explanation='Move division between darks and lights.',Panel='tonePanel'},
  {Command='ParametricHighlightSplit',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlightSplit=Highlight Split'),Group=toneCurve,Explanation='Move division between lights and highlights.',Panel='tonePanel'},
  {Command='ResetParametricDarks',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneDarks=Dark Tones')),Group=toneCurve,Explanation='Reset to default.',Panel='tonePanel'},
  {Command='ResetParametricLights',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneLights=Light Tones')),Group=toneCurve,Explanation='Reset to default.',Panel='tonePanel'},
  {Command='ResetParametricShadows',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadows=Shadow Tones')),Group=toneCurve,Explanation='Reset to default.',Panel='tonePanel'},
  {Command='ResetParametricHighlights',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlights=Highlight Tones')),Group=toneCurve,Explanation='Reset to default.',Panel='tonePanel'},
  {Command='ResetParametricShadowSplit',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadowSplit=Shadow Split')),Group=toneCurve,Explanation='Reset to default.',Panel='tonePanel'},
  {Command='ResetParametricMidtoneSplit',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneMidtoneSplit=Midtone Split')),Group=toneCurve,Explanation='Reset to default.',Panel='tonePanel'},
  {Command='ResetParametricHighlightSplit',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlightSplit=Highlight Split')),Group=toneCurve,Explanation='Reset to default.',Panel='tonePanel'},
  {Command='PointCurveLinear',Type='button',Translation=toneCurve..' '..LOC('$$$/CRaw/ToneCurvePreset/Linear=Linear'),Group=toneCurve,Explanation='Linear Point Curve.',Panel='tonePanel'},
  {Command='PointCurveMediumContrast',Type='button',Translation=toneCurve..' '..LOC('$$$/CRaw/ToneCurvePreset/MediumContrast=Medium Contrast'),Group=toneCurve,Explanation='Medium Contrast Point Curve.',Panel='tonePanel'},
  {Command='PointCurveStrongContrast',Type='button',Translation=toneCurve..' '..LOC('$$$/CRaw/ToneCurvePreset/StrongContrast=Strong Contrast'),Group=toneCurve,Explanation='Strong Contrast Point Curve.',Panel='tonePanel'},
  --
  --develop: mixer panel
  --
  {Command='RevealPanelMixer',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',colorAdjustments),Group=colorAdjustments,Explanation='Open Mixer Panel in Develop Module.'},
  {Command='EnableColorAdjustments',Type='button',Translation=LOC('$$$/AgDevelop/Settings/ToggleColorAdjustments=Color adjustments enable/disable'),Group=colorAdjustments,Explanation='Enable or disable color adjustments.',Panel='mixerPanel'},
  {Command='SaturationAdjustmentRed',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentRed=Saturation Adjustment Red'),Group=colorAdjustments,Explanation='Changes the color vividness or purity of the color.',Panel='mixerPanel'},
  {Command='SaturationAdjustmentOrange',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentOrange=Saturation Adjustment Orange'),Group=colorAdjustments,Explanation='Changes the color vividness or purity of the color.',Panel='mixerPanel'},
  {Command='SaturationAdjustmentYellow',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentYellow=Saturation Adjustment Yellow'),Group=colorAdjustments,Explanation='Changes the color vividness or purity of the color.',Panel='mixerPanel'},
  {Command='SaturationAdjustmentGreen',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentGreen=Saturation Adjustment Green'),Group=colorAdjustments,Explanation='Changes the color vividness or purity of the color.',Panel='mixerPanel'},
  {Command='SaturationAdjustmentAqua',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentAqua=Saturation Adjustment Aqua'),Group=colorAdjustments,Explanation='Changes the color vividness or purity of the color.',Panel='mixerPanel'},
  {Command='SaturationAdjustmentBlue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentBlue=Saturation Adjustment Blue'),Group=colorAdjustments,Explanation='Changes the color vividness or purity of the color.',Panel='mixerPanel'},
  {Command='SaturationAdjustmentPurple',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentPurple=Saturation Adjustment Purple'),Group=colorAdjustments,Explanation='Changes the color vividness or purity of the color.',Panel='mixerPanel'},
  {Command='SaturationAdjustmentMagenta',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentMagenta=Saturation Adjustment Magenta'),Group=colorAdjustments,Explanation='Changes the color vividness or purity of the color.',Panel='mixerPanel'},
  {Command='AllSaturationAdjustment',Type='variable',Translation=LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Saturation=Saturation')..' \226\128\148 '..LOC('$$$/AgDateManagement/ChangeAll=Change all'),Group=colorAdjustments,Explanation='Changes all color vividness or purity of all colors.',Panel='mixerPanel'},
  {Command='HueAdjustmentRed',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentRed=Hue Adjustment Red'),Group=colorAdjustments,Explanation='Changes the color.',Panel='mixerPanel'},
  {Command='HueAdjustmentOrange',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentOrange=Hue Adjustment Orange'),Group=colorAdjustments,Explanation='Changes the color.',Panel='mixerPanel'},
  {Command='HueAdjustmentYellow',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentYellow=Hue Adjustment Yellow'),Group=colorAdjustments,Explanation='Changes the color.',Panel='mixerPanel'},
  {Command='HueAdjustmentGreen',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentGreen=Hue Adjustment Green'),Group=colorAdjustments,Explanation='Changes the color.',Panel='mixerPanel'},
  {Command='HueAdjustmentAqua',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentAqua=Hue Adjustment Aqua'),Group=colorAdjustments,Explanation='Changes the color.',Panel='mixerPanel'},
  {Command='HueAdjustmentBlue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentBlue=Hue Adjustment Blue'),Group=colorAdjustments,Explanation='Changes the color.',Panel='mixerPanel'},
  {Command='HueAdjustmentPurple',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentPurple=Hue Adjustment Purple'),Group=colorAdjustments,Explanation='Changes the color.',Panel='mixerPanel'},
  {Command='HueAdjustmentMagenta',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentMagenta=Hue Adjustment Magenta'),Group=colorAdjustments,Explanation='Changes the color.',Panel='mixerPanel'},
  {Command='LuminanceAdjustmentRed',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentRed=Luminance Adjustment Red'),Group=colorAdjustments,Explanation='Changes the brightness of the color range.',Panel='mixerPanel'},
  {Command='LuminanceAdjustmentOrange',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentOrange=Luminance Adjustment Orange'),Group=colorAdjustments,Explanation='Changes the brightness of the color range.',Panel='mixerPanel'},
  {Command='LuminanceAdjustmentYellow',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentYellow=Luminance Adjustment Yellow'),Group=colorAdjustments,Explanation='Changes the brightness of the color range.',Panel='mixerPanel'},
  {Command='LuminanceAdjustmentGreen',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentGreen=Luminance Adjustment Green'),Group=colorAdjustments,Explanation='Changes the brightness of the color range.',Panel='mixerPanel'},
  {Command='LuminanceAdjustmentAqua',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentAqua=Luminance Adjustment Aqua'),Group=colorAdjustments,Explanation='Changes the brightness of the color range.',Panel='mixerPanel'},
  {Command='LuminanceAdjustmentBlue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentBlue=Luminance Adjustment Blue'),Group=colorAdjustments,Explanation='Changes the brightness of the color range.',Panel='mixerPanel'},
  {Command='LuminanceAdjustmentPurple',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentPurple=Luminance Adjustment Purple'),Group=colorAdjustments,Explanation='Changes the brightness of the color range.',Panel='mixerPanel'},
  {Command='LuminanceAdjustmentMagenta',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentMagenta=Luminance Adjustment Magenta'),Group=colorAdjustments,Explanation='Changes the brightness of the color range.',Panel='mixerPanel'},
  {Command='ConvertToGrayscale',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ConvertToGrayscale=Convert to Grayscale'),Group=colorAdjustments,Explanation='Enable/disable gray scale conversion.',Panel='mixerPanel'},
  {Command='EnableGrayscaleMix',Type='button',Translation=LOC('$$$/AgDevelop/Settings/ToggleBlackAndWhiteMix=Grayscale mix enable/disable'),Group=colorAdjustments,Explanation='Enable or disable black and white mix.',Panel='mixerPanel'},
  {Command='GrayMixerRed',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerRed=Gray Mixer Red'),Group=colorAdjustments,Explanation='Red contribution to luminance.',Panel='mixerPanel'},
  {Command='GrayMixerOrange',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerOrange=Gray Mixer Orange'),Group=colorAdjustments,Explanation='Orange contribution to luminance.',Panel='mixerPanel'},
  {Command='GrayMixerYellow',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerYellow=Gray Mixer Yellow'),Group=colorAdjustments,Explanation='Yellow contribution to luminance.',Panel='mixerPanel'},
  {Command='GrayMixerGreen',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerGreen=Gray Mixer Green'),Group=colorAdjustments,Explanation='Green contribution to luminance.',Panel='mixerPanel'},
  {Command='GrayMixerAqua',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerAqua=Gray Mixer Aqua'),Group=colorAdjustments,Explanation='Aqua contribution to luminance.',Panel='mixerPanel'},
  {Command='GrayMixerBlue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerBlue=Gray Mixer Blue'),Group=colorAdjustments,Explanation='Blue contribution to luminance.',Panel='mixerPanel'},
  {Command='GrayMixerPurple',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerPurple=Gray Mixer Purple'),Group=colorAdjustments,Explanation='Purple contribution to luminance.',Panel='mixerPanel'},
  {Command='GrayMixerMagenta',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerMagenta=Gray Mixer Magenta'),Group=colorAdjustments,Explanation='Magenta contribution to luminance.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentRed',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentRed=Saturation Adjustment Red')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentOrange',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentOrange=Saturation Adjustment Orange')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentYellow',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentYellow=Saturation Adjustment Yellow')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentGreen',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentGreen=Saturation Adjustment Green')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentAqua',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentAqua=Saturation Adjustment Aqua')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentBlue',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentBlue=Saturation Adjustment Blue')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentPurple',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentPurple=Saturation Adjustment Purple')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentMagenta',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentMagenta=Saturation Adjustment Magenta')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetAllSaturationAdjustment',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgDevelop/Settings/SaturationAdjustment=Saturation Adjustments')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentRed',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentRed=Hue Adjustment Red')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentOrange',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentOrange=Hue Adjustment Orange')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentYellow',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentYellow=Hue Adjustment Yellow')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentGreen',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentGreen=Hue Adjustment Green')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentAqua',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentAqua=Hue Adjustment Aqua')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentBlue',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentBlue=Hue Adjustment Blue')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentPurple',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentPurple=Hue Adjustment Purple')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentMagenta',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentMagenta=Hue Adjustment Magenta')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetAllHueAdjustment',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgDevelop/Settings/HueAdjustment=Hue Adjustments')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentRed',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentRed=Luminance Adjustment Red')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentOrange',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentOrange=Luminance Adjustment Orange')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentYellow',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentYellow=Luminance Adjustment Yellow')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentGreen',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentGreen=Luminance Adjustment Green')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentAqua',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentAqua=Luminance Adjustment Aqua')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentBlue',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentBlue=Luminance Adjustment Blue')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentPurple',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentPurple=Luminance Adjustment Purple')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentMagenta',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentMagenta=Luminance Adjustment Magenta')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetAllLuminanceAdjustment',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgDevelop/Settings/LuminanceAdjustment=Luminance Adjustments')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerRed',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerRed=Gray Mixer Red')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerOrange',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerOrange=Gray Mixer Orange')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerYellow',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerYellow=Gray Mixer Yellow')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerGreen',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerGreen=Gray Mixer Green')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerAqua',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerAqua=Gray Mixer Aqua')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerBlue',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerBlue=Gray Mixer Blue')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerPurple',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerPurple=Gray Mixer Purple')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerMagenta',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerMagenta=Gray Mixer Magenta')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetAllGrayMixer',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgDevelop/Setting/BlackAndWhiteMix=Gray Mixer')),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  --
  --develop: color grading panel
  --
  {Command='RevealPanelColorGrading',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',colorGrading),Group=colorGrading,Explanation='Open Color Grading Panel in Develop Module.'},
  {Command='EnableColorGrading',Type='button',Translation=LOC('$$$/AgDevelop/Settings/ToggleColorGrading=Color grading enable/disable'),Group=colorGrading,Explanation='Enable or disable color grading.',Panel='colorGradingPanel'},
  {Command='ColorGradeBlending',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeBlending=Color Blending'),Group=colorGrading,Explanation='Color grading blending changes the transition between each range: highlight, midtone, shadow. More blending gives a broader transition. The legacy Split Toning panel used a blending value of 100.',Panel='colorGradingPanel'},
  {Command='SplitToningBalance',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeBalance=Color Grading Balance'),Group=colorGrading,Explanation='Set the Balance slider to balance the effect between the Highlight and Shadow sliders. Positive values moves more of the image into the highlight slider range; negative values moves more of the image into the shadows slider range. Midtones are shifted as well.',Panel='colorGradingPanel'},
  {Command='ColorGradeGlobalHue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeGlobalHue=Global Hue'),Group=colorGrading,Explanation='Adjust color grading global hue. Users with [controllers that send relative values](https://github.com/rsjaffe/MIDI2LR/wiki/MIDI-Controller-Setup#controllers-that-send-relative-values-or-have-unusual-ranges) can rotate continuously around the color wheel without ever having to reverse the control movement.',Panel='colorGradingPanel',Wraps=true},
  {Command='ColorGradeGlobalLum',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeGlobalLum=Global Luminance'),Group=colorGrading,Explanation='Adjust color grading global luminance.',Panel='colorGradingPanel'},
  {Command='ColorGradeGlobalSat',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeGlobalSat=Global Saturation'),Group=colorGrading,Explanation='Adjust color grading global saturation.',Panel='colorGradingPanel'},
  {Command='SplitToningHighlightHue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightHue=Highlight Hue'),Group=colorGrading,Explanation='Adjust color grading highlight hue. Users with [controllers that send relative values](https://github.com/rsjaffe/MIDI2LR/wiki/MIDI-Controller-Setup#controllers-that-send-relative-values-or-have-unusual-ranges) can rotate continuously around the color wheel without ever having to reverse the control movement.',Panel='colorGradingPanel',Wraps=true},
  {Command='ColorGradeHighlightLum',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeHighlightLum=Highlight Luminance'),Group=colorGrading,Explanation='Adjust color grading highlight luminance.',Panel='colorGradingPanel'},
  {Command='SplitToningHighlightSaturation',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightSaturation=Highlight Saturation'),Group=colorGrading,Explanation='Adjust color grading highlight saturation.',Panel='colorGradingPanel'},
  {Command='ColorGradeMidtoneHue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeMidtoneHue=Midtone Hue'),Group=colorGrading,Explanation='Adjust color grading midtone hue. Users with [controllers that send relative values](https://github.com/rsjaffe/MIDI2LR/wiki/MIDI-Controller-Setup#controllers-that-send-relative-values-or-have-unusual-ranges) can rotate continuously around the color wheel without ever having to reverse the control movement.',Panel='colorGradingPanel',Wraps=true},
  {Command='ColorGradeMidtoneLum',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeMidtoneLum=Midtone Luminance'),Group=colorGrading,Explanation='Adjust color grading midtone luminance.',Panel='colorGradingPanel'},
  {Command='ColorGradeMidtoneSat',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeMidtoneSat=Midtone Saturation'),Group=colorGrading,Explanation='Adjust color grading midtone saturation.',Panel='colorGradingPanel'},
  {Command='SplitToningShadowHue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowHue=Shadow Hue'),Group=colorGrading,Explanation='Adjust color grading shadow hue. Users with [controllers that send relative values](https://github.com/rsjaffe/MIDI2LR/wiki/MIDI-Controller-Setup#controllers-that-send-relative-values-or-have-unusual-ranges) can rotate continuously around the color wheel without ever having to reverse the control movement.',Panel='colorGradingPanel',Wraps=true},
  {Command='ColorGradeShadowLum',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeShadowLum=Shadow Luminance'),Group=colorGrading,Explanation='Adjust color grading shadow luminance.',Panel='colorGradingPanel'},
  {Command='SplitToningShadowSaturation',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowSaturation=Shadow Saturation'),Group=colorGrading,Explanation='Adjust color grading shadow saturation.',Panel='colorGradingPanel'},
  {Command='ColorGradeCopy',Type='button',Translation=LOC('$$$/AgDevelop/ColorGrading/ChannelSettings/Copy=Copy channel settings'),Group=colorGrading,Explanation='Copy settings from currently displayed channel.',Panel='colorGradingPanel'},
  {Command='ColorGradePaste',Type='button',Translation=LOC('$$$/AgDevelop/ColorGrading/ChannelSettings/Paste=Paste channel settings'),Group=colorGrading,Explanation='Paste settings to currently displayed channel.',Panel='colorGradingPanel'},
  {Command='ColorGradeReset3way',Type='button',Translation=LOC('$$$/AgDevelop/ColorGrading/ResetChannel/ThreeWay=Reset 3-way'),Group=colorGrading,Explanation='Reset all channels in 3-way view.',Panel='colorGradingPanel'},
  {Command='ColorGradeResetCurrent',Type='button',Translation=LOC('$$$/AgDevelop/ColorGrading/ResetChannel/Current=Reset ^1',LOC('$$$/AgDevelop/BeforeAfter/Current=current')),Group=colorGrading,Explanation='Reset currently viewed channel(s).',Panel='colorGradingPanel'},
  {Command='ColorGradeResetAll',Type='button',Translation=LOC('$$$/AgDevelop/ColorGrading/ResetChannel/All=Reset all channels'),Group=colorGrading,Explanation='Reset all channels.',Panel='colorGradingPanel'},
  {Command='ColorGrade3Way',Type='button',Translation=LOC('$$$/AgDevelop/ColorGrading/Peeking/Tooltip=Click for ^1 View',LOC('$$$/CRaw/ColorGrading/Tips/3Way=3-Way')),Group=colorGrading,Explanation='Display 3-way view in color grading panel.',Panel='colorGradingPanel'},
  {Command='ColorGradeGlobal',Type='button',Translation=LOC('$$$/AgDevelop/ColorGrading/Peeking/Tooltip=Click for ^1 View',LOC('$$$/CRaw/ColorGrading/Tips/Global=Global')),Group=colorGrading,Explanation='Display global channel in color grading panel.',Panel='colorGradingPanel'},
  {Command='ColorGradeHighlight',Type='button',Translation=LOC('$$$/AgDevelop/ColorGrading/Peeking/Tooltip=Click for ^1 View',LOC('$$$/CRaw/ColorGrading/Tips/Highlights=Highlights')),Group=colorGrading,Explanation='Display highlights channel in color grading panel.',Panel='colorGradingPanel'},
  {Command='ColorGradeMidtone',Type='button',Translation=LOC('$$$/AgDevelop/ColorGrading/Peeking/Tooltip=Click for ^1 View',LOC('$$$/CRaw/ColorGrading/Tips/Midtones=Midtones')),Group=colorGrading,Explanation='Display midtones channel in color grading panel.',Panel='colorGradingPanel'},
  {Command='ColorGradeShadow',Type='button',Translation=LOC('$$$/AgDevelop/ColorGrading/Peeking/Tooltip=Click for ^1 View',LOC('$$$/CRaw/ColorGrading/Tips/Shadows=Shadows')),Group=colorGrading,Explanation='Display shadows channel in color grading panel.',Panel='colorGradingPanel'},
  {Command='ResetColorGradeBlending',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeBlending=Color Blending')),Group=colorGrading,Explanation='Reset color grading blending to default.',Panel='colorGradingPanel'},
  {Command='ResetSplitToningBalance',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeBalance=Color Grading Balance')),Group=colorGrading,Explanation='Reset color grading balance to default.',Panel='colorGradingPanel'},
  {Command='ResetColorGradeGlobalHue',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeGlobalHue=Global Hue')),Group=colorGrading,Explanation='Reset color grading global hue to default.',Panel='colorGradingPanel'},
  {Command='ResetColorGradeGlobalLum',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeGlobalLum=Global Luminance')),Group=colorGrading,Explanation='Reset color grading global luminance to default.',Panel='colorGradingPanel'},
  {Command='ResetColorGradeGlobalSat',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeGlobalSat=Global Saturation')),Group=colorGrading,Explanation='Reset color grading global saturation to default.',Panel='colorGradingPanel'},
  {Command='ResetSplitToningHighlightHue',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightHue=Highlight Hue')),Group=colorGrading,Explanation='Reset color grading highlight hue to default.',Panel='colorGradingPanel'},
  {Command='ResetColorGradeHighlightLum',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeHighlightLum=Highlight Luminance')),Group=colorGrading,Explanation='Reset color grading highlight luminance to default.',Panel='colorGradingPanel'},
  {Command='ResetSplitToningHighlightSaturation',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightSaturation=Highlight Saturation')),Group=colorGrading,Explanation='Reset color grading highlight saturation to default.',Panel='colorGradingPanel'},
  {Command='ResetColorGradeMidtoneHue',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeMidtoneHue=Midtone Hue')),Group=colorGrading,Explanation='Reset color grading midtone hue to default.',Panel='colorGradingPanel'},
  {Command='ResetColorGradeMidtoneLum',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeMidtoneLum=Midtone Luminance')),Group=colorGrading,Explanation='Reset color grading midtone luminance to default.',Panel='colorGradingPanel'},
  {Command='ResetColorGradeMidtoneSat',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeMidtoneSat=Midtone Saturation')),Group=colorGrading,Explanation='Reset color grading midtone saturation to default.',Panel='colorGradingPanel'},
  {Command='ResetSplitToningShadowHue',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowHue=Shadow Hue')),Group=colorGrading,Explanation='Reset color grading shadow hue to default.',Panel='colorGradingPanel'},
  {Command='ResetColorGradeShadowLum',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorGradeShadowLum=Shadow Luminance')),Group=colorGrading,Explanation='Reset color grading shadow luminance to default.',Panel='colorGradingPanel'},
  {Command='ResetSplitToningShadowSaturation',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowSaturation=Shadow Saturation')),Group=colorGrading,Explanation='Reset color grading shadow saturation to default.',Panel='colorGradingPanel'},
  --
  --develop: detail panel
  --
  {Command='RevealPanelDetail',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',detail),Group=detail,Explanation='Open Detail Panel in Develop Module.'},
  {Command='EnableDetail',Type='button',Translation=LOC('$$$/AgDevelop/Settings/ToggleDetail=Detail enable/disable'),Group=detail,Explanation='Enable or disable noise reduction and sharpening.',Panel='detailPanel'},
  {Command='Sharpness',Type='parameter',Translation=LOC('$$$/AgDevelop/Localized/Sharpness=Sharpness'),Group=detail,Explanation='Adjusts edge definition. Increase the Amount value to increase sharpening. A value of zero (0) turns off sharpening. In general, set Amount to a lower value for cleaner images. The adjustment locates pixels that differ from surrounding pixels based on the threshold you specify and increases the pixels\226\128\153 contrast by the amount you specify.',Panel='detailPanel'},
  {Command='SharpenRadius',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenRadius=Sharpen Radius'),Group=detail,Explanation='Adjusts the size of the details that sharpening is applied to. Photos with very fine details may need a lower radius setting. Photos with larger details may be able to use a larger radius. Using too large a radius generally results in unnatural-looking results.',Panel='detailPanel'},
  {Command='SharpenDetail',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenDetail=Sharpen Detail'),Group=detail,Explanation='Adjusts how much high-frequency information is sharpened in the image and how much the sharpening process emphasizes edges. Lower settings primarily sharpen edges to remove blurring. Higher values are useful for making the textures in the image more pronounced.',Panel='detailPanel'},
  {Command='SharpenEdgeMasking',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenEdgeMasking=Sharpen Edge Masking'),Group=detail,Explanation='Controls an edge mask. With a setting of zero (0), everything in the image receives the same amount of sharpening. With a setting of 100, sharpening is mostly restricted to those areas near the strongest edges.',Panel='detailPanel'},
  {Command='LuminanceSmoothing',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceSmoothing=Luminance Smoothing'),Group=detail,Explanation='Reduces luminance noise.',Panel='detailPanel'},
  {Command='LuminanceNoiseReductionDetail',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionDetail=Luminance Detail'),Group=detail,Explanation='Controls the luminance noise threshold. Useful for very noisy photos. Higher values preserve more detail but may produce noisier results. Lower values produce cleaner results but may also remove some detail.',Panel='detailPanel'},
  {Command='LuminanceNoiseReductionContrast',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionContrast=Luminance Contrast'),Group=detail,Explanation='Controls luminance contrast. Useful for very noisy photos. Higher values preserve contrast but may produce noisy blotches or mottling. Lower values produce smoother results but may also have less contrast.',Panel='detailPanel'},
  {Command='ColorNoiseReduction',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReduction=Color Noise Reduction'),Group=detail,Explanation='Reduces color noise.',Panel='detailPanel'},
  {Command='ColorNoiseReductionDetail',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionDetail=Color Noise Reduction Detail'),Group=detail,Explanation='Controls the color noise threshold. Higher values protect thin, detailed color edges but may result in color speckling. Lower values remove color speckles but may result in color bleeding.',Panel='detailPanel'},
  {Command='ColorNoiseReductionSmoothness',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionSmoothness=Color Noise Reduction Smoothness'),Group=detail,Explanation='Increasing this can prevent artifacts especially in the lower frequencies.',Panel='detailPanel'},
  {Command='ResetSharpness',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgDevelop/Localized/Sharpness=Sharpness')),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetSharpenRadius',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenRadius=Sharpen Radius')),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetSharpenDetail',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenDetail=Sharpen Detail')),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetSharpenEdgeMasking',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenEdgeMasking=Sharpen Edge Masking')),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetLuminanceSmoothing',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceSmoothing=Luminance Smoothing')),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetLuminanceNoiseReductionDetail',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionDetail=Luminance Detail')),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetLuminanceNoiseReductionContrast',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionContrast=Luminance Contrast')),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetColorNoiseReduction',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReduction=Color Noise Reduction')),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetColorNoiseReductionDetail',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionDetail=Color Noise Reduction Detail')),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetColorNoiseReductionSmoothness',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionSmoothness=Color Noise Reduction Smoothness')),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  --
  --develop: lens corrections panel
  --
  {Command='RevealPanelLens',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',lensCorrections),Group=lensCorrections,Explanation='Open Lens Corrections Panel in Develop Module.'},
  {Command='EnableLensCorrections',Type='button',Translation=LOC('$$$/AgDevelop/Settings/ToggleLensCorrections=Lens corrections enable/disable'),Group=lensCorrections,Explanation='Enable or disable all lens corrections.',Panel='lensCorrectionsPanel'},
  {Command='LensProfileEnable',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileEnable=Toggle Profile Corrections'),Group=lensCorrections,Explanation='Enable or disable lens profile correction.',Panel='lensCorrectionsPanel'},
  {Command='AutoLateralCA',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RemoveChromaticAberration=Remove Chromatic Aberration'),Group=lensCorrections,Explanation='Toggle Remove Chromatic Aberration.',Panel='lensCorrectionsPanel'},
  {Command='LensProfileDistortionScale',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileDistortionScale=Lens Profile Distortion Scale'),Group=lensCorrections,Explanation='The default value 100 applies 100% of the distortion correction in the profile. Values over 100 apply greater correction to the distortion; values under 100 apply less correction to the distortion.',Panel='lensCorrectionsPanel'},
  {Command='LensProfileChromaticAberrationScale',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/ChromaticAberration=chromatic Aberration')..' — '..LOC('$$$/AgCameraRawUI/ControlTitle/LensProfileDistortionScale=Scale'),Group=lensCorrections,Explanation='The default value 100 applies 100% of the chromatic aberration correction in the profile. Values over 100 apply greater correction to color fringing; values under 100 apply less correction to color fringing.',Panel='lensCorrectionsPanel'},
  {Command='LensProfileVignettingScale',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileVignettingScale=Lens Profile Vignetting Scale'),Group=lensCorrections,Explanation='The default value 100 applies 100% of the vignetting correction in the profile. Values over 100 apply greater correction to vignetting; values under 100 apply less correction to vignetting.',Panel='lensCorrectionsPanel'},
  {Command='ResetLensProfileDistortionScale',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileDistortionScale=Lens Profile Distortion Scale')),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetLensProfileChromaticAberrationScale',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/ChromaticAberration=chromatic Aberration'))..' — '..LOC('$$$/AgCameraRawUI/ControlTitle/LensProfileDistortionScale=Scale'),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetLensProfileVignettingScale',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileVignettingScale=Lens Profile Vignetting Scale')),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='DefringePurpleAmount',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeAmount=Defringe Purple Amount'),Group=lensCorrections,Explanation='You can adjust the affected purple or green hue range using the Purple Hue and the Green Hue sliders. Drag either end-point control to expand or decrease the range of affected colors. Drag between the end point controls to move the hue range. The minimum space between end points is ten units.',Panel='lensCorrectionsPanel'},
  {Command='DefringePurpleHueLo',Type='parameter',Translation=dph..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/Low=Low'),Group=lensCorrections,Explanation='See \226\128\152Defringe Purple Amount\226\128\153 description for details.',Panel='lensCorrectionsPanel'},
  {Command='DefringePurpleHueHi',Type='parameter',Translation=dph..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/High=High'),Group=lensCorrections,Explanation='See \226\128\152Defringe Purple Amount\226\128\153 description for details.',Panel='lensCorrectionsPanel'},
  {Command='DefringeGreenAmount',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeAmount=Defringe Green Amount'),Group=lensCorrections,Explanation='You can adjust the affected purple or green hue range using the Purple Hue and the Green Hue sliders. Drag either end-point control to expand or decrease the range of affected colors. Drag between the end point controls to move the hue range. The minimum space between end points is ten units. The default spacing for the green sliders is narrow to protect green/yellow image colors, like foliage.',Panel='lensCorrectionsPanel'},
  {Command='DefringeGreenHueLo',Type='parameter',Translation=dgh..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/Low=Low'),Group=lensCorrections,Explanation='See \226\128\152Defringe Green Amount\226\128\153 description for details.',Panel='lensCorrectionsPanel'},
  {Command='DefringeGreenHueHi',Type='parameter',Translation=dgh..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/High=High'),Group=lensCorrections,Explanation='See \226\128\152Defringe Green Amount\226\128\153 description for details.',Panel='lensCorrectionsPanel'},
  {Command='ResetDefringePurpleAmount',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeAmount=Defringe Purple Amount')),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetDefringePurpleHueLo',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',dph..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/Low=Low')),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetDefringePurpleHueHi',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',dph..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/High=High')),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetDefringeGreenAmount',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeAmount=Defringe Green Amount')),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetDefringeGreenHueLo',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',dgh..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/Low=Low')),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetDefringeGreenHueHi',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',dgh..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/High=High')),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='LensManualDistortionAmount',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensManualDistortionAmount=Lens Manual Distortion Amount'),Group=lensCorrections,Explanation='Drag to the right to correct barrel distortion and straighten lines that bend away from the center. Drag to the left to correct pincushion distortion and straighten lines that bend toward the center.',Panel='lensCorrectionsPanel'},
  {Command='VignetteAmount',Type='parameter',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteAmount=Vignette Amount'),Group=lensCorrections,Explanation='Sets the amount of lightening or darkening along the edges of an image. Corrects images that have darkened corners caused by lens faults or improper lens shading.',Panel='lensCorrectionsPanel'},
  {Command='VignetteMidpoint',Type='parameter',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteMidpoint=Vignette Midpoint'),Group=lensCorrections,Explanation='Specifies the width of area affected by the Amount slider. Specify a lower number to affect more of the image. Specify a higher number to restrict the effect to the edges of the image.',Panel='lensCorrectionsPanel'},
  {Command='ResetLensManualDistortionAmount',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensManualDistortionAmount=Lens Manual Distortion Amount')),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetVignetteAmount',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteAmount=Vignette Amount')),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetVignetteMidpoint',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteMidpoint=Vignette Midpoint')),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  --
  --develop: transform panel menu
  --
  {Command='RevealPanelTransform',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',transform),Group=transform,Explanation='Open Transform Panel in Develop Module.'},
  {Command='EnableTransform',Type='button',Translation=LOC('$$$/AgDevelop/Settings/ToggleTransform=Transform enable/disable'),Group=transform,Explanation='Enable or disable all transforms.',Panel='transformPanel'},
  {Command='UprightOff',Type='button',Translation=percor..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ValueOff=Off'),Group=transform,Explanation='Upright mode off.',Panel='transformPanel'},
  {Command='UprightAuto',Type='button',Translation=percor..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ValueAuto=Auto'),Group=transform,Explanation='Balanced level, aspect ratio, and perspective corrections.',Panel='transformPanel'},
  {Command='UprightLevel',Type='button',Translation=percor..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ValueLevel=Level'),Group=transform,Explanation='Perspective corrections are weighted toward horizontal details.',Panel='transformPanel'},
  {Command='UprightVertical',Type='button',Translation=percor..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ValueLevelVertical=Vertical'),Group=transform,Explanation='Perspective corrections are weighted toward vertical details.',Panel='transformPanel'},
  {Command='UprightGuided',Type='button',Translation=percor..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ValueLevelGuided=Guided'),Group=transform,Explanation='Perspective corrections guide by drawing lines.',Panel='transformPanel'},
  {Command='UprightFull',Type='button',Translation=percor..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ValueFull=Full'),Group=transform,Explanation='Combination of full Level, Vertical, and Auto perspective corrections.',Panel='transformPanel'},
  {Command='CropConstrainToWarp',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CropConstrainToWarp=Constrain to Warp'),Group=transform,Explanation='Enable or disable Constrain Crop. Takes effect in both Transforms panel and Crop panel.',Panel='transformPanel'},
  {Command='ResetPerspectiveUpright',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/Ops/ResetUprightSettings=Reset Upright Settings'),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
  {Command='PerspectiveVertical',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveVertical=Perspective Vertical'),Group=transform,Explanation='Corrects perspective caused by tilting the camera up or down. Makes vertical lines appear parallel.',Panel='transformPanel'},
  {Command='PerspectiveHorizontal',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveHorizontal=Perspective Horizontal'),Group=transform,Explanation='Corrects perspective caused by angling the camera left or right. Makes horizontal lines parallel.',Panel='transformPanel'},
  {Command='PerspectiveRotate',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveRotate=Perspective Rotate'),Group=transform,Explanation='Corrects for camera tilt. Uses the center of the original, uncropped photo as the axis of rotation.',Panel='transformPanel'},
  {Command='PerspectiveScale',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveScale=Perspective Scale'),Group=transform,Explanation='Adjusts the image scale up or down. Helps to remove empty areas caused by perspective corrections and distortions. Displays areas of the image that extend beyond the crop boundary.',Panel='transformPanel'},
  {Command='PerspectiveAspect',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveAspect=Perspective Aspect'),Group=transform,Explanation='Adjusts the amount the image is stretched horizontally or vertically.',Panel='transformPanel'},
  {Command='PerspectiveX',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveX=Perspective X'),Group=transform,Explanation='Moves the center of the image laterally.',Panel='transformPanel'},
  {Command='PerspectiveY',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveY=Perspective Y'),Group=transform,Explanation='Moves the center of the image vertically.',Panel='transformPanel'},
  {Command='ResetTransforms',Type='button',Translation=LOC("$$$/AgLibrary/Ops/ResetTransforms=Reset all transforms"),Group=transform,Explanation='Clears all transforms from the current photo. Must be called while the Develop module is active.',Panel='transformPanel'},
  {Command='ResetPerspectiveVertical',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveVertical=Perspective Vertical')),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
  {Command='ResetPerspectiveHorizontal',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveHorizontal=Perspective Horizontal')),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
  {Command='ResetPerspectiveRotate',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveRotate=Perspective Rotate')),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
  {Command='ResetPerspectiveScale',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveScale=Perspective Scale')),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
  {Command='ResetPerspectiveAspect',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveAspect=Perspective Aspect')),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
  {Command='ResetPerspectiveX',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveX=Perspective X')),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
  {Command='ResetPerspectiveY',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveY=Perspective Y')),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
  --
  --develop: effects panel
  --
  {Command='RevealPanelEffects',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',effects),Group=effects,Explanation='Open Effects Panel in Develop Module.'},
  {Command='EnableEffects',Type='button',Translation=LOC('$$$/AgDevelop/Settings/ToggleEffects=Effects enable/disable'),Group=effects,Explanation='Enable or disable effects.',Panel='effectsPanel'},
  {Command='PostCropVignetteAmount',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteAmount=Post Crop Vignette Amount'),Group=effects,Explanation='Negative values darken the corners of the photo. Positive values lighten the corners.',Panel='effectsPanel'},
  {Command='PostCropVignetteMidpoint',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteMidpoint=Post Crop Vignette Midpoint'),Group=effects,Explanation='Lower values apply the Amount adjustment to a larger area away from the corners. Higher values restrict the adjustment to an area closer to the corners.',Panel='effectsPanel'},
  {Command='PostCropVignetteFeather',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteFeather=Post Crop Vignette Feather'),Group=effects,Explanation='Lower values reduce softening between the vignette and the vignette\226\128\153s surrounding pixels. Higher values increase the softening.',Panel='effectsPanel'},
  {Command='PostCropVignetteRoundness',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteRoundness=Post Crop Vignette Roundness'),Group=effects,Explanation='Lower values make the vignette effect more oval. Higher values make the vignette effect more circular.',Panel='effectsPanel'},
  {Command='PostCropVignetteStyle',Type='variable',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteStyle=Post Crop Vignette Style'),Group=effects,Explanation='Cycles through: **Highlight Priority** Enables highlight recovery but can lead to color shifts in darkened areas of a photo. Suitable for photos with bright image areas such as clipped specular highlights. **Color Priority** Minimizes color shifts in darkened areas of a photo but cannot perform highlight recovery. **Paint Overlay** Mixes the cropped image values with black or white pixels. Can result in a flat appearance.',Panel='effectsPanel'},
  {Command='PostCropVignetteStyleHighlightPriority',Type='button',Translation=LOC('$$$/AgCameraRawController/PostCropVignetteStyle/Exposure1=Highlight Priority'),Group=effects,Explanation='Post Crop Vignette Style: Highlight Priority.',Panel='effectsPanel'},
  {Command='PostCropVignetteStyleColorPriority',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawController/PostCropVignetteStyle/Exposure2=Color Priority'),Group=effects,Explanation='Post Crop Vignette Style: Color Priority.',Panel='effectsPanel'},
  {Command='PostCropVignetteStylePaintOverlay',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawController/PostCropVignetteStyle/PaintOverlay=Paint Overlay'),Group=effects,Explanation='Post Crop Vignette Style: Paint Overlay.',Panel='effectsPanel'},
  {Command='PostCropVignetteHighlightContrast',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteHighlightContrast=Post Crop Vignette Highlight Contrast'),Group=effects,Explanation='(Highlight Priority and Color Priority only) Controls the degree of highlight contrast preserved when Amount is negative. Suitable for photos with small highlights, such as candles and lamps.',Panel='effectsPanel'},
  {Command='GrainAmount',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainAmount=Grain Amount'),Group=effects,Explanation='Controls the amount of grain applied to the image. Drag to the right to increase the amount. Set to zero to disable grain.',Panel='effectsPanel'},
  {Command='GrainSize',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainSize=Grain Size'),Group=effects,Explanation='Controls grain particle size. At sizes of 25 or greater, blue is added to make the effect look better with noise reduction.',Panel='effectsPanel'},
  {Command='GrainFrequency',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainRoughness=Grain Roughness'),Group=effects,Explanation=' Controls the regularity of the grain. Drag to the left to make the grain more uniform; drag to the right to make the grain more uneven.',Panel='effectsPanel'},
  {Command='ResetPostCropVignetteAmount',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteAmount=Post Crop Vignette Amount')),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetPostCropVignetteMidpoint',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteMidpoint=Post Crop Vignette Midpoint')),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetPostCropVignetteFeather',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteFeather=Post Crop Vignette Feather')),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetPostCropVignetteRoundness',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteRoundness=Post Crop Vignette Roundness')),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetPostCropVignetteStyle',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteStyle=Post Crop Vignette Style')),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetPostCropVignetteHighlightContrast',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteHighlightContrast=Post Crop Vignette Highlight Contrast')),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetGrainAmount',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainAmount=Grain Amount')),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetGrainSize',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainSize=Grain Size')),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetGrainFrequency',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainRoughness=Grain Roughness')),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  --
  --develop: calibration panel
  --
  {Command='RevealPanelCalibrate',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',calibration),Group=calibration,Explanation='Open Camera Calibration Panel in Develop Module.'},
  {Command='EnableCalibration',Type='button',Translation=LOC('$$$/AgDevelop/Settings/ToggleCalibration=Calibration enable/disable'),Group=calibration,Explanation='Enable or disable custom camera calibration.',Panel='calibratePanel'},
  {Command='ProfileAmount',Type='variable',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ProfileAmount=Profile amount'),Group=calibration,Explanation='Varies amount of custom calibration applied. **Note**: The MIDI controller does not get updated when the value is changed directly in Lightroom, either by using a mouse or when changing images. Pickup mode does not affect behavior of this adjustment. Also, Lightroom may complain of being unable to update the value if too many changes occur in a short period of time. Just dismiss those warnings and continue.', Panel='calibratePanel'},
  {Command='ShadowTint',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ShadowTintCalibration=Shadow Tint Calibration'),Group=calibration,Explanation='Corrects for any green or magenta tint in the shadow areas of the photo.',Panel='calibratePanel'},
  {Command='RedHue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedHueCalibration=Red Hue Calibration'),Group=calibration,Explanation='For the red primary. Moving the Hue slider to the left (negative value) is similar to a counterclockwise move on the color wheel; moving it to the right (positive value) is similar to a clockwise move.',Panel='calibratePanel'},
  {Command='RedSaturation',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedSaturationCalibration=Red Saturation Calibration'),Group=calibration,Explanation='For the red primary. Moving the Saturation slider to the left (negative value) desaturates the color; moving it to the right (positive value) increases saturation.',Panel='calibratePanel'},
  {Command='GreenHue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenHueCalibration=Green Hue Calibration'),Group=calibration,Explanation='For the green primary. Moving the Hue slider to the left (negative value) is similar to a counterclockwise move on the color wheel; moving it to the right (positive value) is similar to a clockwise move.',Panel='calibratePanel'},
  {Command='GreenSaturation',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenSaturationCalibration=Green Saturation Calibration'),Group=calibration,Explanation='For the green primary. Moving the Saturation slider to the left (negative value) desaturates the color; moving it to the right (positive value) increases saturation.',Panel='calibratePanel'},
  {Command='BlueHue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueHueCalibration=Blue Hue Calibration'),Group=calibration,Explanation='For the blue primary. Moving the Hue slider to the left (negative value) is similar to a counterclockwise move on the color wheel; moving it to the right (positive value) is similar to a clockwise move.',Panel='calibratePanel'},
  {Command='BlueSaturation',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueSaturationCalibration=Blue Saturation Calibration'),Group=calibration,Explanation='For the blue primary. Moving the Saturation slider to the left (negative value) desaturates the color; moving it to the right (positive value) increases saturation.',Panel='calibratePanel'},
  {Command='ResetShadowTint',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ShadowTintCalibration=Shadow Tint Calibration')),Group=calibration,Explanation='Reset to default.',Panel='calibratePanel'},
  {Command='ResetRedHue',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedHueCalibration=Red Hue Calibration')),Group=calibration,Explanation='Reset to default.',Panel='calibratePanel'},
  {Command='ResetRedSaturation',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedSaturationCalibration=Red Saturation Calibration')),Group=calibration,Explanation='Reset to default.',Panel='calibratePanel'},
  {Command='ResetGreenHue',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenHueCalibration=Green Hue Calibration')),Group=calibration,Explanation='Reset to default.',Panel='calibratePanel'},
  {Command='ResetGreenSaturation',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenSaturationCalibration=Green Saturation Calibration')),Group=calibration,Explanation='Reset to default.',Panel='calibratePanel'},
  {Command='ResetBlueHue',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueHueCalibration=Blue Hue Calibration')),Group=calibration,Explanation='Reset to default.',Panel='calibratePanel'},
  {Command='ResetBlueSaturation',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueSaturationCalibration=Blue Saturation Calibration')),Group=calibration,Explanation='Reset to default.',Panel='calibratePanel'},
  --
  --develop: develop presets
  --
  {Command='PresetPrevious',Type='button',Translation=LOC('$$$/MIDI2LR/Menu/PresetPrevious=Previous preset'),Group=developPresets,Explanation='Apply previous preset (in numerical order) to active photo only.'},
  {Command='PresetNext',Type='button',Translation=LOC('$$$/MIDI2LR/Menu/PresetNext=Next preset'),Group=developPresets,Explanation='Apply next preset (in numerical order) to active photo only.'},
  {Command='PresetPreviousNext',Type='repeat',Translation=LOC('$$$/MIDI2LR/Menu/PresetPrevious=Previous preset')..'—'..LOC('$$$/MIDI2LR/Menu/PresetNext=Next preset'),Group=developPresets,Explanation='Turning knob clockwise applies next preset, counterclockwise, previous.'..repeatexp,Repeats={'PresetNext','PresetPrevious'}},
  {Command='PresetAmount',Type='parameter',Translation=LOC('$$$/AgDevelop/CameraRawPanel/LensCorrection/Amount=Amount'),Group=developPresets,Explanation='Vary amount of preset applied. Only works when preset amount slider is present.',AltParameter='Direct'},
  {Command='Preset_1',Type='button',Translation=developPreset..' 1',Group=developPresets,Explanation='Apply preset 1 to active photo only.'},
  {Command='Preset_2',Type='button',Translation=developPreset..' 2',Group=developPresets,Explanation='Apply preset 2 to active photo only.'},
  {Command='Preset_3',Type='button',Translation=developPreset..' 3',Group=developPresets,Explanation='Apply preset 3 to active photo only.'},
  {Command='Preset_4',Type='button',Translation=developPreset..' 4',Group=developPresets,Explanation='Apply preset 4 to active photo only.'},
  {Command='Preset_5',Type='button',Translation=developPreset..' 5',Group=developPresets,Explanation='Apply preset 5 to active photo only.'},
  {Command='Preset_6',Type='button',Translation=developPreset..' 6',Group=developPresets,Explanation='Apply preset 6 to active photo only.'},
  {Command='Preset_7',Type='button',Translation=developPreset..' 7',Group=developPresets,Explanation='Apply preset 7 to active photo only.'},
  {Command='Preset_8',Type='button',Translation=developPreset..' 8',Group=developPresets,Explanation='Apply preset 8 to active photo only.'},
  {Command='Preset_9',Type='button',Translation=developPreset..' 9',Group=developPresets,Explanation='Apply preset 9 to active photo only.'},
  {Command='Preset_10',Type='button',Translation=developPreset..' 10',Group=developPresets,Explanation='Apply preset 10 to active photo only.'},
  {Command='Preset_11',Type='button',Translation=developPreset..' 11',Group=developPresets,Explanation='Apply preset 11 to active photo only.'},
  {Command='Preset_12',Type='button',Translation=developPreset..' 12',Group=developPresets,Explanation='Apply preset 12 to active photo only.'},
  {Command='Preset_13',Type='button',Translation=developPreset..' 13',Group=developPresets,Explanation='Apply preset 13 to active photo only.'},
  {Command='Preset_14',Type='button',Translation=developPreset..' 14',Group=developPresets,Explanation='Apply preset 14 to active photo only.'},
  {Command='Preset_15',Type='button',Translation=developPreset..' 15',Group=developPresets,Explanation='Apply preset 15 to active photo only.'},
  {Command='Preset_16',Type='button',Translation=developPreset..' 16',Group=developPresets,Explanation='Apply preset 16 to active photo only.'},
  {Command='Preset_17',Type='button',Translation=developPreset..' 17',Group=developPresets,Explanation='Apply preset 17 to active photo only.'},
  {Command='Preset_18',Type='button',Translation=developPreset..' 18',Group=developPresets,Explanation='Apply preset 18 to active photo only.'},
  {Command='Preset_19',Type='button',Translation=developPreset..' 19',Group=developPresets,Explanation='Apply preset 19 to active photo only.'},
  {Command='Preset_20',Type='button',Translation=developPreset..' 20',Group=developPresets,Explanation='Apply preset 20 to active photo only.'},
  {Command='Preset_21',Type='button',Translation=developPreset..' 21',Group=developPresets,Explanation='Apply preset 21 to active photo only.'},
  {Command='Preset_22',Type='button',Translation=developPreset..' 22',Group=developPresets,Explanation='Apply preset 22 to active photo only.'},
  {Command='Preset_23',Type='button',Translation=developPreset..' 23',Group=developPresets,Explanation='Apply preset 23 to active photo only.'},
  {Command='Preset_24',Type='button',Translation=developPreset..' 24',Group=developPresets,Explanation='Apply preset 24 to active photo only.'},
  {Command='Preset_25',Type='button',Translation=developPreset..' 25',Group=developPresets,Explanation='Apply preset 25 to active photo only.'},
  {Command='Preset_26',Type='button',Translation=developPreset..' 26',Group=developPresets,Explanation='Apply preset 26 to active photo only.'},
  {Command='Preset_27',Type='button',Translation=developPreset..' 27',Group=developPresets,Explanation='Apply preset 27 to active photo only.'},
  {Command='Preset_28',Type='button',Translation=developPreset..' 28',Group=developPresets,Explanation='Apply preset 28 to active photo only.'},
  {Command='Preset_29',Type='button',Translation=developPreset..' 29',Group=developPresets,Explanation='Apply preset 29 to active photo only.'},
  {Command='Preset_30',Type='button',Translation=developPreset..' 30',Group=developPresets,Explanation='Apply preset 30 to active photo only.'},
  {Command='Preset_31',Type='button',Translation=developPreset..' 31',Group=developPresets,Explanation='Apply preset 31 to active photo only.'},
  {Command='Preset_32',Type='button',Translation=developPreset..' 32',Group=developPresets,Explanation='Apply preset 32 to active photo only.'},
  {Command='Preset_33',Type='button',Translation=developPreset..' 33',Group=developPresets,Explanation='Apply preset 33 to active photo only.'},
  {Command='Preset_34',Type='button',Translation=developPreset..' 34',Group=developPresets,Explanation='Apply preset 34 to active photo only.'},
  {Command='Preset_35',Type='button',Translation=developPreset..' 35',Group=developPresets,Explanation='Apply preset 35 to active photo only.'},
  {Command='Preset_36',Type='button',Translation=developPreset..' 36',Group=developPresets,Explanation='Apply preset 36 to active photo only.'},
  {Command='Preset_37',Type='button',Translation=developPreset..' 37',Group=developPresets,Explanation='Apply preset 37 to active photo only.'},
  {Command='Preset_38',Type='button',Translation=developPreset..' 38',Group=developPresets,Explanation='Apply preset 38 to active photo only.'},
  {Command='Preset_39',Type='button',Translation=developPreset..' 39',Group=developPresets,Explanation='Apply preset 39 to active photo only.'},
  {Command='Preset_40',Type='button',Translation=developPreset..' 40',Group=developPresets,Explanation='Apply preset 40 to active photo only.'},
  {Command='Preset_41',Type='button',Translation=developPreset..' 41',Group=developPresets,Explanation='Apply preset 41 to active photo only.'},
  {Command='Preset_42',Type='button',Translation=developPreset..' 42',Group=developPresets,Explanation='Apply preset 42 to active photo only.'},
  {Command='Preset_43',Type='button',Translation=developPreset..' 43',Group=developPresets,Explanation='Apply preset 43 to active photo only.'},
  {Command='Preset_44',Type='button',Translation=developPreset..' 44',Group=developPresets,Explanation='Apply preset 44 to active photo only.'},
  {Command='Preset_45',Type='button',Translation=developPreset..' 45',Group=developPresets,Explanation='Apply preset 45 to active photo only.'},
  {Command='Preset_46',Type='button',Translation=developPreset..' 46',Group=developPresets,Explanation='Apply preset 46 to active photo only.'},
  {Command='Preset_47',Type='button',Translation=developPreset..' 47',Group=developPresets,Explanation='Apply preset 47 to active photo only.'},
  {Command='Preset_48',Type='button',Translation=developPreset..' 48',Group=developPresets,Explanation='Apply preset 48 to active photo only.'},
  {Command='Preset_49',Type='button',Translation=developPreset..' 49',Group=developPresets,Explanation='Apply preset 49 to active photo only.'},
  {Command='Preset_50',Type='button',Translation=developPreset..' 50',Group=developPresets,Explanation='Apply preset 50 to active photo only.'},
  {Command='Preset_51',Type='button',Translation=developPreset..' 51',Group=developPresets,Explanation='Apply preset 51 to active photo only.'},
  {Command='Preset_52',Type='button',Translation=developPreset..' 52',Group=developPresets,Explanation='Apply preset 52 to active photo only.'},
  {Command='Preset_53',Type='button',Translation=developPreset..' 53',Group=developPresets,Explanation='Apply preset 53 to active photo only.'},
  {Command='Preset_54',Type='button',Translation=developPreset..' 54',Group=developPresets,Explanation='Apply preset 54 to active photo only.'},
  {Command='Preset_55',Type='button',Translation=developPreset..' 55',Group=developPresets,Explanation='Apply preset 55 to active photo only.'},
  {Command='Preset_56',Type='button',Translation=developPreset..' 56',Group=developPresets,Explanation='Apply preset 56 to active photo only.'},
  {Command='Preset_57',Type='button',Translation=developPreset..' 57',Group=developPresets,Explanation='Apply preset 57 to active photo only.'},
  {Command='Preset_58',Type='button',Translation=developPreset..' 58',Group=developPresets,Explanation='Apply preset 58 to active photo only.'},
  {Command='Preset_59',Type='button',Translation=developPreset..' 59',Group=developPresets,Explanation='Apply preset 59 to active photo only.'},
  {Command='Preset_60',Type='button',Translation=developPreset..' 60',Group=developPresets,Explanation='Apply preset 60 to active photo only.'},
  {Command='Preset_61',Type='button',Translation=developPreset..' 61',Group=developPresets,Explanation='Apply preset 61 to active photo only.'},
  {Command='Preset_62',Type='button',Translation=developPreset..' 62',Group=developPresets,Explanation='Apply preset 62 to active photo only.'},
  {Command='Preset_63',Type='button',Translation=developPreset..' 63',Group=developPresets,Explanation='Apply preset 63 to active photo only.'},
  {Command='Preset_64',Type='button',Translation=developPreset..' 64',Group=developPresets,Explanation='Apply preset 64 to active photo only.'},
  {Command='Preset_65',Type='button',Translation=developPreset..' 65',Group=developPresets,Explanation='Apply preset 65 to active photo only.'},
  {Command='Preset_66',Type='button',Translation=developPreset..' 66',Group=developPresets,Explanation='Apply preset 66 to active photo only.'},
  {Command='Preset_67',Type='button',Translation=developPreset..' 67',Group=developPresets,Explanation='Apply preset 67 to active photo only.'},
  {Command='Preset_68',Type='button',Translation=developPreset..' 68',Group=developPresets,Explanation='Apply preset 68 to active photo only.'},
  {Command='Preset_69',Type='button',Translation=developPreset..' 69',Group=developPresets,Explanation='Apply preset 69 to active photo only.'},
  {Command='Preset_70',Type='button',Translation=developPreset..' 70',Group=developPresets,Explanation='Apply preset 70 to active photo only.'},
  {Command='Preset_71',Type='button',Translation=developPreset..' 71',Group=developPresets,Explanation='Apply preset 71 to active photo only.'},
  {Command='Preset_72',Type='button',Translation=developPreset..' 72',Group=developPresets,Explanation='Apply preset 72 to active photo only.'},
  {Command='Preset_73',Type='button',Translation=developPreset..' 73',Group=developPresets,Explanation='Apply preset 73 to active photo only.'},
  {Command='Preset_74',Type='button',Translation=developPreset..' 74',Group=developPresets,Explanation='Apply preset 74 to active photo only.'},
  {Command='Preset_75',Type='button',Translation=developPreset..' 75',Group=developPresets,Explanation='Apply preset 75 to active photo only.'},
  {Command='Preset_76',Type='button',Translation=developPreset..' 76',Group=developPresets,Explanation='Apply preset 76 to active photo only.'},
  {Command='Preset_77',Type='button',Translation=developPreset..' 77',Group=developPresets,Explanation='Apply preset 77 to active photo only.'},
  {Command='Preset_78',Type='button',Translation=developPreset..' 78',Group=developPresets,Explanation='Apply preset 78 to active photo only.'},
  {Command='Preset_79',Type='button',Translation=developPreset..' 79',Group=developPresets,Explanation='Apply preset 79 to active photo only.'},
  {Command='Preset_80',Type='button',Translation=developPreset..' 80',Group=developPresets,Explanation='Apply preset 80 to active photo only.'},

  --
  --keywords
  --

  {Command='Keyword1',Type='button',Translation=keyword..' 1',Group=keywords,Explanation='Apply keyword 1 to all selected photos.'},
  {Command='Keyword2',Type='button',Translation=keyword..' 2',Group=keywords,Explanation='Apply keyword 2 to all selected photos.'},
  {Command='Keyword3',Type='button',Translation=keyword..' 3',Group=keywords,Explanation='Apply keyword 3 to all selected photos.'},
  {Command='Keyword4',Type='button',Translation=keyword..' 4',Group=keywords,Explanation='Apply keyword 4 to all selected photos.'},
  {Command='Keyword5',Type='button',Translation=keyword..' 5',Group=keywords,Explanation='Apply keyword 5 to all selected photos.'},
  {Command='Keyword6',Type='button',Translation=keyword..' 6',Group=keywords,Explanation='Apply keyword 6 to all selected photos.'},
  {Command='Keyword7',Type='button',Translation=keyword..' 7',Group=keywords,Explanation='Apply keyword 7 to all selected photos.'},
  {Command='Keyword8',Type='button',Translation=keyword..' 8',Group=keywords,Explanation='Apply keyword 8 to all selected photos.'},
  {Command='Keyword9',Type='button',Translation=keyword..' 9',Group=keywords,Explanation='Apply keyword 9 to all selected photos.'},
  {Command='Keyword10',Type='button',Translation=keyword..' 10',Group=keywords,Explanation='Apply keyword 10 to all selected photos.'},
  {Command='Keyword11',Type='button',Translation=keyword..' 11',Group=keywords,Explanation='Apply keyword 11 to all selected photos.'},
  {Command='Keyword12',Type='button',Translation=keyword..' 12',Group=keywords,Explanation='Apply keyword 12 to all selected photos.'},
  {Command='Keyword13',Type='button',Translation=keyword..' 13',Group=keywords,Explanation='Apply keyword 13 to all selected photos.'},
  {Command='Keyword14',Type='button',Translation=keyword..' 14',Group=keywords,Explanation='Apply keyword 14 to all selected photos.'},
  {Command='Keyword15',Type='button',Translation=keyword..' 15',Group=keywords,Explanation='Apply keyword 15 to all selected photos.'},
  {Command='Keyword16',Type='button',Translation=keyword..' 16',Group=keywords,Explanation='Apply keyword 16 to all selected photos.'},
  {Command='Keyword17',Type='button',Translation=keyword..' 17',Group=keywords,Explanation='Apply keyword 17 to all selected photos.'},
  {Command='Keyword18',Type='button',Translation=keyword..' 18',Group=keywords,Explanation='Apply keyword 18 to all selected photos.'},
  {Command='Keyword19',Type='button',Translation=keyword..' 19',Group=keywords,Explanation='Apply keyword 19 to all selected photos.'},
  {Command='Keyword20',Type='button',Translation=keyword..' 20',Group=keywords,Explanation='Apply keyword 20 to all selected photos.'},
  {Command='Keyword21',Type='button',Translation=keyword..' 21',Group=keywords,Explanation='Apply keyword 21 to all selected photos.'},
  {Command='Keyword22',Type='button',Translation=keyword..' 22',Group=keywords,Explanation='Apply keyword 22 to all selected photos.'},
  {Command='Keyword23',Type='button',Translation=keyword..' 23',Group=keywords,Explanation='Apply keyword 23 to all selected photos.'},
  {Command='Keyword24',Type='button',Translation=keyword..' 24',Group=keywords,Explanation='Apply keyword 24 to all selected photos.'},
  {Command='Keyword25',Type='button',Translation=keyword..' 25',Group=keywords,Explanation='Apply keyword 25 to all selected photos.'},
  {Command='Keyword26',Type='button',Translation=keyword..' 26',Group=keywords,Explanation='Apply keyword 26 to all selected photos.'},
  {Command='Keyword27',Type='button',Translation=keyword..' 27',Group=keywords,Explanation='Apply keyword 27 to all selected photos.'},
  {Command='Keyword28',Type='button',Translation=keyword..' 28',Group=keywords,Explanation='Apply keyword 28 to all selected photos.'},
  {Command='Keyword29',Type='button',Translation=keyword..' 29',Group=keywords,Explanation='Apply keyword 29 to all selected photos.'},
  {Command='Keyword30',Type='button',Translation=keyword..' 30',Group=keywords,Explanation='Apply keyword 30 to all selected photos.'},
  {Command='Keyword31',Type='button',Translation=keyword..' 31',Group=keywords,Explanation='Apply keyword 31 to all selected photos.'},
  {Command='Keyword32',Type='button',Translation=keyword..' 32',Group=keywords,Explanation='Apply keyword 32 to all selected photos.'},
  {Command='Keyword33',Type='button',Translation=keyword..' 33',Group=keywords,Explanation='Apply keyword 33 to all selected photos.'},
  {Command='Keyword34',Type='button',Translation=keyword..' 34',Group=keywords,Explanation='Apply keyword 34 to all selected photos.'},
  {Command='Keyword35',Type='button',Translation=keyword..' 35',Group=keywords,Explanation='Apply keyword 35 to all selected photos.'},
  {Command='Keyword36',Type='button',Translation=keyword..' 36',Group=keywords,Explanation='Apply keyword 36 to all selected photos.'},
  {Command='Keyword37',Type='button',Translation=keyword..' 37',Group=keywords,Explanation='Apply keyword 37 to all selected photos.'},
  {Command='Keyword38',Type='button',Translation=keyword..' 38',Group=keywords,Explanation='Apply keyword 38 to all selected photos.'},
  {Command='Keyword39',Type='button',Translation=keyword..' 39',Group=keywords,Explanation='Apply keyword 39 to all selected photos.'},
  {Command='Keyword40',Type='button',Translation=keyword..' 40',Group=keywords,Explanation='Apply keyword 40 to all selected photos.'},
  {Command='Keyword41',Type='button',Translation=keyword..' 41',Group=keywords,Explanation='Apply keyword 41 to all selected photos.'},
  {Command='Keyword42',Type='button',Translation=keyword..' 42',Group=keywords,Explanation='Apply keyword 42 to all selected photos.'},
  {Command='Keyword43',Type='button',Translation=keyword..' 43',Group=keywords,Explanation='Apply keyword 43 to all selected photos.'},
  {Command='Keyword44',Type='button',Translation=keyword..' 44',Group=keywords,Explanation='Apply keyword 44 to all selected photos.'},
  {Command='Keyword45',Type='button',Translation=keyword..' 45',Group=keywords,Explanation='Apply keyword 45 to all selected photos.'},
  {Command='Keyword46',Type='button',Translation=keyword..' 46',Group=keywords,Explanation='Apply keyword 46 to all selected photos.'},
  {Command='Keyword47',Type='button',Translation=keyword..' 47',Group=keywords,Explanation='Apply keyword 47 to all selected photos.'},
  {Command='Keyword48',Type='button',Translation=keyword..' 48',Group=keywords,Explanation='Apply keyword 48 to all selected photos.'},
  {Command='Keyword49',Type='button',Translation=keyword..' 49',Group=keywords,Explanation='Apply keyword 49 to all selected photos.'},
  {Command='Keyword50',Type='button',Translation=keyword..' 50',Group=keywords,Explanation='Apply keyword 50 to all selected photos.'},
  {Command='Keyword51',Type='button',Translation=keyword..' 51',Group=keywords,Explanation='Apply keyword 51 to all selected photos.'},
  {Command='Keyword52',Type='button',Translation=keyword..' 52',Group=keywords,Explanation='Apply keyword 52 to all selected photos.'},
  {Command='Keyword53',Type='button',Translation=keyword..' 53',Group=keywords,Explanation='Apply keyword 53 to all selected photos.'},
  {Command='Keyword54',Type='button',Translation=keyword..' 54',Group=keywords,Explanation='Apply keyword 54 to all selected photos.'},
  {Command='Keyword55',Type='button',Translation=keyword..' 55',Group=keywords,Explanation='Apply keyword 55 to all selected photos.'},
  {Command='Keyword56',Type='button',Translation=keyword..' 56',Group=keywords,Explanation='Apply keyword 56 to all selected photos.'},
  {Command='Keyword57',Type='button',Translation=keyword..' 57',Group=keywords,Explanation='Apply keyword 57 to all selected photos.'},
  {Command='Keyword58',Type='button',Translation=keyword..' 58',Group=keywords,Explanation='Apply keyword 58 to all selected photos.'},
  {Command='Keyword59',Type='button',Translation=keyword..' 59',Group=keywords,Explanation='Apply keyword 59 to all selected photos.'},
  {Command='Keyword60',Type='button',Translation=keyword..' 60',Group=keywords,Explanation='Apply keyword 60 to all selected photos.'},
  {Command='Keyword61',Type='button',Translation=keyword..' 61',Group=keywords,Explanation='Apply keyword 61 to all selected photos.'},
  {Command='Keyword62',Type='button',Translation=keyword..' 62',Group=keywords,Explanation='Apply keyword 62 to all selected photos.'},
  {Command='Keyword63',Type='button',Translation=keyword..' 63',Group=keywords,Explanation='Apply keyword 63 to all selected photos.'},
  {Command='Keyword64',Type='button',Translation=keyword..' 64',Group=keywords,Explanation='Apply keyword 64 to all selected photos.'},

  --
  --toggle keywords
  --

  {Command='Keyword1Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','1'),Group=keywordtoggle,Explanation='Toggle keyword 1 in each of the selected photos.'},
  {Command='Keyword2Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','2'),Group=keywordtoggle,Explanation='Toggle keyword 2 in each of the selected photos.'},
  {Command='Keyword3Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','3'),Group=keywordtoggle,Explanation='Toggle keyword 3 in each of the selected photos.'},
  {Command='Keyword4Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','4'),Group=keywordtoggle,Explanation='Toggle keyword 4 in each of the selected photos.'},
  {Command='Keyword5Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','5'),Group=keywordtoggle,Explanation='Toggle keyword 5 in each of the selected photos.'},
  {Command='Keyword6Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','6'),Group=keywordtoggle,Explanation='Toggle keyword 6 in each of the selected photos.'},
  {Command='Keyword7Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','7'),Group=keywordtoggle,Explanation='Toggle keyword 7 in each of the selected photos.'},
  {Command='Keyword8Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','8'),Group=keywordtoggle,Explanation='Toggle keyword 8 in each of the selected photos.'},
  {Command='Keyword9Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','9'),Group=keywordtoggle,Explanation='Toggle keyword 9 in each of the selected photos.'},
  {Command='Keyword10Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','10'),Group=keywordtoggle,Explanation='Toggle keyword 10 in each of the selected photos.'},
  {Command='Keyword11Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','11'),Group=keywordtoggle,Explanation='Toggle keyword 11 in each of the selected photos.'},
  {Command='Keyword12Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','12'),Group=keywordtoggle,Explanation='Toggle keyword 12 in each of the selected photos.'},
  {Command='Keyword13Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','13'),Group=keywordtoggle,Explanation='Toggle keyword 13 in each of the selected photos.'},
  {Command='Keyword14Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','14'),Group=keywordtoggle,Explanation='Toggle keyword 14 in each of the selected photos.'},
  {Command='Keyword15Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','15'),Group=keywordtoggle,Explanation='Toggle keyword 15 in each of the selected photos.'},
  {Command='Keyword16Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','16'),Group=keywordtoggle,Explanation='Toggle keyword 16 in each of the selected photos.'},
  {Command='Keyword17Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','17'),Group=keywordtoggle,Explanation='Toggle keyword 17 in each of the selected photos.'},
  {Command='Keyword18Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','18'),Group=keywordtoggle,Explanation='Toggle keyword 18 in each of the selected photos.'},
  {Command='Keyword19Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','19'),Group=keywordtoggle,Explanation='Toggle keyword 19 in each of the selected photos.'},
  {Command='Keyword20Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','20'),Group=keywordtoggle,Explanation='Toggle keyword 20 in each of the selected photos.'},
  {Command='Keyword21Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','21'),Group=keywordtoggle,Explanation='Toggle keyword 21 in each of the selected photos.'},
  {Command='Keyword22Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','22'),Group=keywordtoggle,Explanation='Toggle keyword 22 in each of the selected photos.'},
  {Command='Keyword23Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','23'),Group=keywordtoggle,Explanation='Toggle keyword 23 in each of the selected photos.'},
  {Command='Keyword24Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','24'),Group=keywordtoggle,Explanation='Toggle keyword 24 in each of the selected photos.'},
  {Command='Keyword25Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','25'),Group=keywordtoggle,Explanation='Toggle keyword 25 in each of the selected photos.'},
  {Command='Keyword26Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','26'),Group=keywordtoggle,Explanation='Toggle keyword 26 in each of the selected photos.'},
  {Command='Keyword27Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','27'),Group=keywordtoggle,Explanation='Toggle keyword 27 in each of the selected photos.'},
  {Command='Keyword28Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','28'),Group=keywordtoggle,Explanation='Toggle keyword 28 in each of the selected photos.'},
  {Command='Keyword29Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','29'),Group=keywordtoggle,Explanation='Toggle keyword 29 in each of the selected photos.'},
  {Command='Keyword30Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','30'),Group=keywordtoggle,Explanation='Toggle keyword 30 in each of the selected photos.'},
  {Command='Keyword31Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','31'),Group=keywordtoggle,Explanation='Toggle keyword 31 in each of the selected photos.'},
  {Command='Keyword32Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','32'),Group=keywordtoggle,Explanation='Toggle keyword 32 in each of the selected photos.'},
  {Command='Keyword33Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','33'),Group=keywordtoggle,Explanation='Toggle keyword 33 in each of the selected photos.'},
  {Command='Keyword34Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','34'),Group=keywordtoggle,Explanation='Toggle keyword 34 in each of the selected photos.'},
  {Command='Keyword35Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','35'),Group=keywordtoggle,Explanation='Toggle keyword 35 in each of the selected photos.'},
  {Command='Keyword36Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','36'),Group=keywordtoggle,Explanation='Toggle keyword 36 in each of the selected photos.'},
  {Command='Keyword37Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','37'),Group=keywordtoggle,Explanation='Toggle keyword 37 in each of the selected photos.'},
  {Command='Keyword38Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','38'),Group=keywordtoggle,Explanation='Toggle keyword 38 in each of the selected photos.'},
  {Command='Keyword39Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','39'),Group=keywordtoggle,Explanation='Toggle keyword 39 in each of the selected photos.'},
  {Command='Keyword40Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','40'),Group=keywordtoggle,Explanation='Toggle keyword 40 in each of the selected photos.'},
  {Command='Keyword41Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','41'),Group=keywordtoggle,Explanation='Toggle keyword 41 in each of the selected photos.'},
  {Command='Keyword42Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','42'),Group=keywordtoggle,Explanation='Toggle keyword 42 in each of the selected photos.'},
  {Command='Keyword43Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','43'),Group=keywordtoggle,Explanation='Toggle keyword 43 in each of the selected photos.'},
  {Command='Keyword44Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','44'),Group=keywordtoggle,Explanation='Toggle keyword 44 in each of the selected photos.'},
  {Command='Keyword45Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','45'),Group=keywordtoggle,Explanation='Toggle keyword 45 in each of the selected photos.'},
  {Command='Keyword46Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','46'),Group=keywordtoggle,Explanation='Toggle keyword 46 in each of the selected photos.'},
  {Command='Keyword47Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','47'),Group=keywordtoggle,Explanation='Toggle keyword 47 in each of the selected photos.'},
  {Command='Keyword48Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','48'),Group=keywordtoggle,Explanation='Toggle keyword 48 in each of the selected photos.'},
  {Command='Keyword49Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','49'),Group=keywordtoggle,Explanation='Toggle keyword 49 in each of the selected photos.'},
  {Command='Keyword50Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','50'),Group=keywordtoggle,Explanation='Toggle keyword 50 in each of the selected photos.'},
  {Command='Keyword51Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','51'),Group=keywordtoggle,Explanation='Toggle keyword 51 in each of the selected photos.'},
  {Command='Keyword52Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','52'),Group=keywordtoggle,Explanation='Toggle keyword 52 in each of the selected photos.'},
  {Command='Keyword53Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','53'),Group=keywordtoggle,Explanation='Toggle keyword 53 in each of the selected photos.'},
  {Command='Keyword54Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','54'),Group=keywordtoggle,Explanation='Toggle keyword 54 in each of the selected photos.'},
  {Command='Keyword55Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','55'),Group=keywordtoggle,Explanation='Toggle keyword 55 in each of the selected photos.'},
  {Command='Keyword56Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','56'),Group=keywordtoggle,Explanation='Toggle keyword 56 in each of the selected photos.'},
  {Command='Keyword57Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','57'),Group=keywordtoggle,Explanation='Toggle keyword 57 in each of the selected photos.'},
  {Command='Keyword58Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','58'),Group=keywordtoggle,Explanation='Toggle keyword 58 in each of the selected photos.'},
  {Command='Keyword59Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','59'),Group=keywordtoggle,Explanation='Toggle keyword 59 in each of the selected photos.'},
  {Command='Keyword60Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','60'),Group=keywordtoggle,Explanation='Toggle keyword 60 in each of the selected photos.'},
  {Command='Keyword61Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','61'),Group=keywordtoggle,Explanation='Toggle keyword 61 in each of the selected photos.'},
  {Command='Keyword62Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','62'),Group=keywordtoggle,Explanation='Toggle keyword 62 in each of the selected photos.'},
  {Command='Keyword63Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','63'),Group=keywordtoggle,Explanation='Toggle keyword 63 in each of the selected photos.'},
  {Command='Keyword64Toggle',Type='button',Translation=LOC('$$$/MIDI2LR/Keyword/Toggle1=Toggle Keyword ^1','64'),Group=keywordtoggle,Explanation='Toggle keyword 64 in each of the selected photos.'},

  --
  --develop: masks
  --
  {Command='Mask',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',mask),Group=mask,Explanation='Select Mask mode in Develop Module. Repeated press toggles Loupe View.'},
  {Command='MaskEnable',Type='button',Translation=LOC('$$$/AgDevelop/Settings/ToggleMaskgroupBasedCorrections=Toggle Mask'),Group=mask,Explanation='Enable or disable mask-based corrections.'},
  {Command='MaskReset',Type='button',Translation=LOC('$$$/CRaw/Masking/DeleteAllMasks=Delete all masks'),Group=mask,Explanation=''},
  {Command='MaskPrevious',Type='button',Translation=LOC('$$$/MIDI2LR/Menu/PreviousMaskGroup=Previous mask group'),Group=mask,Explanation='Select previous mask group.'},
  {Command='MaskNext',Type='button',Translation=LOC('$$$/MIDI2LR/Menu/NextMaskGroup=Next mask group'),Group=mask,Explanation='Select next mask group.'},
  {Command='MaskInvert',Type='button',Translation=LOC('$$$/AgDevelop/Localized/Masking/MaskInvert=Invert mask group'),Group=mask,Explanation='Invert mask group.'},
  {Command='MaskInvertDup',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/Menu/DuplicateInvert=Duplicate and invert mask group'),Group=mask,Explanation='Duplicate mask group, then invert the duplicate.'},
  {Command='MaskPreviousTool',Type='button',Translation=LOC('$$$/MIDI2LR/Menu/PreviousMaskTool=Previous mask tool'),Group=mask,Explanation='Select next mask tool within a mask group.'},
  {Command='MaskNextTool',Type='button',Translation=LOC('$$$/MIDI2LR/Menu/NextMaskTool=Next mask tool'),Group=mask,Explanation='Select previous mask tool within a mask group.'},
  {Command='MaskDelete',Type='button',Translation=LOC('$$$/AgCameraRawController/Mask/Delete=Delete ^1',LOC('$$$/AgDevelop/Menu/View/MaskGroup=mask group')),Group=mask,Explanation='Delete the selected mask group.'},
  {Command='MaskDeleteTool',Type='button',Translation=LOC('$$$/AgCameraRawController/Mask/Delete=Delete ^1',LOC('$$$/MIDI2LR/Menu/Tool=tool')),Group=mask,Explanation='Delete the selected individual mask tool within the current group.'},
  {Command='MaskInvertTool',Type='button',Translation=LOC('$$$/AgDevelop/Localized/Masking/ToggleInvert=Toggle invert for selected tool'),Group=mask,Explanation=''},
  {Command='MaskHide',Type='button',Translation=LOC('$$$/MIDI2LR/ShortCuts/ShowHide1=Show/hide ^1',LOC('$$$/AgDevelop/Menu/View/MaskGroup=mask group')),Group=mask,Explanation='Show/hide the effect of selected mask group.'},
  {Command='MaskHideTool',Type='button',Translation=LOC('$$$/MIDI2LR/ShortCuts/ShowHide1=Show/hide ^1',LOC('$$$/MIDI2LR/Menu/Tool=tool')),Group=mask,Explanation='Show/hide the effect of selected individual mask tool within the current group.'},
  {Command='MaskNewBrush',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewMask=New ^1',LOC'$$$/AgDevelop/Localized/MaskGroup/Brush=brush'),Group=mask,Explanation=''},
  {Command='MaskNewGrad',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewMask=New ^1',LOC'$$$/AgDevelop/Localized/MaskGroup/Gradient=gradient'),Group=mask,Explanation=''},
  {Command='MaskNewRad',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewMask=New ^1',LOC'$$$/AgDevelop/Localized/MaskGroup/CircularGradient=radial gradient'),Group=mask,Explanation=''},
  {Command='MaskNewColor',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewMask=New ^1',LOC'$$$/AgDevelop/Localized/MaskGroup/ColorRange=color range'),Group=mask,Explanation=''},
  {Command='MaskNewLum',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewMask=New ^1',LOC'$$$/AgDevelop/Localized/MaskGroup/LuminanceRange=luminance range'),Group=mask,Explanation=''},
  {Command='MaskNewDepth',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewMask=New ^1',LOC'$$$/AgDevelop/Localized/MaskGroup/DepthRange=depth range'),Group=mask,Explanation=''},
  {Command='MaskNewSubject',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewMask=New ^1',LOC'$$$/AgDevelop/Localized/MaskGroup/SelectSubject/Short=subject'),Group=mask,Explanation=''},
  {Command='MaskNewSky',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewMask=New ^1',LOC'$$$/AgDevelop/Localized/MaskGroup/SelectSky/Short=sky'),Group=mask,Explanation=''},
  {Command='MaskAddBrush',Type='button',Translation=LOC('$$$/AgDevelop/Localized/Masking/AddToMask=Add ^1 to the mask',LOC'$$$/AgDevelop/Localized/MaskGroup/Brush=brush'),Group=mask,Explanation=''},
  {Command='MaskAddGrad',Type='button',Translation=LOC('$$$/AgDevelop/Localized/Masking/AddToMask=Add ^1 to the mask',LOC'$$$/AgDevelop/Localized/MaskGroup/Gradient=gradient'),Group=mask,Explanation=''},
  {Command='MaskAddRad',Type='button',Translation=LOC('$$$/AgDevelop/Localized/Masking/AddToMask=Add ^1 to the mask',LOC'$$$/AgDevelop/Localized/MaskGroup/CircularGradient=radial gradient'),Group=mask,Explanation=''},
  {Command='MaskAddColor',Type='button',Translation=LOC('$$$/AgDevelop/Localized/Masking/AddToMask=Add ^1 to the mask',LOC'$$$/AgDevelop/Localized/MaskGroup/ColorRange=color range'),Group=mask,Explanation=''},
  {Command='MaskAddLum',Type='button',Translation=LOC('$$$/AgDevelop/Localized/Masking/AddToMask=Add ^1 to the mask',LOC'$$$/AgDevelop/Localized/MaskGroup/LuminanceRange=luminance range'),Group=mask,Explanation=''},
  {Command='MaskAddDepth',Type='button',Translation=LOC('$$$/AgDevelop/Localized/Masking/AddToMask=Add ^1 to the mask',LOC'$$$/AgDevelop/Localized/MaskGroup/DepthRange=depth range'),Group=mask,Explanation=''},
  {Command='MaskAddSubject',Type='button',Translation=LOC('$$$/AgDevelop/Localized/Masking/AddToMask=Add ^1 to the mask',LOC'$$$/AgDevelop/Localized/MaskGroup/SelectSubject/Short=subject'),Group=mask,Explanation=''},
  {Command='MaskAddSky',Type='button',Translation=LOC('$$$/AgDevelop/Localized/Masking/AddToMask=Add ^1 to the mask',LOC'$$$/AgDevelop/Localized/MaskGroup/SelectSky/Short=sky'),Group=mask,Explanation=''},
  {Command='MaskSubBrush',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Subtract=Subtract from mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/Brush=brush'),Group=mask,Explanation=''},
  {Command='MaskSubGrad',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Subtract=Subtract from mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/Gradient=gradient'),Group=mask,Explanation=''},
  {Command='MaskSubRad',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Subtract=Subtract from mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/CircularGradient=radial gradient'),Group=mask,Explanation=''},
  {Command='MaskSubColor',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Subtract=Subtract from mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/ColorRange=color range'),Group=mask,Explanation=''},
  {Command='MaskSubLum',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Subtract=Subtract from mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/LuminanceRange=luminance range'),Group=mask,Explanation=''},
  {Command='MaskSubDepth',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Subtract=Subtract from mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/DepthRange=depth range'),Group=mask,Explanation=''},
  {Command='MaskSubSubject',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Subtract=Subtract from mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/SelectSubject/Short=subject'),Group=mask,Explanation=''},
  {Command='MaskSubSky',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Subtract=Subtract from mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/SelectSky/Short=sky'),Group=mask,Explanation=''},
  {Command='MaskIntBrush',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Intersect=Intersect with mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/Brush=brush'),Group=mask,Explanation=''},
  {Command='MaskIntGrad',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Intersect=Intersect with mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/Gradient=gradient'),Group=mask,Explanation=''},
  {Command='MaskIntRad',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Intersect=Intersect with mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/CircularGradient=radial gradient'),Group=mask,Explanation=''},
  {Command='MaskIntColor',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Intersect=Intersect with mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/ColorRange=color range'),Group=mask,Explanation=''},
  {Command='MaskIntLum',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Intersect=Intersect with mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/LuminanceRange=luminance range'),Group=mask,Explanation=''},
  {Command='MaskIntDepth',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Intersect=Intersect with mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/DepthRange=depth range'),Group=mask,Explanation=''},
  {Command='MaskIntSubject',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Intersect=Intersect with mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/SelectSubject/Short=subject'),Group=mask,Explanation=''},
  {Command='MaskIntSky',Type='button',Translation=LOC('$$$/AgDevelop/Localized/MaskGroup/NewComponent/Intersect=Intersect with mask...')..' '..LOC('$$$/AgDevelop/Localized/MaskGroup/SelectSky/Short=sky'),Group=mask,Explanation=''},

  --
  --develop: localized adjustments
  --
  {Command='RedEye',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Redeye=Red-Eye Correction')),Group=localizedAdjustments,Explanation='Select Red Eye mode in Develop Module. Repeated press toggles Loupe View.'},
  {Command='SpotRemoval',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/SpotRemoval=Spot Removal')),Group=localizedAdjustments,Explanation='Select Spot Removal mode in Develop Module. Repeated press toggles Loupe View.'},
  {Command='ToggleOverlay',type='button',Translation=LOC('$$$/AgDevelop/Localized/Masking/ToggleOverlay=Toggle Mask Overlay'),Group=localizedAdjustments,Explanation='Toggles the localized adjustments mask overlay. Use only when any of the local adjustments filter is active.'},
  {Command='CycleMaskOverlayColor',Type='button',Translation=LOC('$$$/AgDevelop/Menu/View/AdjustmentBrushOverlay/CycleOverlay=Overlay Color'):gsub('%(%&%a%)',''):gsub('%&',''),Group=localizedAdjustments,Explanation='Sends the keystroke <kbd>\226\135\167 Shift</kbd>+<kbd>o</kbd> to Lightroom. Change Mask Overlay Color.'},
  {Command='EnableRedEye',Type='button',Translation=LOC('$$$/AgDevelop/Settings/ToggleRedEye=Red-Eye enable/disable'),Group=localizedAdjustments,Explanation='Enable or disable red eye correction.'},
  {Command='EnableRetouch',Type='button',Translation=LOC('$$$/AgDevelop/Settings/ToggleSpotRemoval=Spot removal enable/disable'),Group=localizedAdjustments,Explanation='Enable or disable spot removal.'},
  {Command='ChangeBrushSize',Type='repeat',Translation=brush..' — '..size,Group=localizedAdjustments,Explanation='Sends keystroke to Lightroom. The keystroke varies according to the which Language Lightroom is set to. Change adjustment brush size. This works with spot, gradient, radial filter and localized adjustment tools. **Caution**: With gradient and radial filter, make sure *brush* is selected when using this command. Turning knob clockwise sends Increase Size signals to Lightroom, counterclockwise Decrease Size.'..repeatexp,Repeats={'BrushSizeLarger','BrushSizeSmaller'}},
  {Command='BrushSizeSmaller',Type='button',Translation=brush..' — '..size..' — '..smaller,Group=localizedAdjustments,Explanation='Sends keystroke to Lightroom. The keystroke varies according to the which Language Lightroom is set to. Reduce adjustment brush size.  This works with spot, gradient, radial filter and localized adjustment tools. **Caution**: With gradient and radial filter, make sure *brush* is selected when using this command.'},
  {Command='BrushSizeLarger',Type='button',Translation=brush..' — '..size..' — '..larger,Group=localizedAdjustments,Explanation='Sends keystroke to Lightroom. The keystroke varies according to the which Language Lightroom is set to. Increase adjustment brush size.  This works with spot, gradient, radial filter and localized adjustment tools. **Caution**: With gradient and radial filter, make sure *brush* is selected when using this command.'},
  {Command='ChangeFeatherSize',Type='repeat',Translation=brush..' — '..feather,Group=localizedAdjustments,Explanation='Sends keystroke to Lightroom. The keystroke varies according to the which Language Lightroom is set to. Change adjustment brush feather size.  This works with spot, gradient, radial filter and localized adjustment tools. **Caution**: With gradient and radial filter, make sure *brush* is selected when using this command. Turning knob clockwise sends Increase Size signals to Lightroom, counterclockwise Decrease Size.'..repeatexp,Repeats={'BrushFeatherLarger','BrushFeatherSmaller'}},
  {Command='BrushFeatherSmaller',Type='button',Translation=brush..' — '..feather..' — '..smaller,Group=localizedAdjustments,Explanation='Sends keystroke to Lightroom. The keystroke varies according to the which Language Lightroom is set to. Reduce adjustment brush feather. This works with spot, gradient, radial filter and localized adjustment tools. **Caution**: With gradient and radial filter, make sure *brush* is selected when using this command.'},
  {Command='BrushFeatherLarger',Type='button',Translation=brush..' — '..feather..' — '..larger,Group=localizedAdjustments,Explanation='Sends keystroke to Lightroom. The keystroke varies according to the which Language Lightroom is set to. Increase adjustment brush feather. This works with spot, gradient, radial filter and localized adjustment tools. **Caution**: With gradient and radial filter, make sure *brush* is selected when using this command.'},
  {Command='local_Amount',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Amount=amount'),Group=localizedAdjustments,Explanation='Adjust amount of application of local presets',AltParameter='Direct'},
  {Command='local_Temperature',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Temperature=Temp.')..' (PV 3+)',PV3=locadj..' '..LOC('$$$/AgDevelop/Localized/Temperature=Temp.'),Group=localizedAdjustments,Explanation='Adjust Temperature for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_Tint',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Tint=Tint')..' (PV 3+)',PV3=locadj..' '..LOC('$$$/AgDevelop/Localized/Tint=Tint'),Group=localizedAdjustments,Explanation='Adjust Tint for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_Exposure',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Exposure=Exposure')..' ('..P2and3plus..')',PV2=locadj..' '..LOC('$$$/AgDevelop/Localized/Exposure=Exposure'),Group=localizedAdjustments,Explanation='Adjust Exposure for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_Contrast',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Contrast=Contrast')..' ('..P2and3plus..')',PV2=locadj..' '..LOC('$$$/AgDevelop/Localized/Contrast=Contrast'),Group=localizedAdjustments,Explanation='Adjust Contrast for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_Highlights',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Highlights=Highlights')..' (PV 3+)',PV3=locadj..' '..LOC('$$$/AgDevelop/Localized/Highlights=Highlights'),Group=localizedAdjustments,Explanation='Adjust Highlights for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_Shadows',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Shadows=Shadows')..' (PV 3+)',PV3=locadj..' '..LOC('$$$/AgDevelop/Localized/Shadows=Shadows'),Group=localizedAdjustments,Explanation='Adjust Shadows for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_Whites2012',Type='parameter',Experimental=true,Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Whites=Whites')..' (PV 3+)',PV3=locadj..' '..LOC('$$$/AgDevelop/Localized/Whites=Whites'),Group=localizedAdjustments,Explanation='Adjust Whites for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_Blacks2012',Type='parameter',Experimental=true,Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Blacks=Blacks')..' (PV 3+)',PV3=locadj..' '..LOC('$$$/AgDevelop/Localized/Blacks=Blacks'),Group=localizedAdjustments,Explanation='Adjust Blacks for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_Texture',Type='parameter',Translation=locadj..' '..LOC('$$$/AgCameraRawUI/Texture=Texture'),Group=localizedAdjustments,Explanation='Adjust Texture for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_Clarity',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Clarity=Clarity')..' ('..P2and3plus..')',PV2=locadj..' '..LOC('$$$/AgDevelop/Localized/Clarity=Clarity'),Group=localizedAdjustments,Explanation='Adjust Clarity for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_Dehaze',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Dehaze=Dehaze')..' (PV 3+)',PV3=locadj..' '..LOC('$$$/AgDevelop/Localized/Dehaze=Dehaze'),Group=localizedAdjustments,Explanation='Adjust Dehaze for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_Hue',Type='parameter',Translation=locadj..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Hue=Hue')..' (PV 5+)',PV5=locadj..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Hue=Hue'),Group=localizedAdjustments,Explanation='Adjust Hue for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_Saturation',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Saturation=Saturation')..' ('..P2and3plus..')',PV2=locadj..' '..LOC('$$$/AgDevelop/Localized/Saturation=Saturation'),Group=localizedAdjustments,Explanation='Adjust Saturation for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_Sharpness',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Sharpness=Sharpness')..' ('..P2and3plus..')',PV2=locadj..' '..LOC('$$$/AgDevelop/Localized/Sharpness=Sharpness'),Group=localizedAdjustments,Explanation='Adjust Sharpness for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_LuminanceNoise',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/LuminanceNoiseReduction=Luminance Noise Reduction')..' (PV 3+)',PV3=locadj..' '..LOC('$$$/AgDevelop/Localized/LuminanceNoiseReduction=Luminance Noise Reduction'),Group=localizedAdjustments,Explanation='Adjust Luminance Noise for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_Moire',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/MoireReduction=Moire')..' (PV 3+)',PV3=locadj..' '..LOC('$$$/AgDevelop/Localized/MoireReduction=Moire'),Group=localizedAdjustments,Explanation='Adjust Moire for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_Defringe',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Defringe=Defringe')..' (PV 3+)',PV3=locadj..' '..LOC('$$$/AgDevelop/Localized/Defringe=Defringe'),Group=localizedAdjustments,Explanation='Adjust Defringe for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  {Command='local_ToningLuminance',Type='parameter',Translation=locadj..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Luminance=Luminance')..' (PV 2)',PV2=locadj..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Luminance=Luminance'),PV3=locadj..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Luminance=Luminance')..' (PV 2)',Group=localizedAdjustments,Explanation='Adjust Toning Luminance for the currently active tool: Brush, Radial Filter, or Graduated Filter.',AltParameter='Direct'},
  --local adjustment resets
  {Command='ResetRedeye',Type='button',Translation=LOC('$$$/AgLibrary/Ops/ResetRedeye=Reset Red-Eye'),Group=localadjresets,Explanation='Delete red eye correction.'},
  {Command='ResetSpotRem',Type='button',Translation=LOC('$$$/AgLibrary/Ops/ResetSpotRemoval=Reset Spot Removal'),Group=localadjresets,Explanation='Delete spot removal.'},
  {Command='Resetlocal_Temperature',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Temperature=Temp.')..' (PV 3+)'),PV3=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Temperature=Temp.')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Tint',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Tint=Tint')..' (PV 3+)'),PV3=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Tint=Tint')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Exposure',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Exposure=Exposure')..' ('..P2and3plus..')'),PV2=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Exposure=Exposure')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Contrast',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Contrast=Contrast')..' ('..P2and3plus..')'),PV2=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Contrast=Contrast')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Highlights',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Highlights=Highlights')..' (PV 3+)'),PV3=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Highlights=Highlights')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Shadows',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Shadows=Shadows')..' (PV 3+)'),PV3=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Shadows=Shadows')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Whites2012',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Whites=Whites')..' (PV 3+)'),PV3=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Whites=Whites')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Blacks2012',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Blacks=Blacks')..' (PV 3+)'),PV3=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Blacks=Blacks')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Texture',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgCameraRawUI/Texture=Texture')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Clarity',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Clarity=Clarity')..' ('..P2and3plus..')'),PV2=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Clarity=Clarity')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Dehaze',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Dehaze=Dehaze')..' (PV 3+)'),PV3=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Dehaze=Dehaze')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Hue',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Hue=Hue')..' (PV 5+)'),PV5=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Hue=Hue')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Saturation',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Saturation=Saturation')..' ('..P2and3plus..')'),PV2=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Saturation=Saturation')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Sharpness',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Sharpness=Sharpness')..' ('..P2and3plus..')'),PV2=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Sharpness=Sharpness')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_LuminanceNoise',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/LuminanceNoiseReduction=Luminance Noise Reduction')..' (PV 3+)'),PV3=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/LuminanceNoiseReduction=Luminence Noise Reduction')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Moire',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/MoireReduction=Moire')..' (PV 3+)'),PV3=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/MoireReduction=Moire')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Defringe',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Defringe=Defringe')..' (PV 3+)'),PV3=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgDevelop/Localized/Defringe=Defringe')),Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_ToningLuminance',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Luminance=Luminance')..' (PV 2)'),PV2=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Luminance=Luminanz')),PV3=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',locadj..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Luminance=Luminanz')..' (PV 2)'),Group=localadjresets,Explanation='Reset to default.'},
  --
  --local adjustment presets
  --
  {Command='LocalPreset1',Type='button',Translation=locadjpre..' 1',Group=locadjpre,Explanation='Use preset 1 for localized adjustments.'},
  {Command='LocalPreset2',Type='button',Translation=locadjpre..' 2',Group=locadjpre,Explanation='Use preset 2 for localized adjustments.'},
  {Command='LocalPreset3',Type='button',Translation=locadjpre..' 3',Group=locadjpre,Explanation='Use preset 3 for localized adjustments.'},
  {Command='LocalPreset4',Type='button',Translation=locadjpre..' 4',Group=locadjpre,Explanation='Use preset 4 for localized adjustments.'},
  {Command='LocalPreset5',Type='button',Translation=locadjpre..' 5',Group=locadjpre,Explanation='Use preset 5 for localized adjustments.'},
  {Command='LocalPreset6',Type='button',Translation=locadjpre..' 6',Group=locadjpre,Explanation='Use preset 6 for localized adjustments.'},
  {Command='LocalPreset7',Type='button',Translation=locadjpre..' 7',Group=locadjpre,Explanation='Use preset 7 for localized adjustments.'},
  {Command='LocalPreset8',Type='button',Translation=locadjpre..' 8',Group=locadjpre,Explanation='Use preset 8 for localized adjustments.'},
  {Command='LocalPreset9',Type='button',Translation=locadjpre..' 9',Group=locadjpre,Explanation='Use preset 9 for localized adjustments.'},
  {Command='LocalPreset10',Type='button',Translation=locadjpre..' 10',Group=locadjpre,Explanation='Use preset 10 for localized adjustments.'},
  {Command='LocalPreset11',Type='button',Translation=locadjpre..' 11',Group=locadjpre,Explanation='Use preset 11 for localized adjustments.'},
  {Command='LocalPreset12',Type='button',Translation=locadjpre..' 12',Group=locadjpre,Explanation='Use preset 12 for localized adjustments.'},
  {Command='LocalPreset13',Type='button',Translation=locadjpre..' 13',Group=locadjpre,Explanation='Use preset 13 for localized adjustments.'},
  {Command='LocalPreset14',Type='button',Translation=locadjpre..' 14',Group=locadjpre,Explanation='Use preset 14 for localized adjustments.'},
  {Command='LocalPreset15',Type='button',Translation=locadjpre..' 15',Group=locadjpre,Explanation='Use preset 15 for localized adjustments.'},
  {Command='LocalPreset16',Type='button',Translation=locadjpre..' 16',Group=locadjpre,Explanation='Use preset 16 for localized adjustments.'},
  {Command='LocalPreset17',Type='button',Translation=locadjpre..' 17',Group=locadjpre,Explanation='Use preset 17 for localized adjustments.'},
  {Command='LocalPreset18',Type='button',Translation=locadjpre..' 18',Group=locadjpre,Explanation='Use preset 18 for localized adjustments.'},
  {Command='LocalPreset19',Type='button',Translation=locadjpre..' 19',Group=locadjpre,Explanation='Use preset 19 for localized adjustments.'},
  {Command='LocalPreset20',Type='button',Translation=locadjpre..' 20',Group=locadjpre,Explanation='Use preset 20 for localized adjustments.'},
  {Command='LocalPreset21',Type='button',Translation=locadjpre..' 21',Group=locadjpre,Explanation='Use preset 21 for localized adjustments.'},
  {Command='LocalPreset22',Type='button',Translation=locadjpre..' 22',Group=locadjpre,Explanation='Use preset 22 for localized adjustments.'},
  {Command='LocalPreset23',Type='button',Translation=locadjpre..' 23',Group=locadjpre,Explanation='Use preset 23 for localized adjustments.'},
  {Command='LocalPreset24',Type='button',Translation=locadjpre..' 24',Group=locadjpre,Explanation='Use preset 24 for localized adjustments.'},
  {Command='LocalPreset25',Type='button',Translation=locadjpre..' 25',Group=locadjpre,Explanation='Use preset 25 for localized adjustments.'},
  {Command='LocalPreset26',Type='button',Translation=locadjpre..' 26',Group=locadjpre,Explanation='Use preset 26 for localized adjustments.'},
  {Command='LocalPreset27',Type='button',Translation=locadjpre..' 27',Group=locadjpre,Explanation='Use preset 27 for localized adjustments.'},
  {Command='LocalPreset28',Type='button',Translation=locadjpre..' 28',Group=locadjpre,Explanation='Use preset 28 for localized adjustments.'},
  {Command='LocalPreset29',Type='button',Translation=locadjpre..' 29',Group=locadjpre,Explanation='Use preset 29 for localized adjustments.'},
  {Command='LocalPreset30',Type='button',Translation=locadjpre..' 30',Group=locadjpre,Explanation='Use preset 30 for localized adjustments.'},
  {Command='LocalPreset31',Type='button',Translation=locadjpre..' 31',Group=locadjpre,Explanation='Use preset 31 for localized adjustments.'},
  {Command='LocalPreset32',Type='button',Translation=locadjpre..' 32',Group=locadjpre,Explanation='Use preset 32 for localized adjustments.'},
  {Command='LocalPreset33',Type='button',Translation=locadjpre..' 33',Group=locadjpre,Explanation='Use preset 33 for localized adjustments.'},
  {Command='LocalPreset34',Type='button',Translation=locadjpre..' 34',Group=locadjpre,Explanation='Use preset 34 for localized adjustments.'},
  {Command='LocalPreset35',Type='button',Translation=locadjpre..' 35',Group=locadjpre,Explanation='Use preset 35 for localized adjustments.'},
  {Command='LocalPreset36',Type='button',Translation=locadjpre..' 36',Group=locadjpre,Explanation='Use preset 36 for localized adjustments.'},
  {Command='LocalPreset37',Type='button',Translation=locadjpre..' 37',Group=locadjpre,Explanation='Use preset 37 for localized adjustments.'},
  {Command='LocalPreset38',Type='button',Translation=locadjpre..' 38',Group=locadjpre,Explanation='Use preset 38 for localized adjustments.'},
  {Command='LocalPreset39',Type='button',Translation=locadjpre..' 39',Group=locadjpre,Explanation='Use preset 39 for localized adjustments.'},
  {Command='LocalPreset40',Type='button',Translation=locadjpre..' 40',Group=locadjpre,Explanation='Use preset 40 for localized adjustments.'},
  {Command='LocalPreset41',Type='button',Translation=locadjpre..' 41',Group=locadjpre,Explanation='Use preset 41 for localized adjustments.'},
  {Command='LocalPreset42',Type='button',Translation=locadjpre..' 42',Group=locadjpre,Explanation='Use preset 42 for localized adjustments.'},
  {Command='LocalPreset43',Type='button',Translation=locadjpre..' 43',Group=locadjpre,Explanation='Use preset 43 for localized adjustments.'},
  {Command='LocalPreset44',Type='button',Translation=locadjpre..' 44',Group=locadjpre,Explanation='Use preset 44 for localized adjustments.'},
  {Command='LocalPreset45',Type='button',Translation=locadjpre..' 45',Group=locadjpre,Explanation='Use preset 45 for localized adjustments.'},
  {Command='LocalPreset46',Type='button',Translation=locadjpre..' 46',Group=locadjpre,Explanation='Use preset 46 for localized adjustments.'},
  {Command='LocalPreset47',Type='button',Translation=locadjpre..' 47',Group=locadjpre,Explanation='Use preset 47 for localized adjustments.'},
  {Command='LocalPreset48',Type='button',Translation=locadjpre..' 48',Group=locadjpre,Explanation='Use preset 48 for localized adjustments.'},
  {Command='LocalPreset49',Type='button',Translation=locadjpre..' 49',Group=locadjpre,Explanation='Use preset 49 for localized adjustments.'},
  {Command='LocalPreset50',Type='button',Translation=locadjpre..' 50',Group=locadjpre,Explanation='Use preset 50 for localized adjustments.'},
  --
  -- Misc
  --

  {Command='straightenAngle',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/StraightenAngle=Straighten Angle'),Group=crop,Explanation='Rotate crop angle. Moves angle in crop tool panel from -45 to 45.'},
  {Command='CropBottom',Type='parameter',Experimental=true,Translation=crop..' - '..LOC('$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Bottom=Bottom'),Group=crop,Explanation='Adjust bottom of crop rectangle.'},
  {Command='CropLeft',Type='parameter',Experimental=true,Translation=crop..' - '..LOC('$$$/AgWatermarking/Alignment/Left=Left'),Group=crop,Explanation='Adjust left side of crop rectangle.'},
  {Command='CropRight',Type='parameter',Experimental=true,Translation=crop..' - '..LOC('$$$/AgWatermarking/Alignment/Right=Right'),Group=crop,Explanation='Adjust right side of crop rectangle.'},
  {Command='CropTop',Type='parameter',Experimental=true,Translation=crop..' - '..LOC('$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Top=Top'),Group=crop,Explanation='Adjust top of crop rectangle.'},
  {Command='CropTopLeft', Type = 'variable',Experimental=true,Translation=crop..' - '..LOC('$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Top=Top')..' - '..LOC('$$$/AgWatermarking/Alignment/Left=Left'),Group=crop,Explanation='Adjust crop from top left corner, preserving the current crop ratio.'},
  {Command='CropTopRight', Type = 'variable',Experimental=true,Translation=crop..' - '..LOC('$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Top=Top')..' - '..LOC('$$$/AgWatermarking/Alignment/Right=Right'),Group=crop,Explanation='Adjust crop from top right corner, preserving the current crop ratio.'},
  {Command='CropBottomLeft', Type = 'variable',Experimental=true,Translation=crop..' - '..LOC('$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Bottom=Bottom')..' - '..LOC('$$$/AgWatermarking/Alignment/Left=Left'),Group=crop,Explanation='Adjust crop from bottom left corner, preserving the current crop ratio.'},
  {Command='CropBottomRight', Type = 'variable',Experimental=true,Translation=crop..' - '..LOC('$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Bottom=Bottom')..' - '..LOC('$$$/AgWatermarking/Alignment/Right=Right'),Group=crop,Explanation='Adjust crop from bottom right corner, preserving the current crop ratio.'},
  {Command='CropAll', Type = 'variable',Experimental=true,Translation=crop..' - '..LOC('$$$/AgDevelop/LookBrowser/ProfileFilter/All=All'),Group=crop,Explanation='Adjust crop at all corners proportionately, preserving the current crop ratio.'},
  {Command='CropMoveVertical', Type = 'variable',Experimental=true,Translation=crop..' - '..LOC('$$$/AgHTemplateBrowserView2/Dialog/Title/Move=move ^1',LOC('$$$/AgLibrary/Ops/flip/direction/v=vertically')),Group=crop,Explanation='Move crop rectangle up or down, preserving the current crop ratio.'},
  {Command='CropMoveHorizontal', Type = 'variable',Experimental=true,Translation=crop..' - '..LOC('$$$/AgHTemplateBrowserView2/Dialog/Title/Move=move ^1',LOC('$$$/AgLibrary/Ops/flip/direction/h=horizontally')),Group=crop,Explanation='Move crop rectangle right or left, preserving the current crop ratio.'},
  {Command='ResetCrop',Type='button',Translation=LOC('$$$/AgLibrary/Ops/ResetCrop=Reset Crop'),Group=crop,Explanation='Reset the crop angle and frame for the current photo.'},
  {Command='ResetstraightenAngle',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Reset=Reset ^1',LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/StraightenAngle=Straighten Angle')),Group=crop,Explanation='Reset crop angle.'},
  {Command='CropOverlay',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',crop),Group=crop,Explanation='Select Crop Overlay mode in Develop Module. Repeated press toggles Loupe View.'},
  {Command='Loupe',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',LOC('$$$/AgPhotoBin/ViewMode/Develop/Loupe=Loupe')),Group=gotoToolModulePanel,Explanation='Select Loupe View mode in Develop Module. Repeated press toggles in and out of Loupe View.'},
  {Command='SwToMmap',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',LOC('$$$/AgLocation/ModuleTitle=Map')),Group=gotoToolModulePanel,Explanation='Switch to Map module.'},
  {Command='SwToMbook',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',LOC('$$$/Ag/Layout/Book/ModuleTitle=Book')),Group=gotoToolModulePanel,Explanation='Switch to Book module.'},
  {Command='SwToMslideshow',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',LOC('$$$/AgApplication/Menu/Window/SecondMonitor/Slideshow=Slideshow')),Group=gotoToolModulePanel,Explanation='Switch to Slideshow module.'},
  {Command='SwToMprint',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',LOC('$$$/AgPrint/Menu/File/Print=Print')),Group=gotoToolModulePanel,Explanation='Switch to Print module.'},
  {Command='SwToMweb',Type='button',Translation=LOC('$$$/MIDI2LR/Database/Show1=Show ^1',LOC('$$$/WPG/Help/Shortcuts/WebHeader=Web')),Group=gotoToolModulePanel,Explanation='Switch to Web module.'},
  {Command='ShoScndVwloupe',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/Loupe=Loupe'),Group=secondaryDisplay,Explanation='Shows Loupe view on the secondary screen or hides the secondary screen if Loupe view was previously being shown.'},
  {Command='ShoScndVwlive_loupe',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/LiveLoupe=Live Loupe'),Group=secondaryDisplay,Explanation='Shows Live Loupe view on the secondary screen or hides the secondary screen if Live Loupe view was previously being shown.'},
  {Command='ShoScndVwlocked_loupe',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/LockedLoupe=Locked Loupe'),Group=secondaryDisplay,Explanation='Shows Locked Loupe view on the secondary screen or hides the secondary screen if Locked Loupe view was previously being shown.'},
  {Command='ShoScndVwgrid',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/Grid=Grid'),Group=secondaryDisplay,Explanation='Shows Grid view on the secondary screen or hides the secondary screen if Grid view was previously being shown.'},
  {Command='ShoScndVwcompare',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/Compare=Compare'),Group=secondaryDisplay,Explanation='Shows Compare view on the secondary screen or hides the secondary screen if Compare view was previously being shown.'},
  {Command='ShoScndVwsurvey',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/Survey=Survey'),Group=secondaryDisplay,Explanation='Shows Survey view on the secondary screen or hides the secondary screen if Survey view was previously being shown.'},
  {Command='ShoScndVwslideshow',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/Slideshow=Slideshow'),Group=secondaryDisplay,Explanation='Shows Slideshow view on the secondary screen or hides the secondary screen if Slideshow view was previously being shown.'},
  {Command='ToggleScreenTwo',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/Show=Show'),Group=secondaryDisplay,Explanation='Toggles the secondary window on/off.'},
  {Command='profile1',Type='button',Translation=profile..' 1',Group=profiles,Explanation='Change to MIDI mapping profile 1.'},
  {Command='profile2',Type='button',Translation=profile..' 2',Group=profiles,Explanation='Change to MIDI mapping profile 2.'},
  {Command='profile3',Type='button',Translation=profile..' 3',Group=profiles,Explanation='Change to MIDI mapping profile 3.'},
  {Command='profile4',Type='button',Translation=profile..' 4',Group=profiles,Explanation='Change to MIDI mapping profile 4.'},
  {Command='profile5',Type='button',Translation=profile..' 5',Group=profiles,Explanation='Change to MIDI mapping profile 5.'},
  {Command='profile6',Type='button',Translation=profile..' 6',Group=profiles,Explanation='Change to MIDI mapping profile 6.'},
  {Command='profile7',Type='button',Translation=profile..' 7',Group=profiles,Explanation='Change to MIDI mapping profile 7.'},
  {Command='profile8',Type='button',Translation=profile..' 8',Group=profiles,Explanation='Change to MIDI mapping profile 8.'},
  {Command='profile9',Type='button',Translation=profile..' 9',Group=profiles,Explanation='Change to MIDI mapping profile 9.'},
  {Command='profile10',Type='button',Translation=profile..' 10',Group=profiles,Explanation='Change to MIDI mapping profile 10.'},
  {Command='profile11',Type='button',Translation=profile..' 11',Group=profiles,Explanation='Change to MIDI mapping profile 11.'},
  {Command='profile12',Type='button',Translation=profile..' 12',Group=profiles,Explanation='Change to MIDI mapping profile 12.'},
  {Command='profile13',Type='button',Translation=profile..' 13',Group=profiles,Explanation='Change to MIDI mapping profile 13.'},
  {Command='profile14',Type='button',Translation=profile..' 14',Group=profiles,Explanation='Change to MIDI mapping profile 14.'},
  {Command='profile15',Type='button',Translation=profile..' 15',Group=profiles,Explanation='Change to MIDI mapping profile 15.'},
  {Command='profile16',Type='button',Translation=profile..' 16',Group=profiles,Explanation='Change to MIDI mapping profile 16.'},
  {Command='profile17',Type='button',Translation=profile..' 17',Group=profiles,Explanation='Change to MIDI mapping profile 17.'},
  {Command='profile18',Type='button',Translation=profile..' 18',Group=profiles,Explanation='Change to MIDI mapping profile 18.'},
  {Command='profile19',Type='button',Translation=profile..' 19',Group=profiles,Explanation='Change to MIDI mapping profile 19.'},
  {Command='profile20',Type='button',Translation=profile..' 20',Group=profiles,Explanation='Change to MIDI mapping profile 20.'},
  {Command='profile21',Type='button',Translation=profile..' 21',Group=profiles,Explanation='Change to MIDI mapping profile 21.'},
  {Command='profile22',Type='button',Translation=profile..' 22',Group=profiles,Explanation='Change to MIDI mapping profile 22.'},
  {Command='profile23',Type='button',Translation=profile..' 23',Group=profiles,Explanation='Change to MIDI mapping profile 23.'},
  {Command='profile24',Type='button',Translation=profile..' 24',Group=profiles,Explanation='Change to MIDI mapping profile 24.'},
  {Command='profile25',Type='button',Translation=profile..' 25',Group=profiles,Explanation='Change to MIDI mapping profile 25.'},
  {Command='profile26',Type='button',Translation=profile..' 26',Group=profiles,Explanation='Change to MIDI mapping profile 26.'},
  {Command='PrevPro',Type='button',Translation=LOC('$$$/MIDI2LR/Profiles/Previous=Previous Profile'),Group=profiles,Explanation='Change to previous (by alpha order) MIDI mapping profile.'},
  {Command='NextPro',Type='button',Translation=LOC('$$$/MIDI2LR/Profiles/Next=Next Profile'),Group=profiles,Explanation='Change to next (by alpha order) MIDI mapping profile.'},
}

local CmdTrans={}
local CmdPanel={}
local Parameters={}
local ValidActions={}
--update LatestPVSupported (at top of this file) when adding PVs below
for _,v in ipairs(DataBase) do
  CmdTrans[v.Command] = {}
  CmdTrans[v.Command][1] = v.PV1 or v.Translation
  CmdTrans[v.Command][2] = v.PV2 or CmdTrans[v.Command][1]
  CmdTrans[v.Command][3] = v.PV3 or CmdTrans[v.Command][2]
  CmdTrans[v.Command][4] = v.PV4 or CmdTrans[v.Command][3]
  CmdTrans[v.Command][5] = v.PV5 or CmdTrans[v.Command][4]
  if v.Panel then
    CmdPanel[v.Command] = v.Panel
  end
  if v.Type == 'parameter' then
    Parameters[v.Command] = v.AltParameter or v.Command
  elseif v.Type == 'button' then
    ValidActions[v.Command] = true
  end
end

local LrPathUtils = import 'LrPathUtils'
local Ut = require 'Utilities'

local AppTrans = LrPathUtils.child(Ut.appdatapath() , 'MenuTrans.xml')
local function WriteAppTrans(language)
  local file = assert(io.open(AppTrans,'w'),LOC("$$$/AgImageIO/Errors/WriteFile=The file could not be written.")..' '..'MenuTrans.txt')
  --new version for xml file
  local CmdStructure={}
  local GroupOrder={}
  local repeats={}
  local wraps={}
  for _,v in ipairs(DataBase) do
    if CmdStructure[v.Group] then
      table.insert(CmdStructure[v.Group],{v.Command,v.Translation})
    else
      GroupOrder[#GroupOrder+1]=v.Group
      CmdStructure[v.Group]={}
      table.insert(CmdStructure[v.Group],{v.Command,v.Translation})
    end
    if v.Wraps then
      wraps[#wraps+1]=v.Command
    end
    if v.Repeats and (type(v.Repeats) == 'table') then
      repeats[v.Command] = v.Repeats
    end
  end
  file:write([=[
<?xml version="1.0" encoding="utf-8"?>
<cereal>
  <value0>
    <cereal_class_version>2</cereal_class_version>
    <language>]=],language,[=[</language>
    <all_commands size="dynamic">
  ]=])
  for i,v in ipairs(GroupOrder) do
    file:write('      <value'.. i-1 ..'>\n        <first>'..v..'</first>\n'..
      '        <second size="dynamic">\n')
    for j,u in ipairs(CmdStructure[v]) do
      file:write('            <value'.. j-1 ..'>\n'..
        '              <first>'.. u[1] ..'</first>\n'..
        '              <second>'.. u[2]:gsub("&","&amp;") ..'</second>\n'..
        '            </value'.. j-1 ..'>\n')
    end
    file:write('        </second>\n      </value'.. i-1 ..'>\n')
  end
  file:write([=[
    </all_commands>
    <repeats size="dynamic">
]=])
  local i = 0
  for k,v in pairs(repeats) do
    file:write('      <value'.. i ..'>\n        <key>'..k..
      '</key>\n        <value>\n          <first xml:space="preserve">'..
      v[1]..' 1\n</first>\n          <second xml:space="preserve">'..v[2]..
      ' 1\n</second>\n        </value>\n      </value'..i..'>\n')
    i = i + 1
  end
  file:write([=[
    </repeats>
    <wraps size="dynamic">
]=])
  for j,v in ipairs(wraps) do
    file:write('      <value'.. j-1 ..'>'..v..'</value'.. j-1 ..'>\n')
  end
  file:write([=[
    </wraps>
  </value0>
</cereal>
  ]=])
  file:close()
end

return {
  AppTrans = AppTrans,
  CmdPanel = CmdPanel,
  CmdTrans = CmdTrans,
  DataBase = DataBase,
  LatestPVSupported = LatestPVSupported,
  Parameters = Parameters,
  ValidActions = ValidActions,
  WriteAppTrans = WriteAppTrans,
}

