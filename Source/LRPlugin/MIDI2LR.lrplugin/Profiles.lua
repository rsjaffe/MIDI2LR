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
  loupe = {ToolModulePanel = 'Tool', presetFile = '',},
  crop = {ToolModulePanel = 'Tool', presetFile = '',},
  dust = {ToolModulePanel = 'Tool', presetFile = '',},
  redeye = {ToolModulePanel = 'Tool', presetFile = '',},
  gradient = {ToolModulePanel = ''Tool, presetFile = '',},
  circularGradient = {ToolModulePanel = 'Tool', presetFile = '',},
  localized = {ToolModulePanel = 'Tool', presetFile = '',},
  tonePanel = {ToolModulePanel = 'Panel', presetFile = '',},
  mixerPanel = {ToolModulePanel = 'Panel', presetFile = '',},
  splitToningPanel = {ToolModulePanel = 'Panel', presetFile = '',},
  detailPanel = {ToolModulePanel = 'Panel', presetFile = '',},
  lensCorrectionsPanel = {ToolModulePanel = 'Panel', presetFile = '',},
  effectsPanel = {ToolModulePanel = 'Panel', presetFile = '',},
  calibratePanel = {ToolModulePanel = 'Panel', presetFile = '',},
  library = {ToolModulePanel = 'Module', presetFile = '',},
  develop = {ToolModulePanel = 'Module', presetFile = '',},
  map = {ToolModulePanel = 'Module', presetFile = '',},
  book = {ToolModulePanel = 'Module', presetFile = '',},
  slideshow = {ToolModulePanel = 'Module', presetFile = '',},
  print = {ToolModulePanel = 'Module', presetFile = '',},
  web = {ToolModulePanel = 'Module', presetFile = '',},
  profile1 = {ToolModulePanel = '', presetFile = '',},
  profile2 = {ToolModulePanel = '', presetFile = '',},
  profile3 = {ToolModulePanel = '', presetFile = '',},
  profile4 = {ToolModulePanel = '', presetFile = '',},
  profile5 = {ToolModulePanel = '', presetFile = '',},
  profile6 = {ToolModulePanel = '', presetFile = '',},
  profile7 = {ToolModulePanel = '', presetFile = '',},
  profile8 = {ToolModulePanel = '', presetFile = '',},
  profile9 = {ToolModulePanel = '', presetFile = '',},
  profile10 = {ToolModulePanel = '', presetFile = '',},
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