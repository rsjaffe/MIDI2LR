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
local LrFileUtils         = import 'LrFileUtils'
local LrView              = import 'LrView'

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
local loadedprofile = ''-- according to application and us
local profilepath = '' --according to application

local function receiveAppMessage(path,filename)
  loadedprofile = filename
  profilepath = path
end


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
    if (newprofile_file ~= nil) and (newprofile_file ~= '') and 
    ((ignoreCurrent == true) or (newToolModulePanel == '') or (current[newToolModulePanel] ~= profilename)) then
      MIDI2LR.SERVER:send('SwitchProfile '..newprofile_file..'\n')
      loadedprofile = newprofile_file
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
  local completion = {}
  local auto_completion = false
  if profilepath and profilepath ~= '' then
    auto_completion = true
    for filePath in LrFileUtils.files(profilepath) do
      local _, fn, ext = filePath.match("(.-)([^\\/]-%.?([^%.\\/]*))$")
      if ext == 'xml' then
        table.insert(completion,fn)
      end
    end
  end
  local allboxes =
  f:column{
    spacing = f:control_spacing(),
    f:row {
      f:column {
        width = LrView.share('profile_column'),
        f:group_box {
          title = LOC("$$$/AgDevelop/Menu/Tools=Tools"):gsub('&',''), --string has & in it in LR database
          width = LrView.share('profile_group'),
          f:row {
            f:static_text{title = ProfileTypes.loupe.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileloupe'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            f:static_text{title = ProfileTypes.crop.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profilecrop'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },    
          f:row {
            f:static_text{title = ProfileTypes.dust.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profiledust'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },  
          f:row {
            f:static_text{title = ProfileTypes.redeye.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileredeye'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },  
          f:row {
            f:static_text{title = ProfileTypes.gradient.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profilegradient'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.circularGradient.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfilecircularGradient'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },   
          f:row {
            f:static_text{title = ProfileTypes.localized.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profilelocalized'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },  
        },
        f:group_box {
          title = LOC("$$$/Application/Menu/Window/Modules=Modules:"):gsub(':',''), --string has : in it in LR database
          width = LrView.share('profile_group'),
          f:row {
            f:static_text{title = ProfileTypes.library.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profilelibrary'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.develop.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profiledevelop'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.map.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profilemap'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.book.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profilesbook'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },  
          f:row {
            f:static_text{title = ProfileTypes.slideshow.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileslideshow'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.print.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprint'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.web.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileweb'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
        },
      },
      f:column {
        width = LrView.share('profile_column'),
        f:group_box {
          title = LOC("$$$/AgPreferences/Interface/GroupTitle/Panels=Panels"),
          width = LrView.share('profile_group'),
          f:row {
            f:static_text{title = ProfileTypes.adjustPanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfileadjustPanel'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            f:static_text{title = ProfileTypes.tonePanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfiletonePanel'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.mixerPanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfilemixerPanel'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.splitToningPanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfilesplitToningPanel'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },  
          f:row {
            f:static_text{title = ProfileTypes.detailPanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfiledetailPanel'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.lensCorrectionsPanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfilelensCorrectionsPanel'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.effectsPanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfileeffectsPanel'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.calibratePanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfilecalibratePanel'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },  
        },
      },
      f:column {
        width = LrView.share('profile_column'),
        f:group_box {
          width = LrView.share('profile_group'),
          f:row {
            f:static_text{title = ProfileTypes.profile1.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile1'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.profile2.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile2'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.profile3.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile3'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.profile4.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile4'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.profile5.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile5'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.profile6.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile6'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.profile7.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile7'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.profile8.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile8'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.profile9.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile9'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
          f:row {
            f:static_text{title = ProfileTypes.profile10.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile10'), width = LrView.share('profile_value'), 
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          }, 
        },
      },
    },
    f:spacer{height = f:control_spacing() * 2,},
    f:push_button {title = LOC("$$$/AgDevelop/PresetsPanel/ClearAll=Clear All"), action = function()
        for k in obstable:pairs() do
          if k:find('Profile') == 1 then
            obstable[k] = ''
          end
        end
      end
    },
  }
  return allboxes
end

local function EndDialog(obstable, status)
  if status == 'ok' then
    useDefaults() -- empty out prior settings
    for k in pairs(ProfileTypes) do
      if type(obstable['Profile'..k])=='string' then
        ProgramPreferences.Profiles[k] = obstable['Profile'..k]:gsub("^%s*(.-)%s*$", "%1")
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
  receiveAppMessage = receiveAppMessage,
}
