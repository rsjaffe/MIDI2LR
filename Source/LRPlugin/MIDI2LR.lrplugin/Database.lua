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

use st:gsub('%&', '') to remove ampersands

Command ( string for command )
Type
  button just on/off
  parameter LR parameter
  variable needs slider value
  repeat sends repeated LR commands when twisting knob (see LR_IPC_Out.cpp)

Fields (experimental and panel are optional)
Command Type Experimental Translation Group (in app) Explanation Panel (in LR Devel)
  -----------------------------------------------------------------------------]]

--local function LOC(str) return str end--for debugging

--Common terms used in database translated once here
--local coarse = LOC('$$$/AgPrint/CalibrationDialog/Coarse=Coarse')
--local fine = LOC('$$$/AgPrint/CalibrationDialog/Fine=Fine')
--local viewModes = LOC('$$$/AgDevelop/Toolbar/ViewModesTool=View Modes')
local PV2003and2010 = 'PV2003'..LOC('$$$/AgStringUtils/localizedList/finalSeparatorString= and ')..'PV2010'
local PV2010and2012 = 'PV2010'..LOC('$$$/AgStringUtils/localizedList/finalSeparatorString= and ')..'PV2012'
local brush = LOC('$$$/TouchWorkspace/Adjustments/Local/Brush=Brush')
local developPreset = LOC('$$$/AgLibrary/Filter/BrowserCriteria/DevelopPreset/Single=Develop Preset')
local dgh = LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeHueLo=Defringe Green Hue')
local dph = LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeHueLo=Defringe Purple Hue')
local feather = LOC('$$$/AgDevelop/Toolbar/Localized/BrushFeather=Feather')
local filtexp = '. See [Library Filters](https://github.com/rsjaffe/MIDI2LR/wiki/Plugin-Options-Dialog#library-filter) for more information.'
local key = LOC('$$$/MIDI2LR/Shortcuts/Key=Key')
local keyexp = '. See [Keyboard Shortcuts](https://github.com/rsjaffe/MIDI2LR/wiki/Plugin-Options-Dialog#keyboard-shortcuts) for more information.'
local keyword = LOC('$$$/AgLibrary/Filter/BrowserCriteria/Keyword=Keyword')
local larger = LOC('$$$/AgEmail/FontSize/larger=larger')
local locadj = LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments')
local percor = LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/UprightPerspectiveCorrection=Perspective Correction')
local primaryDisplay = LOC('$$$/AgPhotoBin/Tooltip/PrimaryViewMode=Primary Display')
local profexp = 'These profiles attempt to match the camera manufacturer\226\128\153s color appearance under specific settings. If Adobe doesn\226\128\153t have this profile set up for your camera, using it may have unexpected results. Additionally, the code for this particular setting has not been fully tested, so even if Adobe has this profile set for your camera, it may not work. Please [post an issue](https://groups.google.com/forum/#!forum/midi2lr) if this happens.'
local profexp1 = 'These profiles attempt to match the camera manufacturer\226\128\153s color appearance under specific settings. If Adobe doesn\226\128\153t have this profile set up for your camera, using it may have unexpected results.'
local profile = LOC('$$$/AgDevelop/CameraRawPanel/Profile=Profile:')
local quickdev = LOC('$$$/AgLibrary/Menu/Panels/QuickDevelop=Quick develop'):gsub('%&', '')
local repeatexp = ' Not suitable for controls with hard stops like faders. This command not usable in [Series of Commands](https://github.com/rsjaffe/MIDI2LR/wiki/Plugin-Options-Dialog#series-of-commands).'
local reset = LOC('$$$/AgDevelop/Panel/ResetButton=Reset')
local serexp = 'Sends a series of button commands. See [Series of Commands](https://github.com/rsjaffe/MIDI2LR/wiki/Plugin-Options-Dialog#series-of-commands) for more information.'
local setcropasp = LOC('$$$/AgLibrary/CameraRawView/Ops/CropAspect=Set crop aspect ratio')
local show = LOC('$$$/AgLibrary/LibraryInfo/Show=Show')
local size = LOC('$$$/AgDevelop/Toolbar/Localized/BrushSize=Size')
local smaller = LOC('$$$/AgEmail/FontSize/smaller=smaller')
local whiteBalance = LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/WhiteBalance=White Balance')

--Groups (but translation may be used for specific actions as well
--Listing the groups here makes it easier to understand layout, but doesn't change how the program works
--If the group was listed above this comment, it'd still work ok, but it would be more confusing to the reader
local keyshortcuts = LOC('$$$/AgLayoutShortcuts/Header/UISortcuts=Keyboard Shortcuts for User')
local commandseries = LOC('$$$/MIDI2LR/Shortcuts/SeriesofCommands=Series of commands')
local filter= LOC('$$$/Library/Filter/FilterLabel=Library filter')
local view = LOC('$$$/AgLibrary/Help/Shortcuts/Header/ViewShortcuts=View')
local rating = LOC('$$$/AgLibrary/Help/Shortcuts/Header/RatingsShortcuts=Rating')
local flagging = LOC('$$$/AgLibrary/Help/Shortcuts/Header/FlaggingShortcuts=Flagging')
local photos = LOC('$$$/AgLibrary/Help/Shortcuts/Header/ImageShortcuts=Photos')
--local general = LOC('$$$/WFCatSearch/TermCategories/General=General')
--local library = LOC('$$$/AgLibraryModule/ModuleTitle=Library')
local develop = LOC('$$$/SmartCollection/Criteria/Heading/Develop=Develop')
local basicTone = LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/BasicTone=Basic Tone')
local toneCurve = LOC('$$$/AgDevelop/CameraRawPanel/TargetName/ToneCurve=Tone Curve')
local colorAdjustments = LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/ColorAdjustments=Color Adjustments')
local resetColorAdjustments = reset..' '..colorAdjustments
local splitToning = LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/SplitToning=Split Toning')
local detail = LOC('$$$/AgDevelop/Panel/Detail=Detail')
local lensCorrections = LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/LensCorrections=Lens Corrections')
local locadjpre = LOC('$$$/MIDI2LR/LocalPresets/Presets=Local adjustments presets')
local transform = LOC('$$$/AgDevelop/CameraRawPanel/Transform=Transform')
local effects = LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Effects=Effects')
local calibration = LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Calibration=Calibration')
local developPresets = LOC('$$$/AgLibrary/Filter/BrowserCriteria/DevelopPreset/Plural=Develop Presets')
local keywords = LOC('$$$/AgLibrary/Filter/BrowserCriteria/Keywords=Keywords')
local localizedAdjustments = LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/LocalAdjustments=Local Adjustments')
local localadjresets = reset..' '..locadj
local localpresets = locadjpre
local crop = LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Crop')
local gotoToolModulePanel = LOC('$$$/AgDialogs/Select=Select').. ' '..LOC('$$$/AgDevelop/Menu/Tools=Tools'):gsub('&','')..LOC('$$$/AgStringUtils/localizedList/separatorString=, ')..LOC('$$$/Application/Menu/Window/Modules=Modules:'):gsub(':','')..LOC('$$$/AgStringUtils/localizedList/finalSeparatorString= and ')..LOC('$$$/AgPreferences/Interface/GroupTitle/Panels=Panels')
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
  {Command='Key32Key31',Type='repeat',Translation=key..' 32 – '..key..' 31',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key32 signals to Lightroom, counterclockwise Key31.'..repeatexp},
  {Command='Key34Key33',Type='repeat',Translation=key..' 34 – '..key..' 33',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key34 signals to Lightroom, counterclockwise Key33.'..repeatexp},
  {Command='Key36Key35',Type='repeat',Translation=key..' 36 – '..key..' 35',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key36 signals to Lightroom, counterclockwise Key35.'..repeatexp},
  {Command='Key38Key37',Type='repeat',Translation=key..' 38 – '..key..' 37',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key38 signals to Lightroom, counterclockwise Key37.'..repeatexp},
  {Command='Key40Key39',Type='repeat',Translation=key..' 40 – '..key..' 39',Group=keyshortcuts,Explanation='Turning knob clockwise sends Key40 signals to Lightroom, counterclockwise Key39.'..repeatexp},
  --command series
  {Command='ActionSeries1',Type='button',Translation=commandseries .. ' 1',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries2',Type='button',Translation=commandseries .. ' 2',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries3',Type='button',Translation=commandseries .. ' 3',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries4',Type='button',Translation=commandseries .. ' 4',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries5',Type='button',Translation=commandseries .. ' 5',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries6',Type='button',Translation=commandseries .. ' 6',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries7',Type='button',Translation=commandseries .. ' 7',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries8',Type='button',Translation=commandseries .. ' 8',Group=commandseries,Explanation=serexp},
  {Command='ActionSeries9',Type='button',Translation=commandseries .. ' 9',Group=commandseries,Explanation=serexp}, 
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
  --view
  {Command='SwToMlibrary',Type='button',Translation=show..' '..LOC('$$$/AgLibraryModule/ModuleTitle=Library'),Group=view,Explanation='Switch to Library module.'},
  {Command='ShoVwpeople',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgPhotoBin/ViewMode/Library/People=People'),Group=view,Explanation='In the People view, the different faces are organized by people stacks.'},  
  {Command='ShoVwgrid',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgPhotoBin/ViewMode/Library/Grid=Grid'),Group=view,Explanation='Displays photos as thumbnails in cells, which can be viewed in compact and expanded sizes.'},
  {Command='GridViewStyle',Type='button',Translation=LOC('$$$/AgLibrary/Help/Shortcuts/CycleGridViews=Cycle grid views'),Group=view,Explanation='Changes the grid view style. Supported in LR versions 7.4 and later.'},
  {Command='ShoVwloupe',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgPhotoBin/ViewMode/Library/Loupe=Loupe'),Group=view,Explanation='Displays a single photo. Zoom levels up to 11:1 are available.'},
  {Command='CycleLoupeViewInfo',Type='button',Translation=LOC('$$$/AgLibrary/Menu/View/LoupeOverlay=Loupe')..' '..LOC('$$$/AgLibrary/Menu/View/LoupeViewInfo/CycleViewStyle=Cycle view style'):gsub('%&', ''),Group=view,Explanation='Changes the Loupe View Info style. Only works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='ShoVwcompare',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgPhotoBin/ViewMode/Library/Compare=Compare'),Group=view,Explanation='Displays photos side by side so that you can evaluate them.'},
  {Command='ShoVwsurvey',Type='button',Translation=primaryDisplay..' '..LOC('$$$/AgPhotoBin/ViewMode/Library/Survey=Survey'),Group=view,Explanation='Displays the active photo with selected photos so that you can evaluate them. The active photo has the lightest colored cell.'},
  {Command='ShoFullHidePanels',Type='button',Translation=LOC('$$$/Application/Menu/Window/FullScreenHidePanels=Full screen, hide panels'):gsub('%&', ''),Group=view,Explanation='Changes the screen mode to Full Screen and Hide Panels. Supported in LR versions 7.4 and later.'},
  {Command='ShoFullPreview',Type='button',Translation=LOC('$$$/Application/Menu/Window/FullscreenPreview=Full screen preview'):gsub('%&', ''),Group=view,Explanation='Changes the screen mode to Full Screen Preview. Supported in LR versions 7.4 and later.'},
  {Command='NextScreenMode',Type='button',Translation=LOC('$$$/AgLibrary/Help/Shortcuts/CycleToNextScreenMode=Cycle to next screen mode'),Group=view,Explanation='Changes to the next screen mode. Supported in LR versions 7.4 and later.'},
  {Command='ToggleLoupe',Type='button',Translation=LOC('$$$/AgLibrary/Menu/View/ToggleLoupeView=Loupe view activate/inactivate'):gsub('&',''),Group=view,Explanation='Toggle loupe view while in Library. Supported in LR versions 7.4 and later.'},
  {Command='ToggleZoomOffOn',Type='button',Translation=LOC('$$$/AgLibrary/Menu/View/ToggleZoomView=Enable/Disable Zoom'):gsub('&',''),Group=view,Explanation=''},
  {Command='ZoomInOut',Type='repeat',Translation=LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomInSome=Zoom In Some')..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomOutSome=Zoom Out Some'),Group=view,Explanation='Turning knob clockwise zooms in, counterclockwise zooms out.'..repeatexp},    
  {Command='ZoomOutIn',Type='repeat',Translation=LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomOutSome=Zoom Out Some')..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomInSome=Zoom In Some'),Group=view,Explanation='Turning knob clockwise zooms out, counterclockwise zooms in.'..repeatexp},    
  {Command='ZoomInLargeStep',Type='button',Translation=LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomIn=Zoom In'),Group=view,Explanation=''},
  {Command='ZoomInSmallStep',Type='button',Translation=LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomInSome=Zoom In Some'),Group=view,Explanation=''},
  {Command='ZoomOutSmallStep',Type='button',Translation=LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomOutSome=Zoom Out Some'),Group=view,Explanation=''},
  {Command='ZoomOutLargeStep',Type='button',Translation=LOC('$$$/AgApplication/Menu/Window/SecondMonitor/ZoomOut=Zoom Out'),Group=view,Explanation=''},
--rating
  {Command='SetRating0',Type='button',Translation=LOC('$$$/AgLibrary/Filter/Stars=^1 Stars','0'),Group=rating,Explanation=''},
  {Command='SetRating1',Type='button',Translation=LOC('$$$/AgLibrary/Menu/Photo/SetRating/Win/Rating1=1 Star'):gsub('&',''),Group=rating,Explanation=''},
  {Command='SetRating2',Type='button',Translation=LOC('$$$/AgLibrary/Menu/Photo/SetRating/Win/Rating2=&2 Stars'):gsub('&',''),Group=rating,Explanation=''},
  {Command='SetRating3',Type='button',Translation=LOC('$$$/AgLibrary/Menu/Photo/SetRating/Win/Rating3=&3 Stars'):gsub('&',''),Group=rating,Explanation=''},
  {Command='SetRating4',Type='button',Translation=LOC('$$$/AgLibrary/Menu/Photo/SetRating/Win/Rating4=&4 Stars'):gsub('&',''),Group=rating,Explanation=''},
  {Command='SetRating5',Type='button',Translation=LOC('$$$/AgLibrary/Menu/Photo/SetRating/Win/Rating5=&5 Stars'):gsub('&',''),Group=rating,Explanation=''},
  {Command='IncreaseRating',Type='button',Translation=LOC('$$$/AgLibrary/Ops/IncreaseRating=Increase Rating'),Group=rating,Explanation=''},
  {Command='DecreaseRating',Type='button',Translation=LOC('$$$/AgLibrary/Ops/DecreaseRating=Decrease Rating'),Group=rating,Explanation=''},
  --flagging
  {Command='Pick',Type='button',Translation=LOC('$$$/AgLibrary/Help/Shortcuts/SetPick=Set Pick Flag'),Group=flagging,Explanation=''},
  {Command='Reject',Type='button',Translation=LOC('$$$/AgLibrary/Help/Shortcuts/SetReject=Set Rejected Flag'),Group=flagging,Explanation=''},
  {Command='RemoveFlag',Type='button',Translation=LOC('$$$/AgLibrary/Menu/Photo/SetPick/Unflagged=Unflagged'):gsub('&',''),Group=flagging,Explanation=''},
  {Command='ToggleBlue',Type='button',Translation=LOC('$$$/AgLibrary/Undo/ToggleColorLabel=Label ^1 Enable/Disable',LOC('$$$/LibraryImporter/ColorLabelBlue=Blue')),Group=flagging,Explanation=''},
  {Command='ToggleGreen',Type='button',Translation=LOC('$$$/AgLibrary/Undo/ToggleColorLabel=Label ^1 Enable/Disable',LOC('$$$/LibraryImporter/ColorLabelGreen=Green')),Group=flagging,Explanation=''},
  {Command='ToggleRed',Type='button',Translation=LOC('$$$/AgLibrary/Undo/ToggleColorLabel=Label ^1 Enable/Disable',LOC('$$$/LibraryImporter/ColorLabelRed=Red')),Group=flagging,Explanation=''},
  {Command='TogglePurple',Type='button',Translation=LOC('$$$/AgLibrary/Undo/ToggleColorLabel=Label ^1 Enable/Disable',LOC('$$$/LibraryImporter/ColorLabelPurple=Purple')),Group=flagging,Explanation=''},
  {Command='ToggleYellow',Type='button',Translation=LOC('$$$/AgLibrary/Undo/ToggleColorLabel=Label ^1 Enable/Disable',LOC('$$$/LibraryImporter/ColorLabelYellow=Yellow')),Group=flagging,Explanation=''},
  --photos
  {Command='EditPhotoshop',Type='button',Translation=LOC('$$$/AgDevelopShortcuts/Edit_in_Photoshop=Edit in Photoshop'),Group=photos,Explanation='Edit the current photo in Photoshop. Supported in LR versions 7.4 and later.'},  
  {Command='openExportDialog',Type='button',Translation=LOC('$$$/AgLibrary/Menu/Export=Export...'):gsub('%&',''),Group=photos,Explanation='Opens export dialog for all selected photos. Supported in LR versions 7.4 and later.'},
  {Command='openExportWithPreviousDialog',Type='button',Translation=LOC('$$$/AgLibrary/Menu/Export/ExportAgain=Export again'):gsub('%&',''),Group=photos,Explanation='Exports with previous settings for all selected photos. Supported in LR versions 7.4 and later.'},
  {Command='SelectRightLeft',Type='repeat',Translation=LOC('$$$/AgLibrary/Menu/Edit/AddToSelection=Add to Selection')..' '..LOC('$$$/AgWatermarking/Alignment/Left=Left')..' '..LOC('$$$/AgWatermarking/Alignment/Right=Right'),Group=photos,Explanation='Extend selection to right or left. Turning knob clockwise sends select Right signals to Lightroom, counterclockwise select Left.'..repeatexp},   
  {Command='Select1Left',Type='button',Translation=LOC('$$$/AgLibrary/Menu/Edit/AddToSelection=Add to Selection')..' '..LOC('$$$/AgWatermarking/Alignment/Left=Left'),Group=photos,Explanation='Extend selection one picture to the left.'},
  {Command='Select1Right',Type='button',Translation=LOC('$$$/AgLibrary/Menu/Edit/AddToSelection=Add to Selection')..' '..LOC('$$$/AgWatermarking/Alignment/Right=Right'),Group=photos,Explanation='Extend selection one picture to the right.'},
  {Command='NextPrev',Type='repeat',Translation=LOC('$$$/AgDevelopShortcuts/Next_Photo=Next Photo')..' — '..LOC('$$$/AgDevelopShortcuts/Previous_Photo=Previous Photo'),Group=photos,Explanation='Move to next or previous photo. Turning knob clockwise sends Next signals to Lightroom, counterclockwise Previous.'..repeatexp},
  {Command='Next',Type='button',Translation=LOC('$$$/AgDevelopShortcuts/Next_Photo=Next Photo'),Group=photos,Explanation=''},
  {Command='Prev',Type='button',Translation=LOC('$$$/AgDevelopShortcuts/Previous_Photo=Previous Photo'),Group=photos,Explanation=''},
  {Command='RotateLeft',Type='button',Translation=LOC('$$$/AgDevelopShortcuts/Rotate_left=Rotate left'),Group=photos,Explanation='Rotates all selected photos left. Supported in LR versions 7.4 and later.'},
  {Command='RotateRight',Type='button',Translation=LOC('$$$/AgDevelopShortcuts/Rotate_right=Rotate right'),Group=photos,Explanation='Rotates all selected photos right. Supported in LR versions 7.4 and later.'},

  --quick develop
  {Command='QuickDevWBAuto',Type='button',Translation=quickdev..' '..whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Auto=Auto'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets auto white balance for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='QuickDevWBDaylight',Type='button',Translation=quickdev..' '..whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Daylight=Daylight'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets daylight white balance for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='QuickDevWBCloudy',Type='button',Translation=quickdev..' '..whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Cloudy=Cloudy'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets cloudy white balance for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='QuickDevWBShade',Type='button',Translation=quickdev..' '..whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Shade=Shade'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets shade white balance for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='QuickDevWBTungsten',Type='button',Translation=quickdev..' '..whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Tungsten=Tungsten'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets tungsten white balance for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='QuickDevWBFluorescent',Type='button',Translation=quickdev..' '..whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Fluorescent=Fluorescent'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets fluorescent white balance for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='QuickDevWBFlash',Type='button',Translation=quickdev..' '..whiteBalance..' '..LOC('$$$/AgCameraRawUI/WhiteBalance/Flash=Flash'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets Auto flash balance for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='SetTreatmentBW',Type='button',Translation=quickdev..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/SetGrayscale=Set treatment B&W'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets the treatment to B&W for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='SetTreatmentColor',Type='button',Translation=quickdev..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/TreatmentColor=Set treatment Color'),Group=quickdev,Explanation='Using the Library\226\128\153s quick develop mode, sets the treatment to Color for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='QuickDevCropAspectOriginal',Type='button',Translation=quickdev..' '..setcropasp..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/CropName/Original=Original'),Group=quickdev,Explanation='Set crop ratio to original. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='QuickDevCropAspectAsShot',Type='button',Translation=quickdev..' '..setcropasp..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/CropName/AsShot=As shot'),Group=quickdev,Explanation='Set crop ratio to as shot. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='QuickDevCropAspect1x1',Type='button',Translation=quickdev..' '..setcropasp..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/CropNameConstruction=^1 x ^2','1','1'),Group=quickdev,Explanation='Set crop ratio to 1 by 1. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='QuickDevCropAspect4x5',Type='button',Translation=quickdev..' '..setcropasp..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/CropNameConstruction=^1 x ^2','4','5'),Group=quickdev,Explanation='Set crop ratio to 4 by 5. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='QuickDevCropAspect85x11',Type='button',Translation=quickdev..' '..setcropasp..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/CropNameConstruction=^1 x ^2','8.5','11'),Group=quickdev,Explanation='Set crop ratio to 8.5 by 11. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='QuickDevCropAspect5x7',Type='button',Translation=quickdev..' '..setcropasp..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/CropNameConstruction=^1 x ^2','5','7'),Group=quickdev,Explanation='Set crop ratio to 5 by 7. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},
  {Command='QuickDevCropAspect2x3',Type='button',Translation=quickdev..' '..setcropasp..' '..LOC('$$$/AgLibrary/CameraRawView/Ops/CropNameConstruction=^1 x ^2','2','3'),Group=quickdev,Explanation='Set crop ratio to 2 by 3. Using the Library\226\128\153s quick develop mode, sets crop aspect for all selected photos. Works in Library and Develop modules. Supported in LR versions 7.4 and later.'},


  --Develop
  {Command='SwToMdevelop',Type='button',Translation=show..' '..LOC('$$$/SmartCollection/Criteria/Heading/Develop=Develop'),Group=develop,Explanation='Switch to Develop module.'},
  --develop: copy paste sync
  {Command='LRCopy',Type='button',Translation='Lightroom '..LOC('$$$/AgLibrary/Menu/Develop/CopySettings=Copy Settings'):gsub('&',''),Group=develop,Explanation='Lightroom Copy (open the selection box).'},
  {Command='LRPaste',Type='button',Translation='Lightroom '..LOC('$$$/AgCameraRawNamedSettings/Ops/PasteSettings=Paste Settings'),Group=develop,Explanation='Lightroom Paste.'},
  {Command='VirtualCopy',Type='button',Translation=LOC('$$$/AgLibrary/Bezel/CreateVirtualCopy=Create Virtual Copy'):gsub('&',''),Group=develop,Explanation='Creates a virtual copy for each of the currently selected photos and videos. The new virtual copies will be selected.'},
  {Command='ResetAll',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/Ops/ResetSettings=Reset Settings'),Group=develop,Explanation='Reset to defaults.'},
  {Command='ResetLast',Type='button',Translation=reset..' '..LOC('$$$/LibraryUpgradeCatalogUtils/CatalogInfo/LastModified/Label=Last Modified'):gsub(':',''),Group=develop,Explanation='Resets the last parameter that was adjusted by an encoder or fader to default.'},
  {Command='ChangeLastDevelopParameter',Type='repeat',Translation=LOC('$$$/ImportView/More=More')..' – '..LOC('$$$/ImportView/Less=Less')..' '..LOC('$$$/LibraryUpgradeCatalogUtils/CatalogInfo/LastModified/Label=Last Modified'),Group=develop,Explanation='Increments or decrements the last parameter that was adjusted by an encoder or fader. Turning knob clockwise sends Increment signals to Lightroom, counterclockwise Decrement.'..repeatexp},
  {Command='IncrementLastDevelopParameter',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Increase=^1 Increase: ^2','',LOC('$$$/LibraryUpgradeCatalogUtils/CatalogInfo/LastModified/Label=Last Modified')):gsub(':',''):gsub('^%s*(.-)%s*$', '%1'),Group=develop,Explanation='Increments the last parameter that was adjusted by an encoder or fader.'},
  {Command='DecrementLastDevelopParameter',Type='button',Translation=LOC('$$$/AgCameraRawController/TargetAdjustment/Decrease=^1 Decrease: ^2','',LOC('$$$/LibraryUpgradeCatalogUtils/CatalogInfo/LastModified/Label=Last Modified')):gsub(':',''):gsub('^%s*(.-)%s*$', '%1'),Group=develop,Explanation='Decrements the last parameter that was adjusted by an encoder or fader.'},
  {Command='ChangeCurrentSlider',Type='repeat',Translation=LOC('$$$/ImportView/More=More')..' – '..LOC('$$$/ImportView/Less=Less'),Group=develop,Explanation='Adjusts selected develop slider up or down. Turning knob clockwise sends Increment signals to Lightroom, counterclockwise Decrement.'..repeatexp},
  {Command='SliderIncrease',Type='button',Translation=LOC('$$$/ImportView/More=More'),Group=develop,Explanation='Increases the selected develop slider.'},
  {Command='SliderDecrease',Type='button',Translation=LOC('$$$/ImportView/Less=Less'),Group=develop,Explanation='Decreases the selected develop slider.'},
  {Command='RedoUndo',Type='repeat',Translation=LOC('$$$/Bezel/RedoTitle=Redo')..' – '..LOC('$$$/AgCameraRawController/SoftProofingVirtualCopyPrompt/Undo=Undo'),Group=develop,Explanation='Turning knob clockwise sends Redo signals to Lightroom, counterclockwise Undo.'..repeatexp},
  {Command='Undo',Type='button',Translation=LOC('$$$/AgCameraRawController/SoftProofingVirtualCopyPrompt/Undo=Undo'),Group=develop,Explanation=''},
  {Command='Redo',Type='button',Translation=LOC('$$$/Bezel/RedoTitle=Redo'),Group=develop,Explanation=''},
  {Command='ShowClipping',Type='button',Translation=LOC('$$$/AgDevelop/Histogram/Menu/ShowClippingIndicators=Show clipping'),Group=develop,Explanation='Toggles clipping indicators on/off. Must be called while the Develop module is active. Supported in LR versions 7.4 and later.'},
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
  {Command='RevealPanelAdjust',Type='button',Translation=show..' '..basicTone,Group=basicTone,Explanation='Open Basic Adjustments Panel in Develop Module.'},
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
  {Command='Tint',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Tint=Tint'),Group=basicTone,Explanation='Fine-tunes the white balance to compensate for a green or magenta tint. Move the slider to the left (negative values) to add green to the photo; move it to the right (positive values) to add magenta.',Panel='adjustPanel'},
  {Command='Exposure',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Exposure=Exposure'),Group=basicTone,Explanation='Sets the overall image brightness. Adjust the slider until the photo looks good and the image is the desired brightness.',Panel='adjustPanel'},
  {Command='Contrast',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Contrast=Contrast'),Group=basicTone,Explanation='Increases or decreases image contrast, mainly affecting midtones. When you increase contrast, the middle-to-dark image areas become darker, and the middle-to-light image areas become lighter. The image tones are inversely affected as you decrease contrast.',Panel='adjustPanel'},
  {Command='Highlights',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Highlights=Highlights')..' ('..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HighlightRecovery=Highlight Recovery')..' in '..PV2003and2010..')',Group=basicTone,Explanation='Adjusts bright image areas. Drag to the left to darken highlights and recover \226\128\156blown out\226\128\157 highlight details. Drag to the right to brighten highlights while minimizing clipping.<br /><br />In '..PV2003and2010..', controls recovery and reduces the tones of extreme highlights and attempts to recover highlight detail lost because of camera overexposure. Lightroom can recover detail in raw image files if one or two channels are clipped.',Panel='adjustPanel'},
  {Command='Brightness',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Brightness=Brightness'),Group=basicTone,Explanation='Adjusts image brightness, mainly affecting midtones. Adjust Brightness after setting Exposure, Recovery, and Blacks sliders. Large brightness adjustments can affect shadow or highlight clipping, so you may want to readjust the Exposure, Recovery, or Blacks slider after adjusting brightness. No effect unless in PV2003 or PV2010)',Panel='adjustPanel'},
  {Command='Shadows',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Shadows=Shadows')..' ('..LOC('$$$/AgCameraRawUI/FillLight=Fill Light')..' in '..PV2003and2010..')',Group=basicTone,Explanation='Adjusts dark image areas. Drag to the left to darken shadows while minimizing clipping. Drag to the right to brighten shadows and recover shadow details.<br /><br />In '..PV2003and2010..', controls Fill Light, and lightens shadow to reveal more detail while maintaining blacks. Take care not to over apply the setting and reveal image noise.',Panel='adjustPanel'},
  {Command='Whites',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Whites=Whites')..' ('..LOC('$$$/MIDI2LR/Versions/notfor=doesn\226\128\153t work with')..' '..PV2003and2010..')',Group=basicTone,Explanation='Adjusts white clipping. Drag to the left to reduce clipping in highlights. Drag to the right to increase highlight clipping.',Panel='adjustPanel'},
  {Command='Blacks',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Blacks=Blacks'),Group=basicTone,Explanation='Specifies which image values map to black. Moving the slider to the right increases the areas that become black, sometimes creating the impression of increased image contrast. The greatest effect is in the shadows, with much less change in the midtones and highlights.',Panel='adjustPanel'},
  {Command='Clarity',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Clarity=Clarity'),Group=basicTone,Explanation='Adds depth to an image by increasing local contrast. When using this setting, it is best to zoom in to 100% or greater.',Panel='adjustPanel'},
  {Command='Dehaze',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/DehazeAmount=Dehaze Amount'),Group=basicTone,Explanation='Controls the amount of haze in a photograph. Drag to the right to remove haze; drag to the left to add haze.',Panel='adjustPanel'},
  {Command='Vibrance',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Vibrance=Vibrance'),Group=basicTone,Explanation='Adjusts the saturation so that clipping is minimized as colors approach full saturation, changing the saturation of all lower-saturated colors with less effect on the higher-saturated colors. Vibrance also prevents skin tones from becoming over saturated.',Panel='adjustPanel'},
  {Command='Saturation',Type='parameter',Translation=LOC('$$$/AgCameraRawUI/Saturation=Saturation'),Group=basicTone,Explanation='Adjusts the saturation of all image colors equally from -100 (monochrome) to +100 (double the saturation).',Panel='adjustPanel'},
  {Command='ResetTemperature',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Temperature=Temperature'),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetTint',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Tint=Tint'),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetExposure',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawUI/Exposure=Exposure'),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetContrast',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawUI/Contrast=Contrast'),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetHighlights',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Highlights=Highlights'),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetShadows',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Shadows=Shadows'),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetWhites',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawUI/Whites=Whites'),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetBlacks',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawUI/Blacks=Blacks'),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetClarity',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Clarity=Clarity'),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetDehaze',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/DehazeAmount=Dehaze Amount'),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},  
  {Command='ResetVibrance',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawUI/Vibrance=Vibrance'),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
  {Command='ResetSaturation',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawUI/Saturation=Saturation'),Group=basicTone,Explanation='Reset to default.',Panel='adjustPanel'},
--
--develop: tone curve panel
--
  {Command='RevealPanelTone',Type='button',Translation=show..' '..toneCurve,Group=toneCurve,Explanation='Open Tone Curve Panel in Develop Module.'},
  {Command='EnableToneCurve',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableToneCurve=Enable Tone Curve'),Group=toneCurve,Explanation='Enable or disable tone curve.  Supported in LR versions 7.4 and later.',Panel='tonePanel'},
  {Command='ParametricDarks',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneDarks=Dark Tones'),Group=toneCurve,Explanation='Adjust darks.',Panel='tonePanel'},
  {Command='ParametricLights',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneLights=Light Tones'),Group=toneCurve,Explanation='Adjust lights.',Panel='tonePanel'},
  {Command='ParametricShadows',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadows=Shadow Tones'),Group=toneCurve,Explanation='Adjust shadows.',Panel='tonePanel'},
  {Command='ParametricHighlights',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlights=Highlight Tones'),Group=toneCurve,Explanation='Adjust highlights.',Panel='tonePanel'},
  {Command='ParametricShadowSplit',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadowSplit=Shadow Split'),Group=toneCurve,Explanation='Move division between shadows and darks.',Panel='tonePanel'},
  {Command='ParametricMidtoneSplit',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneMidtoneSplit=Midtone Split'),Group=toneCurve,Explanation='Move division between darks and lights.',Panel='tonePanel'},
  {Command='ParametricHighlightSplit',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlightSplit=Highlight Split'),Group=toneCurve,Explanation='Move division between lights and highlights.',Panel='tonePanel'},
  {Command='ResetParametricDarks',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneDarks=Dark Tones'),Group=toneCurve,Explanation='Reset to default.',Panel='tonePanel'},
  {Command='ResetParametricLights',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneLights=Light Tones'),Group=toneCurve,Explanation='Reset to default.',Panel='tonePanel'},
  {Command='ResetParametricShadows',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadows=Shadow Tones'),Group=toneCurve,Explanation='Reset to default.',Panel='tonePanel'},
  {Command='ResetParametricHighlights',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlights=Highlight Tones'),Group=toneCurve,Explanation='Reset to default.',Panel='tonePanel'},
  {Command='ResetParametricShadowSplit',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadowSplit=Shadow Split'),Group=toneCurve,Explanation='Reset to default.',Panel='tonePanel'},
  {Command='ResetParametricMidtoneSplit',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneMidtoneSplit=Midtone Split'),Group=toneCurve,Explanation='Reset to default.',Panel='tonePanel'},
  {Command='ResetParametricHighlightSplit',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlightSplit=Highlight Split'),Group=toneCurve,Explanation='Reset to default.',Panel='tonePanel'},
  {Command='PointCurveLinear',Type='button',Translation=toneCurve..' '..LOC('$$$/CRaw/ToneCurvePreset/Linear=Linear'),Group=toneCurve,Explanation='Linear Point Curve.',Panel='tonePanel'},
  {Command='PointCurveMediumContrast',Type='button',Translation=toneCurve..' '..LOC('$$$/CRaw/ToneCurvePreset/MediumContrast=Medium Contrast'),Group=toneCurve,Explanation='Medium Contrast Point Curve.',Panel='tonePanel'},
  {Command='PointCurveStrongContrast',Type='button',Translation=toneCurve..' '..LOC('$$$/CRaw/ToneCurvePreset/StrongContrast=Strong Contrast'),Group=toneCurve,Explanation='Strong Contrast Point Curve.',Panel='tonePanel'},
--
--develop: mixer panel
--
  {Command='RevealPanelMixer',Type='button',Translation=show..' '..colorAdjustments,Group=colorAdjustments,Explanation='Open Mixer Panel in Develop Module.'},
  {Command='EnableColorAdjustments',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableColorAdjustments=Enable Color Adjustments'),Group=colorAdjustments,Explanation='Enable or disable color adjustments.',Panel='mixerPanel'},
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
  {Command='EnableGrayscaleMix',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableGrayscaleMix=Enable Grayscale Mix'),Group=colorAdjustments,Explanation='Enable or disable black and white mix.',Panel='mixerPanel'},
  {Command='GrayMixerRed',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerRed=Gray Mixer Red'),Group=colorAdjustments,Explanation='Red contribution to luminance.',Panel='mixerPanel'},
  {Command='GrayMixerOrange',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerOrange=Gray Mixer Orange'),Group=colorAdjustments,Explanation='Orange contribution to luminance.',Panel='mixerPanel'},
  {Command='GrayMixerYellow',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerYellow=Gray Mixer Yellow'),Group=colorAdjustments,Explanation='Yellow contribution to luminance.',Panel='mixerPanel'},
  {Command='GrayMixerGreen',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerGreen=Gray Mixer Green'),Group=colorAdjustments,Explanation='Green contribution to luminance.',Panel='mixerPanel'},
  {Command='GrayMixerAqua',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerAqua=Gray Mixer Aqua'),Group=colorAdjustments,Explanation='Aqua contribution to luminance.',Panel='mixerPanel'},
  {Command='GrayMixerBlue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerBlue=Gray Mixer Blue'),Group=colorAdjustments,Explanation='Blue contribution to luminance.',Panel='mixerPanel'},
  {Command='GrayMixerPurple',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerPurple=Gray Mixer Purple'),Group=colorAdjustments,Explanation='Purple contribution to luminance.',Panel='mixerPanel'},
  {Command='GrayMixerMagenta',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerMagenta=Gray Mixer Magenta'),Group=colorAdjustments,Explanation='Magenta contribution to luminance.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentRed',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentRed=Saturation Adjustment Red'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentOrange',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentOrange=Saturation Adjustment Orange'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentYellow',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentYellow=Saturation Adjustment Yellow'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentGreen',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentGreen=Saturation Adjustment Green'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentAqua',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentAqua=Saturation Adjustment Aqua'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentBlue',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentBlue=Saturation Adjustment Blue'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentPurple',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentPurple=Saturation Adjustment Purple'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetSaturationAdjustmentMagenta',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentMagenta=Saturation Adjustment Magenta'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetAllSaturationAdjustment',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Saturation=Saturation')..' \226\128\148 '..LOC('$$$/AgDateManagement/ChangeAll=Change all'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentRed',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentRed=Hue Adjustment Red'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentOrange',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentOrange=Hue Adjustment Orange'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentYellow',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentYellow=Hue Adjustment Yellow'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentGreen',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentGreen=Hue Adjustment Green'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentAqua',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentAqua=Hue Adjustment Aqua'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentBlue',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentBlue=Hue Adjustment Blue'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentPurple',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentPurple=Hue Adjustment Purple'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetHueAdjustmentMagenta',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentMagenta=Hue Adjustment Magenta'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentRed',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentRed=Luminance Adjustment Red'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentOrange',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentOrange=Luminance Adjustment Orange'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentYellow',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentYellow=Luminance Adjustment Yellow'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentGreen',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentGreen=Luminance Adjustment Green'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentAqua',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentAqua=Luminance Adjustment Aqua'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentBlue',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentBlue=Luminance Adjustment Blue'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentPurple',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentPurple=Luminance Adjustment Purple'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetLuminanceAdjustmentMagenta',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentMagenta=Luminance Adjustment Magenta'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerRed',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerRed=Gray Mixer Red'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerOrange',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerOrange=Gray Mixer Orange'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerYellow',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerYellow=Gray Mixer Yellow'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerGreen',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerGreen=Gray Mixer Green'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerAqua',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerAqua=Gray Mixer Aqua'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerBlue',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerBlue=Gray Mixer Blue'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerPurple',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerPurple=Gray Mixer Purple'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
  {Command='ResetGrayMixerMagenta',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerMagenta=Gray Mixer Magenta'),Group=resetColorAdjustments,Explanation='Reset to default.',Panel='mixerPanel'},
--
--develop: split toning panel
--
  {Command='RevealPanelSplit',Type='button',Translation=show..' '..splitToning,Group=splitToning,Explanation='Open Split Toning Panel in Develop Module.'},
  {Command='EnableSplitToning',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableSplitToning=Enable Split Toning'),Group=splitToning,Explanation='Enable or disable split toning effects.',Panel='splitToningPanel'},
  {Command='SplitToningShadowHue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowHue=Shadow Hue'),Group=splitToning,Explanation='Color of the tone for shadows.',Panel='splitToningPanel'},
  {Command='SplitToningShadowSaturation',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowSaturation=Shadow Saturation'),Group=splitToning,Explanation='Strength of the effect.',Panel='splitToningPanel'},
  {Command='SplitToningHighlightHue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightHue=Highlight Hue'),Group=splitToning,Explanation='Color of the tone for highlights.',Panel='splitToningPanel'},
  {Command='SplitToningHighlightSaturation',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightSaturation=Highlight Saturation'),Group=splitToning,Explanation='Strength of the effect.',Panel='splitToningPanel'},
  {Command='SplitToningBalance',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningBalance=Split Toning Balance'),Group=splitToning,Explanation='Set the Balance slider to balance the effect between the Highlight and Shadow sliders. Positive values increase the effect of the Highlight sliders; negative values increase the effect of the Shadow sliders.',Panel='splitToningPanel'},
  {Command='ResetSplitToningShadowHue',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowHue=Shadow Hue'),Group=splitToning,Explanation='Reset to default.',Panel='splitToningPanel'},
  {Command='ResetSplitToningShadowSaturation',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowSaturation=Shadow Saturation'),Group=splitToning,Explanation='Reset to default.',Panel='splitToningPanel'},
  {Command='ResetSplitToningHighlightHue',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightHue=Highlight Hue'),Group=splitToning,Explanation='Reset to default.',Panel='splitToningPanel'},
  {Command='ResetSplitToningHighlightSaturation',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightSaturation=Highlight Saturation'),Group=splitToning,Explanation='Reset to default.',Panel='splitToningPanel'},
  {Command='ResetSplitToningBalance',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningBalance=Split Toning Balance'),Group=splitToning,Explanation='Reset to default.',Panel='splitToningPanel'},
--
--develop: detail panel
--
  {Command='RevealPanelDetail',Type='button',Translation=show..' '..detail,Group=detail,Explanation='Open Detail Panel in Develop Module.'},
  {Command='EnableDetail',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableDetail=Enable Detail'),Group=detail,Explanation='Enable or disable noise reduction and sharpening.',Panel='detailPanel'},
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
  {Command='ResetSharpness',Type='button',Translation=reset..' '..LOC('$$$/AgDevelop/Localized/Sharpness=Sharpness'),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetSharpenRadius',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenRadius=Sharpen Radius'),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetSharpenDetail',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenDetail=Sharpen Detail'),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetSharpenEdgeMasking',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenEdgeMasking=Sharpen Edge Masking'),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetLuminanceSmoothing',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceSmoothing=Luminance Smoothing'),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetLuminanceNoiseReductionDetail',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionDetail=Luminance Detail'),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetLuminanceNoiseReductionContrast',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionContrast=Luminance Contrast'),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetColorNoiseReduction',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReduction=Color Noise Reduction'),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetColorNoiseReductionDetail',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionDetail=Color Noise Reduction Detail'),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
  {Command='ResetColorNoiseReductionSmoothness',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionSmoothness=Color Noise Reduction Smoothness'),Group=detail,Explanation='Reset to default.',Panel='detailPanel'},
--
--develop: lens corrections panel
--
  {Command='RevealPanelLens',Type='button',Translation=show..' '..lensCorrections,Group=lensCorrections,Explanation='Open Lens Corrections Panel in Develop Module.'},
  {Command='EnableLensCorrections',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableLensCorrections=Enable Lens Corrections'),Group=lensCorrections,Explanation='Enable or disable all lens corrections.',Panel='lensCorrectionsPanel'},
  {Command='LensProfileEnable',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileEnable=Toggle Profile Corrections'),Group=lensCorrections,Explanation='Enable or disable lens profile correction.',Panel='lensCorrectionsPanel'},
  {Command='AutoLateralCA',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RemoveChromaticAberration=Remove Chromatic Aberration'),Group=lensCorrections,Explanation='Toggle Remove Chromatic Aberration.',Panel='lensCorrectionsPanel'},
  {Command='LensProfileDistortionScale',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileDistortionScale=Lens Profile Distortion Scale'),Group=lensCorrections,Explanation='The default value 100 applies 100% of the distortion correction in the profile. Values over 100 apply greater correction to the distortion; values under 100 apply less correction to the distortion.',Panel='lensCorrectionsPanel'},
  {Command='LensProfileChromaticAberrationScale',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileChromaticAberrationScale=Lens Profile Chromatic Aberration Scale'),Group=lensCorrections,Explanation='The default value 100 applies 100% of the chromatic aberration correction in the profile. Values over 100 apply greater correction to color fringing; values under 100 apply less correction to color fringing.',Panel='lensCorrectionsPanel'},
  {Command='LensProfileVignettingScale',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileVignettingScale=Lens Profile Vignetting Scale'),Group=lensCorrections,Explanation='The default value 100 applies 100% of the vignetting correction in the profile. Values over 100 apply greater correction to vignetting; values under 100 apply less correction to vignetting.',Panel='lensCorrectionsPanel'},
  {Command='CropConstrainToWarp',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CropConstrainToWarp=Constrain to Warp'),Group=lensCorrections,Explanation='Enable or disable Constrain Crop.',Panel='lensCorrectionsPanel'},
  {Command='ResetLensProfileDistortionScale',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileDistortionScale=Lens Profile Distortion Scale'),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetLensProfileChromaticAberrationScale',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileChromaticAberrationScale=Lens Profile Chromatic Aberration Scale'),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetLensProfileVignettingScale',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileVignettingScale=Lens Profile Vignetting Scale'),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='DefringePurpleAmount',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeAmount=Defringe Purple Amount'),Group=lensCorrections,Explanation='You can adjust the affected purple or green hue range using the Purple Hue and the Green Hue sliders. Drag either end-point control to expand or decrease the range of affected colors. Drag between the end point controls to move the hue range. The minimum space between end points is ten units.',Panel='lensCorrectionsPanel'},
  {Command='DefringePurpleHueLo',Type='parameter',Translation=dph..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/Low=Low'),Group=lensCorrections,Explanation='See \226\128\152Defringe Purple Amount\226\128\153 description for details.',Panel='lensCorrectionsPanel'},
  {Command='DefringePurpleHueHi',Type='parameter',Translation=dph..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/High=High'),Group=lensCorrections,Explanation='See \226\128\152Defringe Purple Amount\226\128\153 description for details.',Panel='lensCorrectionsPanel'},
  {Command='DefringeGreenAmount',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeAmount=Defringe Green Amount'),Group=lensCorrections,Explanation='You can adjust the affected purple or green hue range using the Purple Hue and the Green Hue sliders. Drag either end-point control to expand or decrease the range of affected colors. Drag between the end point controls to move the hue range. The minimum space between end points is ten units. The default spacing for the green sliders is narrow to protect green/yellow image colors, like foliage.',Panel='lensCorrectionsPanel'},
  {Command='DefringeGreenHueLo',Type='parameter',Translation=dgh..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/Low=Low'),Group=lensCorrections,Explanation='See \226\128\152Defringe Green Amount\226\128\153 description for details.',Panel='lensCorrectionsPanel'},
  {Command='DefringeGreenHueHi',Type='parameter',Translation=dgh..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/High=High'),Group=lensCorrections,Explanation='See \226\128\152Defringe Green Amount\226\128\153 description for details.',Panel='lensCorrectionsPanel'},
  {Command='ResetDefringePurpleAmount',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeAmount=Defringe Purple Amount'),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetDefringePurpleHueLo',Type='button',Translation=reset..' '..dph..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/Low=Low'),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetDefringePurpleHueHi',Type='button',Translation=reset..' '..dph..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/High=High'),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetDefringeGreenAmount',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeAmount=Defringe Green Amount'),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetDefringeGreenHueLo',Type='button',Translation=reset..' '..dgh..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/Low=Low'),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetDefringeGreenHueHi',Type='button',Translation=reset..' '..dgh..' - '..LOC('$$$/Slideshow/Panels/PanAndZoom/High=High'),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='LensManualDistortionAmount',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensManualDistortionAmount=Lens Manual Distortion Amount'),Group=lensCorrections,Explanation='Drag to the right to correct barrel distortion and straighten lines that bend away from the center. Drag to the left to correct pincushion distortion and straighten lines that bend toward the center.',Panel='lensCorrectionsPanel'},
  {Command='VignetteAmount',Type='parameter',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteAmount=Vignette Amount'),Group=lensCorrections,Explanation='Sets the amount of lightening or darkening along the edges of an image. Corrects images that have darkened corners caused by lens faults or improper lens shading.',Panel='lensCorrectionsPanel'},
  {Command='VignetteMidpoint',Type='parameter',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteMidpoint=Vignette Midpoint'),Group=lensCorrections,Explanation='Specifies the width of area affected by the Amount slider. Specify a lower number to affect more of the image. Specify a higher number to restrict the effect to the edges of the image.',Panel='lensCorrectionsPanel'},
  {Command='ResetLensManualDistortionAmount',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensManualDistortionAmount=Lens Manual Distortion Amount'),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetVignetteAmount',Type='button',Experimental=true,Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteAmount=Vignette Amount'),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
  {Command='ResetVignetteMidpoint',Type='button',Experimental=true,Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteMidpoint=Vignette Midpoint'),Group=lensCorrections,Explanation='Reset to default.',Panel='lensCorrectionsPanel'},
--
--develop: transform panel menu
--
  {Command='RevealPanelTransform',Type='button',Translation=show..' '..transform,Group=transform,Explanation='Open Transform Panel in Develop Module.'},
  {Command='EnableTransform',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableTransform=Enable Transform'),Group=transform,Explanation='Enable or disable all tranforms.',Panel='transformPanel'},
  {Command='UprightOff',Type='button',Translation=percor..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ValueOff=Off'),Group=transform,Explanation='Upright mode off.',Panel='transformPanel'},
  {Command='UprightAuto',Type='button',Translation=percor..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ValueAuto=Auto'),Group=transform,Explanation='Balanced level, aspect ratio, and perspective corrections.',Panel='transformPanel'},
  {Command='UprightLevel',Type='button',Translation=percor..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ValueLevel=Level'),Group=transform,Explanation='Perspective corrections are weighted toward horizontal details.',Panel='transformPanel'},
  {Command='UprightVertical',Type='button',Translation=percor..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ValueLevelVertical=Vertical'),Group=transform,Explanation='Perspective corrections are weighted toward vertical details.',Panel='transformPanel'},
  {Command='UprightGuided',Type='button',Translation=percor..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ValueLevelGuided=Guided'),Group=transform,Explanation='Perspective corrections guide by drawing lines.',Panel='transformPanel'},
  {Command='UprightFull',Type='button',Translation=percor..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ValueFull=Full'),Group=transform,Explanation='Combination of full Level, Vertical, and Auto perspective corrections.',Panel='transformPanel'},
  {Command='ResetPerspectiveUpright',Type='button',Translation=LOC('$$$/AgCameraRawNamedSettings/Ops/ResetUprightSettings=Reset Upright Settings'),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
  {Command='PerspectiveVertical',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveVertical=Perspective Vertical'),Group=transform,Explanation='Corrects perspective caused by tilting the camera up or down. Makes vertical lines appear parallel.',Panel='transformPanel'},
  {Command='PerspectiveHorizontal',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveHorizontal=Perspective Horizontal'),Group=transform,Explanation='Corrects perspective caused by angling the camera left or right. Makes horizontal lines parallel.',Panel='transformPanel'},
  {Command='PerspectiveRotate',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveRotate=Perspective Rotate'),Group=transform,Explanation='Corrects for camera tilt. Uses the center of the original, uncropped photo as the axis of rotation.',Panel='transformPanel'},
  {Command='PerspectiveScale',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveScale=Perspective Scale'),Group=transform,Explanation='Adjusts the image scale up or down. Helps to remove empty areas caused by perspective corrections and distortions. Displays areas of the image that extend beyond the crop boundary.',Panel='transformPanel'},
  {Command='PerspectiveAspect',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveAspect=Perspective Aspect'),Group=transform,Explanation='Adjusts the amount the image is stretched horizontally or vertically.',Panel='transformPanel'},
  {Command='PerspectiveX',Type='parameter',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveX=Perspective X'),Group=transform,Explanation='Moves the center of the image laterally.',Panel='transformPanel'},
  {Command='PerspectiveY',Type='parameter',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveY=Perspective Y'),Group=transform,Explanation='Moves the center of the image vertically.',Panel='transformPanel'},
  {Command='ResetTransforms',Type='button',Translation=LOC("$$$/AgLibrary/Ops/ResetTransforms=Reset all transforms"),Group=transform,Explanation='Clears all transforms from the current photo. Must be called while the Develop module is active. Supported in LR versions 6.6 and later.',Panel='transformPanel'},  
  {Command='ResetPerspectiveVertical',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveVertical=Perspective Vertical'),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
  {Command='ResetPerspectiveHorizontal',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveHorizontal=Perspective Horizontal'),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
  {Command='ResetPerspectiveRotate',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveRotate=Perspective Rotate'),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
  {Command='ResetPerspectiveScale',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveScale=Perspective Scale'),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
  {Command='ResetPerspectiveAspect',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveAspect=Perspective Aspect'),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
  {Command='ResetPerspectiveX',Type='button',Experimental=true,Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveX=Perspective X'),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
  {Command='ResetPerspectiveY',Type='button',Experimental=true,Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveY=Perspective Y'),Group=transform,Explanation='Reset to default.',Panel='transformPanel'},
--
--develop: effects panel
--
  {Command='RevealPanelEffects',Type='button',Translation=show..' '..effects,Group=effects,Explanation='Open Effects Panel in Develop Module.'},
  {Command='EnableEffects',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableEffects=Enable Effects'),Group=effects,Explanation='Enable or disable effects.',Panel='effectsPanel'},
  {Command='PostCropVignetteAmount',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteAmount=Post Crop Vignette Amount'),Group=effects,Explanation='Negative values darken the corners of the photo. Positive values lighten the corners.',Panel='effectsPanel'},
  {Command='PostCropVignetteMidpoint',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteMidpoint=Post Crop Vignette Midpoint'),Group=effects,Explanation='Lower values apply the Amount adjustment to a larger area away from the corners. Higher values restrict the adjustment to an area closer to the corners.',Panel='effectsPanel'},
  {Command='PostCropVignetteFeather',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteFeather=Post Crop Vignette Feather'),Group=effects,Explanation='Lower values reduce softening between the vignette and the vignette\226\128\153s surrounding pixels. Higher values increase the softening.',Panel='effectsPanel'},
  {Command='PostCropVignetteRoundness',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteRoundness=Post Crop Vignette Roundness'),Group=effects,Explanation='Lower values make the vignette effect more oval. Higher values make the vignette effect more circular.',Panel='effectsPanel'},
  {Command='PostCropVignetteStyle',Type='variable',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteStyle=Post Crop Vignette Style'),Group=effects,Explanation='Cycles through: **Highlight Priority** Enables highlight recovery but can lead to color shifts in darkened areas of a photo. Suitable for photos with bright image areas such as clipped specular highlights. **Color Priority** Minimizes color shifts in darkened areas of a photo but cannot perform highlight recovery. **Paint Overlay** Mixes the cropped image values with black or white pixels. Can result in a flat appearance.',Panel='effectsPanel'},
  {Command='PostCropVignetteStyleHighlightPriority',Type='button',Translation=LOC('$$$/AgCameraRawController/PostCropVignetteStyle/Exposure1=Highlight Priority'),Group=effects,Explanation='Post Crop Vignette Style: Highlight Priority.',Panel='effectsPanel'},
  {Command='PostCropVignetteStyleColorPriority',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawController/PostCropVignetteStyle/Exposure2=Color Priority'),Group=effects,Explanation='Post Crop Vignette Style: Color Priority.',Panel='effectsPanel'},
  {Command='PostCropVignetteStylePaintOverlay',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawController/PostCropVignetteStyle/PaintOverlay=Paint Overlay'),Group=effects,Explanation='Post Crop Vignette Style: Paint Overlay.',Panel='effectsPanel'},
  {Command='PostCropVignetteHighlightContrast',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteHighlightContrast=Post Crop Vignette Highlight Contrast'),Group=effects,Explanation='(Highlight Priority and Color Priority only) Controls the degree of highlight contrast preserved when Amount is negative. Suitable for photos with small highlights, such as candles and lamps.',Panel='effectsPanel'},
  {Command='GrainAmount',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainAmount=Grain Amount'),Group=effects,Explanation='Controls the amount of grain applied to the image. Drag to the right to increase the amount. Set to zero to disable grain.',Panel='effectsPanel'},
  {Command='GrainSize',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainSize=Grain Size'),Group=effects,Explanation='Controls grain particle size. At sizes of 25 or greater, blue is added to make the effect look better with noise reduction.',Panel='effectsPanel'},
  {Command='GrainFrequency',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainRoughness=Grain Roughness'),Group=effects,Explanation=' Controls the regularity of the grain. Drag to the left to make the grain more uniform; drag to the right to make the grain more uneven.',Panel='effectsPanel'},
  {Command='ResetPostCropVignetteAmount',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteAmount=Post Crop Vignette Amount'),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetPostCropVignetteMidpoint',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteMidpoint=Post Crop Vignette Midpoint'),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetPostCropVignetteFeather',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteFeather=Post Crop Vignette Feather'),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetPostCropVignetteRoundness',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteRoundness=Post Crop Vignette Roundness'),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetPostCropVignetteStyle',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteStyle=Post Crop Vignette Style'),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetPostCropVignetteHighlightContrast',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteHighlightContrast=Post Crop Vignette Highlight Contrast'),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetGrainAmount',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainAmount=Grain Amount'),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetGrainSize',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainSize=Grain Size'),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
  {Command='ResetGrainFrequency',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainRoughness=Grain Roughness'),Group=effects,Explanation='Reset to default.',Panel='effectsPanel'},
--
--develop: calibration panel
--
  {Command='RevealPanelCalibrate',Type='button',Translation=show..' '..calibration,Group=calibration,Explanation='Open Camera Calibration Panel in Develop Module.'},
  {Command='EnableCalibration',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableCalibration=Enable Calibration'),Group=calibration,Explanation='Enable or disable custom camera calibration.',Panel='calibratePanel'},
  {Command='Profile_Adobe_Standard',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Standard=Standard'),Group=calibration,Explanation='Applies the Adobe Standard profile.',Panel='calibratePanel'},
  {Command='Profile_Camera_Bold',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Bold=Camera Bold'),Group=calibration,Explanation=profexp,Panel='calibratePanel'},
  {Command='Profile_Camera_Clear',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Clear=Camera Clear'),Group=calibration,Explanation=profexp1,Panel='calibratePanel'},
  {Command='Profile_Camera_Color',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Color=Camera Color'),Group=calibration,Explanation=profexp,Panel='calibratePanel'},
  {Command='Profile_Camera_Darker_Skin_Tone',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/DarkerSkinTone=Camera Darker Skin Tone'),Group=calibration,Explanation=profexp, 'calibratePanel'},
  {Command='Profile_Camera_Deep',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Deep=Camera Deep'),Group=calibration,Explanation=profexp1,Panel='calibratePanel'},
  {Command='Profile_Camera_Faithful',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Faithful=Camera Faithful'),Group=calibration,Explanation=profexp, 'calibratePanel'},
  {Command='Profile_Camera_Flat',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Flat=Camera Flat'),Group=calibration,Explanation=profexp, 'calibratePanel'},
  {Command='Profile_Camera_Landscape',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Landscape=Camera Landscape'),Group=calibration,Explanation=profexp1,Panel='calibratePanel'},
  {Command='Profile_Camera_Light',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Light=Camera Light'),Group=calibration,Explanation=profexp1,Panel='calibratePanel'},
  {Command='Profile_Camera_Lighter_Skin_Tone',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/LighterSkinTone=Camera Lighter Skin Tone'),Group=calibration,Explanation=profexp, 'calibratePanel'},
  {Command='Profile_Camera_LMonochrome',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/LMonochrome=Camera L-Monochrome'),Group=calibration,Explanation=profexp, 'calibratePanel'},
  {Command='Profile_Camera_Monochrome',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Monochrome=Camera Monochrome'),Group=calibration,Explanation=profexp, 'calibratePanel'},
  {Command='Profile_Camera_Monotone',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Monotone=Camera Monotone'),Group=calibration,Explanation=profexp, 'calibratePanel'},
  {Command='Profile_Camera_Muted',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Muted=Camera Muted'),Group=calibration,Explanation=profexp, 'calibratePanel'},
  {Command='Profile_Camera_Natural',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Natural=Camera Natural'),Group=calibration,Explanation=profexp, 'calibratePanel'},
  {Command='Profile_Camera_Neutral',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Neutral=Camera Neutral'),Group=calibration,Explanation=profexp1,Panel='calibratePanel'},
  {Command='Profile_Camera_Portrait',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Portrait=Camera Portrait'),Group=calibration,Explanation=profexp1,Panel='calibratePanel'},
  {Command='Profile_Camera_Positive_Film',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/PositiveFilm=Camera Positive Film'),Group=calibration,Explanation=profexp, 'calibratePanel'},
  {Command='Profile_Camera_Scenery',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Scenery=Camera Scenery'),Group=calibration,Explanation=profexp, 'calibratePanel'},
  {Command='Profile_Camera_Standard',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Standard=Camera Standard'),Group=calibration,Explanation=profexp1,Panel='calibratePanel'},
  {Command='Profile_Camera_Vibrant',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Vibrant=Camera Vibrant'),Group=calibration,Explanation=profexp,Panel='calibratePanel'},  
  {Command='Profile_Camera_Vivid',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/Vivid=Camera Vivid'),Group=calibration,Explanation=profexp1,Panel='calibratePanel'},
  {Command='Profile_Camera_Vivid_Blue',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/VividBlue=Camera Vivid Blue'),Group=calibration,Explanation=profexp, 'calibratePanel'},
  {Command='Profile_Camera_Vivid_Green',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/VividGreen=Camera Vivid Green'),Group=calibration,Explanation=profexp, 'calibratePanel'},
  {Command='Profile_Camera_Vivid_Red',Type='button',Experimental=true,Translation=LOC('$$$/CRaw/Style/Profile/Postfix/VividRed=Camera Vivid Red'),Group=calibration,Explanation=profexp, 'calibratePanel'},  
  {Command='ShadowTint',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ShadowTintCalibration=Shadow Tint Calibration'),Group=calibration,Explanation='Corrects for any green or magenta tint in the shadow areas of the photo.',Panel='calibratePanel'},
  {Command='RedHue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedHueCalibration=Red Hue Calibration'),Group=calibration,Explanation='For the red primary. Moving the Hue slider to the left (negative value) is similar to a counterclockwise move on the color wheel; moving it to the right (positive value) is similar to a clockwise move.',Panel='calibratePanel'},
  {Command='RedSaturation',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedSaturationCalibration=Red Saturation Calibration'),Group=calibration,Explanation='For the red primary. Moving the Saturation slider to the left (negative value) desaturates the color; moving it to the right (positive value) increases saturation.',Panel='calibratePanel'},
  {Command='GreenHue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenHueCalibration=Green Hue Calibration'),Group=calibration,Explanation='For the green primary. Moving the Hue slider to the left (negative value) is similar to a counterclockwise move on the color wheel; moving it to the right (positive value) is similar to a clockwise move.',Panel='calibratePanel'},
  {Command='GreenSaturation',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenSaturationCalibration=Green Saturation Calibration'),Group=calibration,Explanation='For the green primary. Moving the Saturation slider to the left (negative value) desaturates the color; moving it to the right (positive value) increases saturation.',Panel='calibratePanel'},
  {Command='BlueHue',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueHueCalibration=Blue Hue Calibration'),Group=calibration,Explanation='For the blue primary. Moving the Hue slider to the left (negative value) is similar to a counterclockwise move on the color wheel; moving it to the right (positive value) is similar to a clockwise move.',Panel='calibratePanel'},
  {Command='BlueSaturation',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueSaturationCalibration=Blue Saturation Calibration'),Group=calibration,Explanation='For the blue primary. Moving the Saturation slider to the left (negative value) desaturates the color; moving it to the right (positive value) increases saturation.',Panel='calibratePanel'},
  {Command='ResetShadowTint',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ShadowTintCalibration=Shadow Tint Calibration'),Group=calibration,Explanation='Reset to default.',Panel='calibratePanel'},
  {Command='ResetRedHue',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedHueCalibration=Red Hue Calibration'),Group=calibration,Explanation='Reset to default.',Panel='calibratePanel'},
  {Command='ResetRedSaturation',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedSaturationCalibration=Red Saturation Calibration'),Group=calibration,Explanation='Reset to default.',Panel='calibratePanel'},
  {Command='ResetGreenHue',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenHueCalibration=Green Hue Calibration'),Group=calibration,Explanation='Reset to default.',Panel='calibratePanel'},
  {Command='ResetGreenSaturation',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenSaturationCalibration=Green Saturation Calibration'),Group=calibration,Explanation='Reset to default.',Panel='calibratePanel'},
  {Command='ResetBlueHue',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueHueCalibration=Blue Hue Calibration'),Group=calibration,Explanation='Reset to default.',Panel='calibratePanel'},
  {Command='ResetBlueSaturation',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueSaturationCalibration=Blue Saturation Calibration'),Group=calibration,Explanation='Reset to default.',Panel='calibratePanel'},
--
--develop: develop presets
--
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

--
--develop: localized adjustments
--
  {Command='GraduatedFilter',Type='button',Translation=show..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/GraduatedFilters=Graduated Filters'),Group=localizedAdjustments,Explanation='Select Graduated Filter mode in Develop Module. Repeated press toggles Loupe View.'},
  {Command='RadialFilter',Type='button',Translation=show..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/RadialFilters=Radial Filters'),Group=localizedAdjustments,Explanation='Select Radial Filter View mode in Develop Module. Repeated press toggles Loupe View.'},
  {Command='RedEye',Type='button',Translation=show..' '..LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Redeye=Red-Eye Correction'),Group=localizedAdjustments,Explanation='Select Red Eye mode in Develop Module. Repeated press toggles Loupe View.'},
  {Command='SpotRemoval',Type='button',Translation=show..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/SpotRemoval=Spot Removal'),Group=localizedAdjustments,Explanation='Select Spot Removal mode in Develop Module. Repeated press toggles Loupe View.'},
  {Command='AdjustmentBrush',Type='button',Translation=show..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/BrushAdjustments=Brush Adjustments'),Group=localizedAdjustments,Explanation='Select Adjustment Brush mode in Develop Module. Repeated press toggles Loupe View.'},
  {Command='ShowMaskOverlay',Type='button',Translation=LOC('$$$/AgDevelop/LocalizedToolbar/ShowMaskOverlay=Show Mask Overlay'),Group=localizedAdjustments,Explanation='Show or hide the mask overlay.'},
  {Command='ToggleOverlay',type='button',Translation=locadj..' '..LOC('$$$/AgDevelop/Menu/View/AdjustmentMaskOverlay=Correction mask overlay')..' '..LOC('$$$/MIDI2LR/ShortCuts/ShowHide=show or hide'),Group=localizedAdjustments,Explanation='Toggles the localized adjustments mask overlay. Use only when any of the local adjustments filter is active. Supported in LR versions 7.4 and later.'},
  {Command='CycleMaskOverlayColor',Type='button',Translation=LOC('$$$/AgDevelop/Menu/View/AdjustmentBrushOverlay/CycleOverlay=Overlay Color'):gsub('&',''),Group=localizedAdjustments,Explanation='Change Mask Overlay Color.'},
  {Command='EnableCircularGradientBasedCorrections',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableRadialFilter=Enable Radial Filter'),Group=localizedAdjustments,Explanation='Enable or disable radial filter.'},
  {Command='EnableGradientBasedCorrections',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableGraduatedFilter=Enable Graduated Filter'),Group=localizedAdjustments,Explanation='Enable or disable graduated filter.'},
  {Command='EnablePaintBasedCorrections',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableBrushAdjustments=Enable Brush Adjustments'),Group=localizedAdjustments,Explanation='Enable or disable brush adjustments.'},
  {Command='EnableRedEye',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableRedEye=Enable Red-Eye'),Group=localizedAdjustments,Explanation='Enable or disable red eye correction.'},
  {Command='EnableRetouch',Type='button',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableSpotRemoval=Enable Spot Removal'),Group=localizedAdjustments,Explanation='Enable or disable spot removal.'},
  {Command='ChangeBrushSize',Type='repeat',Translation=brush..' — '..size,Group=localizedAdjustments,Explanation='Change adjustment brush size. This works with spot, gradient, radial filter and localized adjustment tools. **Caution**: With gradient and radial filter, make sure *brush* is selected when using this command. Turning knob clockwise sends Increase Size signals to Lightroom, counterclockwise Decrease Size.'..repeatexp},
  {Command='BrushSizeSmaller',Type='button',Translation=brush..' — '..size..' — '..smaller,Group=localizedAdjustments,Explanation='Reduce adjustment brush size.  This works with spot, gradient, radial filter and localized adjustment tools. **Caution**: With gradient and radial filter, make sure *brush* is selected when using this command.'},
  {Command='BrushSizeLarger',Type='button',Translation=brush..' — '..size..' — '..larger,Group=localizedAdjustments,Explanation='Increase adjustment brush size.  This works with spot, gradient, radial filter and localized adjustment tools. **Caution**: With gradient and radial filter, make sure *brush* is selected when using this command.'},
  {Command='ChangeFeatherSize',Type='repeat',Translation=brush..' — '..feather,Group=localizedAdjustments,Explanation='Change adjustment brush feather size.  This works with spot, gradient, radial filter and localized adjustment tools. **Caution**: With gradient and radial filter, make sure *brush* is selected when using this command. Turning knob clockwise sends Increase Size signals to Lightroom, counterclockwise Decrease Size.'..repeatexp},
  {Command='BrushFeatherSmaller',Type='button',Translation=brush..' — '..feather..' — '..smaller,Group=localizedAdjustments,Explanation='Reduce adjustment brush feather. This works with spot, gradient, radial filter and localized adjustment tools. **Caution**: With gradient and radial filter, make sure *brush* is selected when using this command.'},
  {Command='BrushFeatherLarger',Type='button',Translation=brush..' — '..feather..' — '..larger,Group=localizedAdjustments,Explanation='Increase adjustment brush feather. This works with spot, gradient, radial filter and localized adjustment tools. **Caution**: With gradient and radial filter, make sure *brush* is selected when using this command.'},
  {Command='local_Temperature',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Temperature=Temp.')..' (PV2012)',Group=localizedAdjustments,Explanation='Adjust Temperature for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
  {Command='local_Tint',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Tint=Tint')..' (PV2012)',Group=localizedAdjustments,Explanation='Adjust Tint for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
  {Command='local_Exposure',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Exposure=Exposure')..' ('..PV2010and2012..')',Group=localizedAdjustments,Explanation='Adjust Exposure for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
  {Command='local_Contrast',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Contrast=Contrast')..' ('..PV2010and2012..')',Group=localizedAdjustments,Explanation='Adjust Contrast for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
  {Command='local_Highlights',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Highlights=Highlights')..' (PV2012)',Group=localizedAdjustments,Explanation='Adjust Highlights for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
  {Command='local_Shadows',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Shadows=Shadows')..' (PV2012)',Group=localizedAdjustments,Explanation='Adjust Shadows for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
  {Command='local_Whites2012',Type='parameter',Experimental=true,Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Whites=Whites')..' (PV2012)',Group=localizedAdjustments,Explanation='Adjust Whites for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
  {Command='local_Blacks2012',Type='parameter',Experimental=true,Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Blacks=Blacks')..' (PV2012)',Group=localizedAdjustments,Explanation='Adjust Blacks for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
  {Command='local_Clarity',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Clarity=Clarity')..' ('..PV2010and2012..')',Group=localizedAdjustments,Explanation='Adjust Clarity for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
  {Command='local_Dehaze',Type='parameter',Experimental=true,Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Dehaze=Dehaze')..' (PV2012)',Group=localizedAdjustments,Explanation='Adjust Dehaze for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
  {Command='local_Saturation',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Saturation=Saturation')..' ('..PV2010and2012..')',Group=localizedAdjustments,Explanation='Adjust Saturation for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
  {Command='local_Sharpness',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Sharpness=Sharpness')..' ('..PV2010and2012..')',Group=localizedAdjustments,Explanation='Adjust Sharpness for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
  {Command='local_LuminanceNoise',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/LuminanceNoiseReduction=Luminence Noise Reduction')..' (PV2012)',Group=localizedAdjustments,Explanation='Adjust Luminance Noise for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
  {Command='local_Moire',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/MoireReduction=Moire')..' (PV2012)',Group=localizedAdjustments,Explanation='Adjust Moire for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
  {Command='local_Defringe',Type='parameter',Translation=locadj..' '..LOC('$$$/AgDevelop/Localized/Defringe=Defringe')..' (PV2012)',Group=localizedAdjustments,Explanation='Adjust Defringe for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
  {Command='local_ToningLuminance',Type='parameter',Translation=locadj..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Luminance=Luminance')..' (PV2010)',Group=localizedAdjustments,Explanation='Adjust Toning Luminance for the currently active tool: Brush, Radial Filter, or Graduated Filter.'},
--local adjustment resets 
  {Command='ResetCircGrad',Type='button',Translation=LOC('$$$/AgLibrary/Ops/ResetRadialFilters=Reset Radial Filters'),Group=localadjresets,Explanation='Delete radial filter.'},
  {Command='ResetGradient',Type='button',Translation=LOC('$$$/AgLibrary/Ops/ResetGraduatedFilters=Reset Graduated Filters'),Group=localadjresets,Explanation='Delete graduated filter.'},
  {Command='ResetBrushing',Type='button',Translation=LOC('$$$/AgLibrary/Ops/ResetBrushing=Reset Brush Corrections'),Group=localadjresets,Explanation='Delete brush adjustments.'},
  {Command='ResetRedeye',Type='button',Translation=LOC('$$$/AgLibrary/Ops/ResetRedeye=Reset Red-Eye'),Group=localadjresets,Explanation='Delete red eye correction.'},
  {Command='ResetSpotRem',Type='button',Translation=LOC('$$$/AgLibrary/Ops/ResetSpotRemoval=Reset Spot Removal'),Group=localadjresets,Explanation='Delete spot removal.'},
  {Command='Resetlocal_Temperature',Type='button',Translation=reset..' '..locadj..' '..LOC('$$$/AgDevelop/Localized/Temperature=Temp.')..' (PV2012)',Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Tint',Type='button',Translation=reset..' '..locadj..' '..LOC('$$$/AgDevelop/Localized/Tint=Tint')..' (PV2012)',Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Exposure',Type='button',Translation=reset..' '..locadj..' '..LOC('$$$/AgDevelop/Localized/Exposure=Exposure')..' ('..PV2010and2012..')',Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Contrast',Type='button',Translation=reset..' '..locadj..' '..LOC('$$$/AgDevelop/Localized/Contrast=Contrast')..' ('..PV2010and2012..')',Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Highlights',Type='button',Translation=reset..' '..locadj..' '..LOC('$$$/AgDevelop/Localized/Highlights=Highlights')..' (PV2012)',Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Shadows',Type='button',Translation=reset..' '..locadj..' '..LOC('$$$/AgDevelop/Localized/Shadows=Shadows')..' (PV2012)',Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Whites2012',Type='button',Experimental=true,Translation=reset..' '..locadj..' '..LOC('$$$/AgDevelop/Localized/Whites=Whites')..' (PV2012)',Group=localadjresets,Explanation='Reset to default.'}, 
  {Command='Resetlocal_Blacks2012',Type='button',Experimental=true,Translation=reset..' '..locadj..' '..LOC('$$$/AgDevelop/Localized/Blacks=Blacks')..' (PV2012)',Group=localadjresets,Explanation='Reset to default.'}, 
  {Command='Resetlocal_Clarity',Type='button',Translation=reset..' '..locadj..' '..LOC('$$$/AgDevelop/Localized/Clarity=Clarity')..' ('..PV2010and2012..')',Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Dehaze',Type='button',Experimental=true,Translation=reset..' '..locadj..' '..LOC('$$$/AgDevelop/Localized/Dehaze=Dehaze')..' (PV2012)',Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Saturation',Type='button',Translation=reset..' '..locadj..' '..LOC('$$$/AgDevelop/Localized/Saturation=Saturation')..' ('..PV2010and2012..')',Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Sharpness',Type='button',Translation=reset..' '..locadj..' '..LOC('$$$/AgDevelop/Localized/Sharpness=Sharpness')..' ('..PV2010and2012..')',Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_LuminanceNoise',Type='button',Translation=reset..' '..locadj..' '..LOC('$$$/AgDevelop/Localized/LuminanceNoiseReduction=Luminence Noise Reduction')..' (PV2012)',Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Moire',Type='button',Translation=reset..' '..locadj..' '..LOC('$$$/AgDevelop/Localized/MoireReduction=Moire')..' (PV2012)',Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_Defringe',Type='button',Translation=reset..' '..locadj..' '..LOC('$$$/AgDevelop/Localized/Defringe=Defringe')..' (PV2012)',Group=localadjresets,Explanation='Reset to default.'},
  {Command='Resetlocal_ToningLuminance',Type='button',Translation=reset..' '..locadj..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/Luminance=Luminanz')..' (PV2010)',Group=localadjresets,Explanation='Reset to default.'}, 
--local adjustment presets
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

--
-- Misc
--

  {Command='straightenAngle',Type='parameter',Translation=LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/StraightenAngle=Straighten Angle'),Group=crop,Explanation='Rotate crop angle. Moves angle in crop tool panel from -45 to 45.'},
  {Command='CropAngle',Type='parameter',Experimental=true,Translation=LOC('$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CropAngle=Crop Angle'),Group=crop,Explanation='Use *Straighten Angle* (above) instead. This control remains listed as a legacy item. Rotate crop rectangle. This control is constrained to picture boundaries even when \226\128\156constrain to image\226\128\157 is not selected. It also causes the aspect ratio of the crop to change.'},
  {Command='CropBottom',Type='parameter',Experimental=true,Translation=crop..' - '.. LOC('$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Bottom=Bottom'),Group=crop,Explanation='Adjust bottom of crop rectangle.'},
  {Command='CropLeft',Type='parameter',Experimental=true,Translation=crop..' - '..LOC('$$$/AgWatermarking/Alignment/Left=Left'),Group=crop,Explanation='Adjust left side of crop rectangle.'},
  {Command='CropRight',Type='parameter',Experimental=true,Translation=crop..' - '..LOC('$$$/AgWatermarking/Alignment/Right=Right'),Group=crop,Explanation='Adjust right side of crop rectangle.'},
  {Command='CropTop',Type='parameter',Experimental=true,Translation=crop..' - '..LOC('$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Top=Top'),Group=crop,Explanation='Adjust top of crop rectangle.'},
  {Command='ResetCrop',Type='button',Translation=LOC('$$$/AgLibrary/Ops/ResetCrop=Reset Crop'),Group=crop,Explanation='Reset the crop angle and frame for the current photo.'},
  {Command='ResetstraightenAngle',Type='button',Translation=reset..' '..LOC('$$$/AgCameraRawNamedSettings/SaveNamedDialog/StraightenAngle=Straighten Angle'),Group=crop,Explanation='Reset crop angle.'},
  {Command='CropOverlay',Type='button',Translation=show..' '..crop,Group=crop,Explanation='Select Crop Overlay mode in Develop Module. Repeated press toggles Loupe View.'},
  {Command='Loupe',Type='button',Translation=show..' '..LOC('$$$/AgPhotoBin/ViewMode/Develop/Loupe=Loupe'),Group=gotoToolModulePanel,Explanation='Select Loupe View mode in Develop Module. Repeated press toggles in and out of Loupe View.'},
  {Command='SwToMmap',Type='button',Translation=show..' '..LOC('$$$/AgLocation/ModuleTitle=Map'),Group=gotoToolModulePanel,Explanation='Switch to Map module.'},
  {Command='SwToMbook',Type='button',Translation=show..' '..LOC('$$$/Ag/Layout/Book/ModuleTitle=Book'),Group=gotoToolModulePanel,Explanation='Switch to Book module.'},
  {Command='SwToMslideshow',Type='button',Translation=show..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/Slideshow=Slideshow'),Group=gotoToolModulePanel,Explanation='Switch to Slideshow module.'},
  {Command='SwToMprint',Type='button',Translation=show..' '..LOC('$$$/AgPrint/Menu/File/Print=Print'),Group=gotoToolModulePanel,Explanation='Switch to Print module.'},
  {Command='SwToMweb',Type='button',Translation=show..' '..LOC('$$$/WPG/Help/Shortcuts/WebHeader=Web'),Group=gotoToolModulePanel,Explanation='Switch to Web module.'},
  {Command='ShoScndVwloupe',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/Loupe=Loupe'),Group=secondaryDisplay,Explanation='Shows Loupe view on the secondary screen, or hides the secondary screen if Loupe view was previously being shown.'},
  {Command='ShoScndVwlive_loupe',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/LiveLoupe=Live Loupe'),Group=secondaryDisplay,Explanation='Shows Live Loupe view on the secondary screen, or hides the secondary screen if Live Loupe view was previously being shown.'},
  {Command='ShoScndVwlocked_loupe',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/LockedLoupe=Locked Loupe'),Group=secondaryDisplay,Explanation='Shows Locked Loupe view on the secondary screen, or hides the secondary screen if Locked Loupe view was previously being shown.'},
  {Command='ShoScndVwgrid',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/Grid=Grid'),Group=secondaryDisplay,Explanation='Shows Grid view on the secondary screen, or hides the secondary screen if Grid view was previously being shown.'},
  {Command='ShoScndVwcompare',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/Compare=Compare'),Group=secondaryDisplay,Explanation='Shows Compare view on the secondary screen, or hides the secondary screen if Compare view was previously being shown.'},
  {Command='ShoScndVwsurvey',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/Survey=Survey'),Group=secondaryDisplay,Explanation='Shows Survey view on the secondary screen, or hides the secondary screen if Survey view was previously being shown.'},
  {Command='ShoScndVwslideshow',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/Slideshow=Slideshow'),Group=secondaryDisplay,Explanation='Shows Slideshow view on the secondary screen, or hides the secondary screen if Slideshow view was previously being shown.'},
  {Command='ToggleScreenTwo',Type='button',Translation=secondaryDisplay..' '..LOC('$$$/AgApplication/Menu/Window/SecondMonitor/Show=Show'),Group=secondaryDisplay,Explanation='Toggles the the secondary window on/off.'},
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
  {Command='PrevPro',Type='button',Translation=LOC('$$$/MIDI2LR/Profiles/Previous=Previous Profile'),Group=profiles,Explanation='Change to previous (by alpha order) MIDI mapping profile.'},
  {Command='NextPro',Type='button',Translation=LOC('$$$/MIDI2LR/Profiles/Next=Next Profile'),Group=profiles,Explanation='Change to next (by alpha order) MIDI mapping profile.'},
  {Command='FullRefresh',Type='button',Translation=LOC('$$$/AgLibrary/ViewBar/Sort/RefreshMode/Manual=Manual Update'),Group=profiles,Explanation='Force an update of all develop settings in MIDI controller, even if MIDI2LR believes MIDI controller is up-to-date. Useful if controller out of sync with Lightroom (e.g., with layer changes).'},
}

local CmdTrans={}
local CmdPanel={}
local Parameters={}
local ValidActions={}
for _,v in ipairs(DataBase) do
  CmdTrans[v.Command] = v.Translation
  if v.Panel then
    CmdPanel[v.Command] = v.Panel
  end
  if v.Type == 'parameter' then
    Parameters[v.Command] = true
  elseif v.Type == 'button' then
    ValidActions[v.Command] = true
  end 
end

local LrPathUtils = import 'LrPathUtils'
local Ut = require 'Utilities'

local AppTrans = LrPathUtils.child(Ut.appdatapath() , 'MenuTrans.xml') 
local function WriteAppTrans(language) 
  local file = assert(io.open(AppTrans,'w'),'Error writing to MenuTrans.txt') 
--new version for xml file  
  local CmdStructure={}
  local GroupOrder={}
  for _,v in ipairs(DataBase) do
    if CmdStructure[v.Group] then
      table.insert(CmdStructure[v.Group],{v.Command,v.Translation})
    else
      GroupOrder[#GroupOrder+1]=v.Group
      CmdStructure[v.Group]={}
      table.insert(CmdStructure[v.Group],{v.Command,v.Translation})
    end
  end
  file:write([=[
<?xml version="1.0" encoding="utf-8"?>
<cereal>
  <value0>
    <cereal_class_version>1</cereal_class_version>
    <language>]=],language,[=[</language>
    <all_commands size="dynamic">
]=])
  for i,v in ipairs(GroupOrder) do
    file:write('      <value'.. i-1 ..'>\n        <first>'..v..'</first>\n'..
      '        <second size="dynamic">\n')
    for j,u in ipairs(CmdStructure[v]) do
      file:write('            <value'.. j-1 ..'>\n'..
        '              <first>'.. u[1] ..'</first>\n'..
        '              <second>'.. u[2] ..'</second>\n'..
        '            </value'.. j-1 ..'>\n')
    end
    file:write('        </second>\n      </value'.. i-1 ..'>\n')
  end
  file:write([=[
    </all_commands>
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
  Parameters = Parameters,
  ValidActions = ValidActions,
  WriteAppTrans = WriteAppTrans,
}

