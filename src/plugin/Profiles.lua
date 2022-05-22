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

local Database            = require 'Database'
local Init                = require 'Init'
local Limits              = require 'Limits'
local ProfileTypes        = require 'ProfileTypes'
local LrApplication       = import 'LrApplication'
local LrApplicationView   = import 'LrApplicationView'
local LrDevelopController = import 'LrDevelopController'
local LrDialogs           = import 'LrDialogs'
local LrFileUtils         = import 'LrFileUtils'
local LrStringUtils       = import 'LrStringUtils'
local LrTasks             = import 'LrTasks'
local LrView              = import 'LrView'

local currentTMP = {Tool = '', Module = '', Panel = '', Profile = ''}
local loadedprofile = ''-- according to application and us
local profilepath = '' --according to application

local function doprofilechange(newprofile)
  if ProgramPreferences.ProfilesShowBezelOnChange then
    local filename = newprofile:match(".-([^\\^/]-([^%.]+))$")
    filename = filename:sub(0, -5)
    LrDialogs.showBezel(filename)
  end
  loadedprofile = newprofile
  if   (LrApplication.activeCatalog():getTargetPhoto() ~= nil) and
  (LrApplicationView.getCurrentModuleName() == 'develop') then
    -- refresh MIDI controller since mapping has changed
    LrTasks.startAsyncTask ( function ()
            --[[-----------debug section, enable by adding - to beginning this line
    LrMobdebug.on()
    --]]-----------end debug section
        local photoval = LrApplication.activeCatalog():getTargetPhoto():getDevelopSettings()
        -- refresh crop values
        local val_bottom = photoval.CropBottom
        MIDI2LR.SERVER:send(string.format('CropBottomRight %g\n', val_bottom))
        MIDI2LR.SERVER:send(string.format('CropBottomLeft %g\n', val_bottom))
        MIDI2LR.SERVER:send(string.format('CropAll %g\n', val_bottom))
        MIDI2LR.SERVER:send(string.format('CropBottom %g\n', val_bottom))
        local val_top = photoval.CropTop
        MIDI2LR.SERVER:send(string.format('CropTopRight %g\n', val_top))
        MIDI2LR.SERVER:send(string.format('CropTopLeft %g\n', val_top))
        MIDI2LR.SERVER:send(string.format('CropTop %g\n', val_top))
        local val_left = photoval.CropLeft
        local val_right = photoval.CropRight
        MIDI2LR.SERVER:send(string.format('CropLeft %g\n', val_left))
        MIDI2LR.SERVER:send(string.format('CropRight %g\n', val_right))
        local range_v = (1 - (val_bottom - val_top))
        if range_v == 0.0 then
          MIDI2LR.SERVER:send('CropMoveVertical 0\n')
        else
          MIDI2LR.SERVER:send(string.format('CropMoveVertical %g\n', val_top / range_v))
        end
        local range_h = (1 - (val_right - val_left))
        if range_h == 0.0 then
          MIDI2LR.SERVER:send('CropMoveHorizontal 0\n')
        else
          MIDI2LR.SERVER:send(string.format('CropMoveHorizontal %g\n', val_left / range_h))
        end
        for param,altparam in pairs(Database.Parameters) do
          local min,max = Limits.GetMinMax(param) --can't include ClientUtilities: circular reference
          local lrvalue
          if altparam == 'Direct' then
            lrvalue = LrDevelopController.getValue(param)
          else
            if param == altparam then
              lrvalue = (photoval[param] or 0)
            else
              lrvalue = (photoval[param] or 0) + (photoval[altparam] or 0)
            end
          end
          if type(min) == 'number' and type(max) == 'number' and type(lrvalue) == 'number' then
            local midivalue = (lrvalue-min)/(max-min)
            if midivalue >= 1.0 then
              MIDI2LR.SERVER:send(string.format('%s 1.0\n', param))
            elseif midivalue <= 0.0 then -- = catches -0.0 and sends it as 0.0
              MIDI2LR.SERVER:send(string.format('%s 0.0\n', param))
            else
              MIDI2LR.SERVER:send(string.format('%s %g\n', param, midivalue))
            end
          end
        end
      end
    )
  end
end

local function setDirectory(value)
  profilepath = value
end

local function setFile(value)
  if loadedprofile ~= value then
    doprofilechange(value)
  end
end

local function setFullPath(value)
  local path, profile = value:match("(.-)([^\\/]-%.?([^%.\\/]*))$")
  profilepath = path
  if profile ~= loadedprofile then
    doprofilechange(profile)
  end
end

local function changeProfile(profilename, ignoreCurrent)
  local changed = false
  if profilename and ProfileTypes[profilename] then
    local newprofile_file = ProgramPreferences.Profiles[profilename]
    local TMP = ProfileTypes[profilename]['TMP']
    if (newprofile_file ~= nil) and (newprofile_file ~= '') and (loadedprofile ~= newprofile_file) and
    ((ignoreCurrent == true) or (currentTMP[TMP] ~= profilename)) then
      MIDI2LR.SERVER:send('SwitchProfile '..newprofile_file..'\n')
      doprofilechange(newprofile_file)
    end
    currentTMP[TMP] = profilename
  end
  return changed
end

local function checkProfile()
  --as this runs 4X/second, doing check against currentTMP here to
  -- make it faster than always deferring to changeProfile
  local newmod = LrApplicationView.getCurrentModuleName()
  if newmod == 'develop' then
    local tool = LrDevelopController.getSelectedTool()
    if currentTMP.Module ~= newmod and
    ProgramPreferences.Profiles[tool] == '' then
      changeProfile(newmod)
    elseif currentTMP.Tool ~= tool then
      changeProfile(tool)
      currentTMP.Module = newmod  -- make sure that TMP.Module is
      -- set correctly when changing to tool profile
    end
  elseif currentTMP.Module ~= newmod then
    changeProfile(newmod)
    currentTMP.Tool = '' -- remove tool for modules~=develop
  end
end

local function StartDialog(obstable,f)
  for k in pairs(ProfileTypes) do
    obstable['Profile'..k] = ProgramPreferences.Profiles[k]
  end
  obstable.ProfilesShowBezelOnChange = ProgramPreferences.ProfilesShowBezelOnChange
  local completion = {}
  local auto_completion = false
  if profilepath and profilepath ~= '' then
    auto_completion = true
    for filePath in LrFileUtils.files(profilepath) do
      local _, fn, ext = filePath:match("(.-)([^\\/]-%.?([^%.\\/]*))$")
      if ext == 'xml' then
        completion[#completion+1] = fn
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
          title = LOC("$$$/Application/Menu/Window/Modules=Modules:"):gsub(' ?:',''), --string has : in it in LR database
          width = LrView.share('profile_group'),
          font='<system/small/bold>',
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.library.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profilelibrary'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.develop.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profiledevelop'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.map.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profilemap'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.book.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profilebook'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.slideshow.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileslideshow'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.print.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprint'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.web.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileweb'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
        },
        f:spacer { height=11 },
        f:group_box {
          title = LOC("$$$/AgDevelop/Menu/Tools=Tools"):gsub('&',''), --string has & in it in LR database
          width = LrView.share('profile_group'),
          font='<system/small/bold>',
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.loupe.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileloupe'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.crop.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profilecrop'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.dust.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profiledust'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.redeye.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileredeye'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.masking.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profilemasking'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:spacer{height = f:control_spacing() * 4,},
          f:push_button {title = LOC("$$$/AgDevelop/PresetsPanel/ClearAll=Clear All"), action = function()
              for k in obstable:pairs() do
                if k:find('Profile') == 1 then
                  obstable[k] = ''
                end
              end
            end
          },
          f:checkbox {title = LOC("$$$/MIDI2LR/Profiles/NotifyWhenChanged=Notify when profile changes"), value = LrView.bind('ProfilesShowBezelOnChange')}
        },
      },
      f:spacer { width = 2 },
      f:column {
        width = LrView.share('profile_column'),
        f:group_box {
          title = LOC("$$$/AgPreferences/Interface/GroupTitle/Panels=Panels"),
          width = LrView.share('profile_group'),
          size='regular', font='<system/small/bold>',
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.adjustPanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfileadjustPanel'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.tonePanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfiletonePanel'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.mixerPanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfilemixerPanel'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.colorGradingPanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfilecolorGradingPanel'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.detailPanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfiledetailPanel'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.lensCorrectionsPanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfilelensCorrectionsPanel'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.transformPanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfiletransformPanel'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.effectsPanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfileeffectsPanel'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.calibratePanel.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('ProfilecalibratePanel'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
        },
        f:spacer { height=11 },
        f:group_box {
          title = LOC("$$$/CRaw/Style/ProfileGroup/Profiles=Profiles"),
          width = LrView.share('profile_group'),
          size='regular', font='<system/small/bold>',
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile1.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile1'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile2.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile2'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile3.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile3'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile4.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile4'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile5.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile5'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile6.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile6'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile7.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile7'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile8.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile8'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
        },
      },
      f:spacer { width = 2 },
      f:column {
        width = LrView.share('profile_column'),
        f:group_box {
          title = LOC("$$$/CRaw/Style/ProfileGroup/Profiles=Profiles"),
          width = LrView.share('profile_group'),
          font='<system/small/bold>',
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile9.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile9'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile10.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile10'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile11.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile11'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile12.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile12'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile13.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile13'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile14.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile14'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile15.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile15'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile16.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile16'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile17.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile17'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile18.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile18'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile19.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile19'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile20.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile20'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile21.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile21'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile22.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile22'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile23.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile23'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile24.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile24'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile25.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile25'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
          f:row {
            font='<system>',
            f:static_text{title = ProfileTypes.profile26.friendlyName, width = LrView.share('profile_label'),},
            f:edit_field{ value = LrView.bind ('Profileprofile26'), width = LrView.share('profile_value'),
              width_in_chars = 15, auto_completion = auto_completion, completion = completion},
          },
        },
      },
    },

  }
  return allboxes
end

local function EndDialog(obstable, status)
  if status == 'ok' then
    Init.UseDefaultsProfiles() -- empty out prior settings
    for k in pairs(ProfileTypes) do
      if type(obstable['Profile'..k])=='string' then
        ProgramPreferences.Profiles[k] = LrStringUtils.trimWhitespace(obstable['Profile'..k])
      end
    end
    ProgramPreferences.ProfilesShowBezelOnChange = obstable.ProfilesShowBezelOnChange
  end
end

return {
  changeProfile = changeProfile,
  checkProfile = checkProfile,
  StartDialog = StartDialog,
  EndDialog = EndDialog,
  setDirectory = setDirectory,
  setFile = setFile,
  setFullPath = setFullPath,
}
