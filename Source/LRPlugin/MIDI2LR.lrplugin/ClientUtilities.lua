--[[----------------------------------------------------------------------------

ClientUtilities.lua

Procedures used by Client.lua. Moved to separate file as size of Client.lua
became unwieldy.
 
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
local Limits     = require 'Limits'
local Database   = require 'Database'
local Profiles   = require 'Profiles'
local Ut         = require 'Utilities'
local LrApplication       = import 'LrApplication'
local LrApplicationView   = import 'LrApplicationView'
local LrBinding           = import 'LrBinding'
local LrDevelopController = import 'LrDevelopController'
local LrDialogs           = import 'LrDialogs'
local LrFunctionContext   = import 'LrFunctionContext'
local LrStringUtils       = import 'LrStringUtils'
local LrTasks             = import 'LrTasks'
local LrView              = import 'LrView'


local function showBezel(param, value1, value2)
  local precisionList = { 
    Blacks=0,
    BlueHue=0,
    BlueSaturation=0,
    Brightness=0,
    Clarity=0,
    ColorNoiseReduction=0,
    ColorNoiseReductionDetail=0,
    ColorNoiseReductionSmoothness=0,
    Contrast=0,
    CropAngle=3,
    CropBottom=3,
    CropLeft=3,
    CropRight=3,
    CropTop=3,
    DefringeGreenAmount=0,
    DefringeGreenHueHi=0,
    DefringeGreenHueLo=0,
    DefringePurpleAmount=0,
    DefringePurpleHueHi=0,
    DefringePurpleHueLo=0,
    Dehaze=0,
    Exposure=2,
    GrainAmount=0,
    GrainFrequency=0,
    GrainSize=0,
    GreenHue=0,
    GreenSaturation=0,
    Highlights=0,
    HueAdjustmentAqua=0,
    HueAdjustmentBlue=0,
    HueAdjustmentGreen=0,
    HueAdjustmentMagenta=0,
    HueAdjustmentOrange=0,
    HueAdjustmentPurple=0,
    HueAdjustmentRed=0,
    HueAdjustmentYellow=0,
    LensManualDistortionAmount=0,
    LensProfileChromaticAberrationScale=0,
    LensProfileDistortionScale=0,
    LensProfileVignettingScale=0,
    LuminanceAdjustmentAqua=0,
    LuminanceAdjustmentBlue=0,
    LuminanceAdjustmentGreen=0,
    LuminanceAdjustmentMagenta=0,
    LuminanceAdjustmentOrange=0,
    LuminanceAdjustmentPurple=0,
    LuminanceAdjustmentRed=0,
    LuminanceAdjustmentYellow=0,
    LuminanceNoiseReductionContrast=0,
    LuminanceNoiseReductionDetail=0,
    LuminanceSmoothing=0,
    ParametricDarks=0,
    ParametricHighlightSplit=0,
    ParametricHighlights=0,
    ParametricLights=0,
    ParametricMidtoneSplit=0,
    ParametricShadowSplit=0,
    ParametricShadows=0,
    PerspectiveAspect=0,
    PerspectiveHorizontal=0,
    PerspectiveRotate=1,
    PerspectiveScale=0,
    PerspectiveVertical=0,
    PerspectiveX=1,
    PerspectiveY=1,
    PostCropVignetteAmount=0,
    PostCropVignetteFeather=0,
    PostCropVignetteHighlightContrast=0,
    PostCropVignetteMidpoint=0,
    PostCropVignetteRoundness=0,
    RedHue=0,
    RedSaturation=0,
    Saturation=0,
    SaturationAdjustmentAqua=0,
    SaturationAdjustmentBlue=0,
    SaturationAdjustmentGreen=0,
    SaturationAdjustmentMagenta=0,
    SaturationAdjustmentOrange=0,
    SaturationAdjustmentPurple=0,
    SaturationAdjustmentRed=0,
    SaturationAdjustmentYellow=0,
    ShadowTint=0,
    Shadows=0,
    SharpenDetail=0,
    SharpenEdgeMasking=0,
    SharpenRadius=1,
    SplitToningBalance=0,
    SplitToningHighlightHue=0,
    SplitToningHighlightSaturation=0,
    SplitToningShadowHue=0,
    SplitToningShadowSaturation=0,
    Temperature=0,
    Tint=0,
    Vibrance=0,
    VignetteAmount=0,
    VignetteMidpoint=0,
    Whites=0,
    local_Blacks2012=0,
    local_Clarity=0,
    local_Contrast=0,
    local_Defringe=0,
    local_Dehaze=0,
    local_Exposure=2,
    local_Highlights=0,
    local_LuminanceNoise=0,
    local_Moire=0,
    local_Saturation=0,
    local_Shadows=0,
    local_Sharpness=0,
    local_Temperature=0,
    local_Tint=0,
    local_Whites2012=0,
    straightenAngle=3,
  }
  local precision = precisionList[param] or 4
  local bezelname = Database.CmdTrans[param] or param
  if value2 then
    LrDialogs.showBezel(bezelname..'  '..LrStringUtils.numberToString(value1,precision) .. '  ' ..LrStringUtils.numberToString(value2,precision) )
  else
    LrDialogs.showBezel(bezelname..'  '..LrStringUtils.numberToString(value1,precision))
  end
end

local function fApplyFilter(filternumber)
  return function()
    local filterUuid = ProgramPreferences.Filters[filternumber]
    if filterUuid == nil then return end
    if LrApplication.activeCatalog():setViewFilter(filterUuid) then --true if filter changed
      local _,str = LrApplication.activeCatalog():getCurrentViewFilter()
      if str then LrDialogs.showBezel(str) end -- str nil if not defined
    end
  end
end

local function fApplyPreset(presetnumber)
  return function()
    local presetUuid = ProgramPreferences.Presets[presetnumber]
    if presetUuid == nil or LrApplication.activeCatalog():getTargetPhoto() == nil then return end
    local preset = LrApplication.developPresetByUuid(presetUuid)
    LrTasks.startAsyncTask ( function () 
        LrApplication.activeCatalog():withWriteAccessDo(
          'Apply preset '..preset:getName(), 
          function()     
            LrDialogs.showBezel(preset:getName())
            LrApplication.activeCatalog():getTargetPhoto():applyDevelopPreset(preset) 
          end,
          { timeout = 4, 
            callback = function() LrDialogs.showError(LOC("$$$/AgCustomMetadataRegistry/UpdateCatalog/Error=The catalog could not be updated with additional module metadata.").. 'PastePreset.') end, 
            asynchronous = true }
        ) 
      end )
  end
end

local function fChangePanel(panelname)
  return function()
    Ut.execFOM(LrDevelopController.revealPanel,panelname)
    Profiles.changeProfile(panelname) 
  end
end

local function fChangeModule(modulename)
  return function()
    LrApplicationView.switchToModule(modulename) 
    Profiles.changeProfile(modulename) 
  end
end

local function fToggle01(param)
  return function()
    if Ut.execFOM(LrDevelopController.getValue, param) == 0 then
      LrDevelopController.setValue(param,1)
    else
      LrDevelopController.setValue(param,0)
    end
  end
end

local function fToggle01Async(param)
  return function()
    LrTasks.startAsyncTask ( function ()
        if Ut.execFOM(LrDevelopController.getValue, param) == 0 then
          LrDevelopController.setValue(param,1)
        else
          LrDevelopController.setValue(param,0)
        end
      end )
  end
end

local function fToggle1ModN(param, N)
  return function()
    local v = Ut.execFOM(LrDevelopController.getValue, param)
    v = (v % N) + 1
    LrDevelopController.setValue(param,v)
  end
end

local function fToggleTF(param)
  return function()
    local v = Ut.execFOM(LrDevelopController.getValue, param)
    v = not v
    LrDevelopController.setValue(param,v)    
  end
end

local function fToggleTFasync(param)
  return function()
    LrTasks.startAsyncTask ( function ()
        if LrApplication.activeCatalog():getTargetPhoto() == nil then return end
        LrApplication.activeCatalog():withWriteAccessDo(
          'MIDI2LR: Apply settings',
          function()
            local params = LrApplication.activeCatalog():getTargetPhoto():getDevelopSettings()
            params[param] = not params[param]
            LrApplication.activeCatalog():getTargetPhoto():applyDevelopSettings(params) 
          end,
          { timeout = 4,
            callback = function()
              LrDialogs.showError(LOC("$$$/AgCustomMetadataRegistry/UpdateCatalog/Error=The catalog could not be updated with additional module metadata.")..' ApplySettings')
            end,
            asynchronous = true
          }
        )
      end
    )
  end
end

local function fToggleTool(param)
  return function()
    if LrApplicationView.getCurrentModuleName() ~= 'develop' then
      LrApplicationView.switchToModule('develop')
    end
    if(LrDevelopController.getSelectedTool() == param) then -- toggle between the tool/loupe
      LrDevelopController.selectTool('loupe')
      Profiles.changeProfile('loupe', true)
    else
      LrDevelopController.selectTool(param)
      Profiles.changeProfile(param, true)
    end
  end
end



local function PasteSettings  ()
  if MIDI2LR.Copied_Settings == nil or LrApplication.activeCatalog():getTargetPhoto() == nil then return end
  LrTasks.startAsyncTask ( function () 
      LrApplication.activeCatalog():withWriteAccessDo(
        'MIDI2LR: Paste settings', 
        function() LrApplication.activeCatalog():getTargetPhoto():applyDevelopSettings(MIDI2LR.Copied_Settings) end,
        { timeout = 4, 
          callback = function() 
            LrDialogs.showError(LOC("$$$/AgCustomMetadataRegistry/UpdateCatalog/Error=The catalog could not be updated with additional module metadata.")..' PasteSettings') 
          end, 
          asynchronous = true 
        }
      ) 
    end 
  )
end

local function ApplySettings(settings)
  if LrApplication.activeCatalog():getTargetPhoto() == nil then return end
  LrTasks.startAsyncTask ( function ()
      LrApplication.activeCatalog():withWriteAccessDo(
        'MIDI2LR: Apply settings',
        function() LrApplication.activeCatalog():getTargetPhoto():applyDevelopSettings(settings) end,
        { timeout = 4,
          callback = function()
            LrDialogs.showError(LOC("$$$/AgCustomMetadataRegistry/UpdateCatalog/Error=The catalog could not be updated with additional module metadata.")..' ApplySettings')
          end,
          asynchronous = true
        }
      )
    end
  )
end

local function CopySettings ()
  if LrApplication.activeCatalog():getTargetPhoto() == nil then return end
  LrTasks.startAsyncTask ( 
    function () 
      MIDI2LR.Copied_Settings = LrApplication.activeCatalog():getTargetPhoto():getDevelopSettings() 
    end
  ) 
end

local function FullRefresh()
  if Limits.LimitsCanBeSet() then
    for param in pairs(Database.Parameters) do
      local min,max = Limits.GetMinMax(param)
      local lrvalue = LrDevelopController.getValue(param)
      if type(min) == 'number' and type(max) == 'number' and type(lrvalue) == 'number' then
        local midivalue = (lrvalue-min)/(max-min)
        MIDI2LR.SERVER:send(string.format('%s %g\n', param, midivalue))
      end
      Profiles.resyncDeferred = false
    end
  else
    Profiles.resyncDeferred = true
  end
end

local function MIDIValueToLRValue(param, midi_value)
  -- must be called when in develop module with photo selected
  -- map midi range to develop parameter range
  -- expects midi_value 0.0-1.0, doesn't protect against out-of-range
  local min,max = Limits.GetMinMax(param)
  return midi_value * (max-min) + min
end

local function LRValueToMIDIValue(param)
  -- needs to be called in Develop module with photo selected
  -- map develop parameter range to midi range
  local min,max = Limits.GetMinMax(param)
  local retval = (LrDevelopController.getValue(param)-min)/(max-min)
  if retval > 1 then return 1 end
  if retval < 0 then return 0 end
  return retval
end

local function UpdateCameraProfile(name)
  return function()
    fChangePanel('calibratePanel')
    ApplySettings({
        CameraProfile = name
      })
  end
end

local function fSimulateKeys(keys, developonly, tool)
  return function()
    if developonly then
      if LrApplicationView.getCurrentModuleName() == 'develop' and LrApplication.activeCatalog():getTargetPhoto() ~= nil then
        if tool == nil or tool[LrDevelopController.getSelectedTool()] then
          MIDI2LR.SERVER:send(string.format('SendKey %s\n', keys))
        end
      end
    else
      MIDI2LR.SERVER:send(string.format('SendKey %s\n', keys))
    end
  end
end

local function SimulateKeys(keys, developonly, tool)
  if developonly then
    if LrApplicationView.getCurrentModuleName() == 'develop' and LrApplication.activeCatalog():getTargetPhoto() ~= nil then
      if tool == nil or tool == LrDevelopController.getSelectedTool() then
        MIDI2LR.SERVER:send(string.format('SendKey %s\n', keys))
      end
    end
  else
    MIDI2LR.SERVER:send(string.format('SendKey %s\n', keys))
  end 
end

local function UpdatePointCurve(settings)
  return function()
    fChangePanel('tonePanel')
    ApplySettings(settings)
  end
end

return {
  ApplySettings = ApplySettings,
  CopySettings = CopySettings,
  FullRefresh = FullRefresh,
  LRValueToMIDIValue = LRValueToMIDIValue,
  MIDIValueToLRValue = MIDIValueToLRValue,
  PasteSettings = PasteSettings,
  SimulateKeys = SimulateKeys,
  UpdateCameraProfile = UpdateCameraProfile,
  UpdatePointCurve = UpdatePointCurve,
  fApplyFilter = fApplyFilter,
  fApplyPreset = fApplyPreset,
  fChangeModule = fChangeModule,
  fChangePanel = fChangePanel,
  fSimulateKeys = fSimulateKeys,
  fToggle01 = fToggle01,
  fToggle01Async = fToggle01Async,
  fToggle1ModN = fToggle1ModN,
  fToggleTF = fToggleTF,
  fToggleTFasync = fToggleTFasync,
  fToggleTool = fToggleTool,
  showBezel = showBezel
}
