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
local LrDevelopController = import 'LrDevelopController'
local LrDialogs           = import 'LrDialogs'
local LrStringUtils       = import 'LrStringUtils'
local LrTasks             = import 'LrTasks'

--modules may be library develop map book slideshow print web
local needsModule = {
  [LrDevelopController.addAdjustmentChangeObserver]    = {module = 'develop', photoSelected = false },
  [LrDevelopController.decrement]                      = {module = 'develop', photoSelected = true },
  [LrDevelopController.getProcessVersion]              = {module = 'develop', photoSelected = true },
  [LrDevelopController.getRange]                       = {module = 'develop', photoSelected = true },
  [LrDevelopController.getSelectedTool]                = {module = 'develop', photoSelected = false },
  [LrDevelopController.getValue]                       = {module = 'develop', photoSelected = true },
  [LrDevelopController.increment]                      = {module = 'develop', photoSelected = true },
  [LrDevelopController.resetAllDevelopAdjustments]     = {module = 'develop', photoSelected = true },
  [LrDevelopController.resetBrushing]                  = {module = 'develop', photoSelected = true },
  [LrDevelopController.resetCircularGradient]          = {module = 'develop', photoSelected = true },
  [LrDevelopController.resetCrop]                      = {module = 'develop', photoSelected = true },
  [LrDevelopController.resetGradient]                  = {module = 'develop', photoSelected = true },
  [LrDevelopController.resetRedeye]                    = {module = 'develop', photoSelected = true },
  [LrDevelopController.resetSpotRemoval]               = {module = 'develop', photoSelected = true },
  [LrDevelopController.resetToDefault]                 = {module = 'develop', photoSelected = true },
  [LrDevelopController.revealAdjustedControls]         = {module = 'develop', photoSelected = false },
  [LrDevelopController.revealPanel]                    = {module = 'develop', photoSelected = false },
  [LrDevelopController.selectTool]                     = {module = 'develop', photoSelected = false },
  [LrDevelopController.setMultipleAdjustmentThreshold] = {module = 'develop', photoSelected = false },
  [LrDevelopController.setProcessVersion]              = {module = 'develop', photoSelected = true },
  [LrDevelopController.setTrackingDelay]               = {module = 'develop', photoSelected = false },
  [LrDevelopController.setValue]                       = {module = 'develop', photoSelected = true },
  [LrDevelopController.startTracking]                  = {module = 'develop', photoSelected = false },
  [LrDevelopController.stopTracking]                   = {module = 'develop', photoSelected = false },
}

if Ut.LrVersion100orMore
then
  needsModule[LrDevelopController.setActiveColorGradingView]      = {module = 'develop', photoSelected = false }
end

if Ut.LrVersion74orMore
then
  needsModule[LrDevelopController.showClipping]        = {module = 'develop', photoSelected = false}
end

if Ut.LrVersion66orMore
then
  needsModule[LrDevelopController.resetTransforms]     = {module = 'develop', photoSelected = true }
end


local _needsModule = {
  __index = function (t,k)
    t[k] = {module = nil, photoSelected = false}
    return t[k]
  end
}
setmetatable ( needsModule, _needsModule)

--public

--------------------------------------------------------------------------------
-- Returns function passed to it, with appropriate switch to Lightroom module if
-- needed.
-- This should wrap a module-specific Lightroom SDK function call unless you 
-- already know that Lightroom is set to the correct module. Stays in module that was 
-- opened after function ends.
-- @tparam function F The function to use.
-- @param ... Any arguments to the function.
-- @treturn function Function closure.
--------------------------------------------------------------------------------
local function wrapFOM(F,...)
  local openModule = needsModule[F]['module']
  if openModule == nil then
    return function() 
      return F(unpack(arg))  --proper tail call
    end
  end
  return function()
    if needsModule[F]['photoSelected'] and LrApplication.activeCatalog():getTargetPhoto() == nil then return end
    if LrApplicationView.getCurrentModuleName() ~= openModule then
      LrApplicationView.switchToModule(openModule)
      LrTasks.yield() -- need this to allow module change before F is called
    end
    return F(unpack(arg)) --proper tail call
  end
end

--------------------------------------------------------------------------------
-- Executes function passed to it, with appropriate switch to Lightroom module if
-- needed.
-- This should wrap a module-specific Lightroom SDK function call unless you 
-- already know that Lightroom is set to the correct module. Stays in module that was 
-- opened after function ends.
-- @tparam function F The function to use.
-- @param ... Any arguments to the function.
-- @return Results of passed function.
--------------------------------------------------------------------------------
local function execFOM(F,...)
  local openModule = needsModule[F]['module']
  if openModule == nil then
    return F(...) --proper tail call
  end
  if needsModule[F]['photoSelected'] and LrApplication.activeCatalog():getTargetPhoto() == nil then return end
  if LrApplicationView.getCurrentModuleName() ~= openModule then
    LrApplicationView.switchToModule(openModule)
    LrTasks.yield() -- need this to allow module change before F is called
  end
  return F(...) --proper tail call
end

local wfep_action = {
  QuickDevWBAuto                  = function(T) T:quickDevelopSetWhiteBalacne("Auto") end,--balance misspelled in api
  QuickDevWBCloudy                = function(T) T:quickDevelopSetWhiteBalacne("Cloudy") end,
  QuickDevWBDaylight              = function(T) T:quickDevelopSetWhiteBalacne("Daylight") end,
  QuickDevWBFlash                 = function(T) T:quickDevelopSetWhiteBalacne("Flash") end,
  QuickDevWBFluorescent           = function(T) T:quickDevelopSetWhiteBalacne("Fluorescent") end,
  QuickDevWBShade                 = function(T) T:quickDevelopSetWhiteBalacne("Shade") end,
  QuickDevWBTungsten              = function(T) T:quickDevelopSetWhiteBalacne("Tungsten") end,
  SetTreatmentBW                  = function(T) T:quickDevelopSetTreatment("grayscale") end,
  SetTreatmentColor               = function(T) T:quickDevelopSetTreatment("color") end,
  addOrRemoveFromTargetCollection = function(T) T:addOrRemoveFromTargetCollection() end,
  openExportDialog                = function(T) T:openExportDialog() end,
  openExportWithPreviousDialog    = function(T) T:openExportWithPreviousDialog() end,
  rotateLeft                      = function(T) T:rotateLeft() end,
  rotateRight                     = function(T) T:rotateRight() end,
}
-- equivalent to "LrApplication.activeCatalog():getTargetPhoto():rotateLeft()", e.g., with target checking
local function wrapForEachPhoto(F) --note lightroom applies this to all selected photos. no need to get all selected
  if not Ut.LrVersion74orMore then return function() end end -- not supported
  local SelectedAction = wfep_action
  [F]
  return function()    
    local LrCat = LrApplication.activeCatalog()
    local TargetPhoto  = LrCat:getTargetPhoto()
    if TargetPhoto then
      SelectedAction(TargetPhoto)
    end
  end
end

local function QuickCropAspect(aspect)
  local TargetPhoto = LrApplication.activeCatalog():getTargetPhoto()
  if TargetPhoto then
    TargetPhoto:quickDevelopCropAspect(aspect)
  end
end

local sb_precisionList = { 
  Blacks=0,
  BlueHue=0,
  BlueSaturation=0,
  Brightness=0,
  Clarity=0,
  ColorGradeBlending=0,
  ColorGradeBalance=0,
  ColorGradeGlobalHue=0,
  ColorGradeGlobalLum=0,
  ColorGradeGlobalSat=0,
  ColorGradeHighlightHue=0,
  ColorGradeHighlightLum=0,
  ColorGradeHighlightSat=0,
  ColorGradeMidtoneHue=0,
  ColorGradeMidtoneLum=0,
  ColorGradeMidtoneSat=0,
  ColorGradeShadowHue=0,
  ColorGradeShadowLum=0,
  ColorGradeShadowSat=0,
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
  Texture=0,
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
local function showBezel(param, value1, value2)
  local precision = sb_precisionList[param] or 4
  -- uses processVersion to index bezelname
  local processVersion = tonumber(LrDevelopController.getProcessVersion():match('%-?%d+'))
  -- ternary operator idiom. Note: not quite ternary operator. If [processVersion] doesn't match an entry, will use param instead.
  -- if CmdTrans[param] exists, will try first [processVersion] then, if that doesn't exist, [LatestPVSupported], then fallback to param
  -- this is in case someone doesn't update MIDI2LR but Adobe goes to a higher process version not supported by MIDI2LR CmdTrans table
  local bezelname = Database.CmdTrans[param] and (Database.CmdTrans[param][processVersion] or Database.CmdTrans[param][Database.LatestPVSupported]) or param
  if value2 then
    LrDialogs.showBezel(bezelname..'  '..LrStringUtils.numberToStringWithSeparators(value1,precision) .. '  ' ..LrStringUtils.numberToStringWithSeparators(value2,precision) )
  else
    LrDialogs.showBezel(bezelname..'  '..LrStringUtils.numberToStringWithSeparators(value1,precision))
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
    execFOM(LrDevelopController.revealPanel,panelname)
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
    if execFOM(LrDevelopController.getValue, param) == 0 then
      LrDevelopController.setValue(param,1)
    else
      LrDevelopController.setValue(param,0)
    end
  end
end

local function fToggle01Async(param)
  return function()
    LrTasks.startAsyncTask ( function ()
        if execFOM(LrDevelopController.getValue, param) == 0 then
          LrDevelopController.setValue(param,1)
        else
          LrDevelopController.setValue(param,0)
        end
      end )
  end
end

local function fToggle1ModN(param, N)
  return function()
    local v = execFOM(LrDevelopController.getValue, param)
    v = (v % N) + 1
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
      LrTasks.yield() -- need this to allow module change before next action
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

local ftt1_functionList = {
  gradient = LrDevelopController.goToDevelopGraduatedFilter,
  circularGradient = LrDevelopController.goToDevelopRadialFilter,
  dust = LrDevelopController.goToSpotRemoval,
}
local function fToggleTool1(param) --for new version toggle tool
  return function()
    if LrApplicationView.getCurrentModuleName() == 'develop' and 
    LrDevelopController.getSelectedTool() == param then
      LrDevelopController.selectTool('loupe')
    else 
      ftt1_functionList[param]()
    end
  end
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

local GradeToSplit = {
  ColorGradeHighlightHue      = 'SplitToningHighlightHue',
  ColorGradeHighlightSat      = 'SplitToningHighlightSaturation',
  ColorGradeShadowHue         = 'SplitToningShadowHue',
  ColorGradeShadowSat         = 'SplitToningShadowSaturation',
  ColorGradeBalance           = 'SplitToningBalance',
}
local function FullRefresh()
  -- if this code is changed, change similar code in Profiles.lua
  if Limits.LimitsCanBeSet() then
    -- refresh crop values
    local val = LrDevelopController.getValue("CropBottom")
    MIDI2LR.SERVER:send(string.format('CropBottomRight %g\n', val))
    MIDI2LR.SERVER:send(string.format('CropBottomLeft %g\n', val))
    MIDI2LR.SERVER:send(string.format('CropAll %g\n', val))
    MIDI2LR.SERVER:send(string.format('CropBottom %g\n', val))
    val = LrDevelopController.getValue("CropTop")
    MIDI2LR.SERVER:send(string.format('CropTopRight %g\n', val))
    MIDI2LR.SERVER:send(string.format('CropTopLeft %g\n', val))
    MIDI2LR.SERVER:send(string.format('CropTop %g\n', val))
    MIDI2LR.SERVER:send(string.format('CropLeft %g\n', LrDevelopController.getValue("CropLeft")))
    MIDI2LR.SERVER:send(string.format('CropRight %g\n', LrDevelopController.getValue("CropRight")))
    for param in pairs(Database.Parameters) do
      local min,max = Limits.GetMinMax(param)
      local lrvalue = LrDevelopController.getValue(param)
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
    for trueparam,param in pairs(GradeToSplit) do
      local min,max = Limits.GetMinMax(param)
      local lrvalue = LrDevelopController.getValue(param)
      if type(min) == 'number' and type(max) == 'number' and type(lrvalue) == 'number' then
        local midivalue = (lrvalue-min)/(max-min)
        if midivalue >= 1.0 then 
          MIDI2LR.SERVER:send(string.format('%s 1.0\n', trueparam))
        elseif midivalue <= 0.0 then -- = catches -0.0 and sends it as 0.0
          MIDI2LR.SERVER:send(string.format('%s 0.0\n', trueparam))
        else
          MIDI2LR.SERVER:send(string.format('%s %g\n', trueparam, midivalue))
        end
      end
    end
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
          MIDI2LR.SERVER:send('SendKey '.. keys ..'\n')
        end
      end
    else
      MIDI2LR.SERVER:send('SendKey '.. keys ..'\n')
    end
  end
end


local function UpdatePointCurve(settings)
  return function()
    fChangePanel('tonePanel')
    ApplySettings(settings)
  end
end

local cg_hsl_parms = {global={'ColorGradeGlobalHue','ColorGradeGlobalSat','ColorGradeGlobalLum'},
  highlight={'SplitToningHighlightHue','SplitToningHighlightSaturation','ColorGradeHighlightLum'},
  midtone={'ColorGradeMidtoneHue','ColorGradeMidtoneSat','ColorGradeMidtoneLum'},
  shadow={'SplitToningShadowHue','SplitToningShadowSaturation','ColorGradeShadowLum'}}

local cg_hsl = {0,0,0}

local function cg_hsl_copy()
  local currentView = LrDevelopController.getActiveColorGradingView()
  if currentView == '3-way' then
    return
  end
  cg_hsl = {LrDevelopController.getValue(cg_hsl_parms[currentView][1]),
    LrDevelopController.getValue(cg_hsl_parms[currentView][2]),
    LrDevelopController.getValue(cg_hsl_parms[currentView][3])}
end

local function cg_hsl_paste()
  local currentView = LrDevelopController.getActiveColorGradingView()
  if currentView == '3-way' then
    LrDevelopController.setValue(cg_hsl_parms.highlight[1],cg_hsl[1])
    LrDevelopController.setValue(cg_hsl_parms.highlight[2],cg_hsl[2])
    LrDevelopController.setValue(cg_hsl_parms.highlight[3],cg_hsl[3])
    LrDevelopController.setValue(cg_hsl_parms.midtone[1],cg_hsl[1])
    LrDevelopController.setValue(cg_hsl_parms.midtone[2],cg_hsl[2])
    LrDevelopController.setValue(cg_hsl_parms.midtone[3],cg_hsl[3])
    LrDevelopController.setValue(cg_hsl_parms.shadow[1],cg_hsl[1])
    LrDevelopController.setValue(cg_hsl_parms.shadow[2],cg_hsl[2])
    LrDevelopController.setValue(cg_hsl_parms.shadow[3],cg_hsl[3])
    FullRefresh()
    return
  end
  LrDevelopController.setValue(cg_hsl_parms[currentView][1],cg_hsl[1])
  LrDevelopController.setValue(cg_hsl_parms[currentView][2],cg_hsl[2])
  LrDevelopController.setValue(cg_hsl_parms[currentView][3],cg_hsl[3])
  FullRefresh()
end

local function cg_reset_3way()
  LrDevelopController.resetToDefault('SplitToningHighlightHue')
  LrDevelopController.resetToDefault('SplitToningHighlightSaturation')
  LrDevelopController.resetToDefault('ColorGradeHighlightLum')
  LrDevelopController.resetToDefault('ColorGradeMidtoneHue')
  LrDevelopController.resetToDefault('ColorGradeMidtoneSat')
  LrDevelopController.resetToDefault('ColorGradeMidtoneLum')
  LrDevelopController.resetToDefault('SplitToningShadowHue')
  LrDevelopController.resetToDefault('SplitToningShadowSaturation')
  LrDevelopController.resetToDefault('ColorGradeShadowLum')
  FullRefresh()
end

local function cg_reset_all()
  LrDevelopController.resetToDefault('ColorGradeGlobalHue')
  LrDevelopController.resetToDefault('ColorGradeGlobalSat')
  LrDevelopController.resetToDefault('ColorGradeGlobalLum')
  cg_reset_3way()
end

local function cg_reset_current()
  local currentView = LrDevelopController.getActiveColorGradingView()
  if currentView == '3-way' then
    cg_reset_3way()
    return
  end
  if currentView == 'global' then
    LrDevelopController.resetToDefault('ColorGradeGlobalHue')
    LrDevelopController.resetToDefault('ColorGradeGlobalSat')
    LrDevelopController.resetToDefault('ColorGradeGlobalLum')
    FullRefresh()
    return
  end
  if currentView == 'highlight' then
    LrDevelopController.resetToDefault('SplitToningHighlightHue')
    LrDevelopController.resetToDefault('SplitToningHighlightSaturation')
    LrDevelopController.resetToDefault('ColorGradeHighlightLum')
    FullRefresh()
    return
  end
  if currentView == 'midtone' then
    LrDevelopController.resetToDefault('ColorGradeMidtoneHue')
    LrDevelopController.resetToDefault('ColorGradeMidtoneSat')
    LrDevelopController.resetToDefault('ColorGradeMidtoneLum')
    FullRefresh()
    return
  end
  if currentView == 'shadow' then
    LrDevelopController.resetToDefault('SplitToningShadowHue')
    LrDevelopController.resetToDefault('SplitToningShadowSaturation')
    LrDevelopController.resetToDefault('ColorGradeShadowLum')
    FullRefresh()
    return
  end
end

local function ProfileAmount(value)
  local val = value -- make available to async task
  LrTasks.startAsyncTask ( function ()
      if LrApplication.activeCatalog():getTargetPhoto() == nil then return end
      LrApplication.activeCatalog():withWriteAccessDo(
        'MIDI2LR: Profile amount',
        function()
          local params = LrApplication.activeCatalog():getTargetPhoto():getDevelopSettings()
          if params and params.Look and params.Look.Amount then
            params.Look.Amount = val * 2
            LrApplication.activeCatalog():getTargetPhoto():applyDevelopSettings(params) 
            if ProgramPreferences.ClientShowBezelOnChange then
              local bezelname = (Database.CmdTrans.ProfileAmount and Database.CmdTrans.ProfileAmount[Database.LatestPVSupported]) or "Profile Amount"
              LrDialogs.showBezel(bezelname .. '  ' .. LrStringUtils.numberToStringWithSeparators(val*200, 0))
            end
          end
        end,
        { timeout = 4,
          callback = function()
            LrDialogs.showError(LOC("$$$/AgCustomMetadataRegistry/UpdateCatalog/Error=The catalog could not be updated with additional module metadata.")..' ProfileAmount')
          end,
          asynchronous = true
        }
      )
    end
  )
end

return {
  ApplySettings = ApplySettings,
  FullRefresh = FullRefresh,
  LRValueToMIDIValue = LRValueToMIDIValue,
  MIDIValueToLRValue = MIDIValueToLRValue,
  QuickCropAspect = QuickCropAspect,
  UpdateCameraProfile = UpdateCameraProfile,
  UpdatePointCurve = UpdatePointCurve,
  cg_hsl_copy = cg_hsl_copy,
  cg_hsl_paste = cg_hsl_paste,
  cg_reset_3way = cg_reset_3way,
  cg_reset_all = cg_reset_all,
  cg_reset_current = cg_reset_current,
  execFOM = execFOM,
  fApplyFilter = fApplyFilter,
  fApplyPreset = fApplyPreset,
  fChangeModule = fChangeModule,
  fChangePanel = fChangePanel,
  fSimulateKeys = fSimulateKeys,
  fToggle01 = fToggle01,
  fToggle01Async = fToggle01Async,
  fToggle1ModN = fToggle1ModN,
  fToggleTFasync = fToggleTFasync,
  fToggleTool = fToggleTool,
  fToggleTool1 = fToggleTool1,
  showBezel = showBezel,
  wrapFOM = wrapFOM,
  wrapForEachPhoto = wrapForEachPhoto,
  ProfileAmount = ProfileAmount,
}
