--[[
This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
--]]

local LrApplication       = import 'LrApplication'
local LrApplicationView   = import 'LrApplicationView'
local LrDevelopController = import 'LrDevelopController'
local LrDialogs           = import 'LrDialogs'
local LrTasks             = import 'LrTasks'
local LrView              = import 'LrView'
local Database            = require 'Database'

--hidden
local DisplayOrder           = {'Temperature','Tint','Exposure','straightenAngle'}
local Fineness               = nil
local FineLimits             = {}

--public--each must be in table of exports

--------------------------------------------------------------------------------
-- Table listing parameter names managed by Limits module.
--------------------------------------------------------------------------------
local LimitParameters           = {}--{Temperature = true, Tint = true, Exposure = true}
for _, p in ipairs(DisplayOrder) do
  LimitParameters[p] = true
end

--clean up the preferences of discarded Limits and fix label and order
--has to be done after init and after loading full limits file

for k in pairs(ProgramPreferences.Limits) do
  if LimitParameters[k] then
    ProgramPreferences.Limits[k]['label'] = Database.CmdTrans[k][Database.LatestPVSupported]
  else
    ProgramPreferences.Limits[k] = nil --erase unused
  end
end

--------------------------------------------------------------------------------
-- Checks whether in Develop module and whether a photo is selected. This should
-- guard any calls that can reach into GetMinMax. Following is where calls should
-- be guarded. Choices were made based upon effect on program responsiveness.
-- Guard multiple calls to GetMinMax at one spot rather than further down the chain.
-- FullRefresh has test for LimitsCanBeSet, and can be called without checking.
-- MIDIValuetoLRValue has no test; all calls must check LimitsCanBeSet.
-- LRValueToMIDIValue has no test; all calls must check LimistCanBeSet.
-- ClampValue has no guard; all calls must be guarded.
-- doprofilechange is guarded.
-- @return bool as result of test
--------------------------------------------------------------------------------
local function LimitsCanBeSet()
  return
  (LrApplication.activeCatalog():getTargetPhoto() ~= nil) and
  (LrApplicationView.getCurrentModuleName() == 'develop')
end

--------------------------------------------------------------------------------
-- Provides min and max for given parameter and mode. Must be called when Fineness
-- is not nil. This function checks limits to ensure that fineness is calculated
-- within overall limits for a parameter.
-- @param param Which parameter is getting fine limits
-- @param lr_low Low end of Lightroom range
-- @param lr_max High end of Lightroom range
-- @param lr_value Current value of parameter
-- @return nil.
--------------------------------------------------------------------------------
local function CalcFineLimit(param, lr_low, lr_max, lr_value)
  if LimitParameters[param] ~= nil and type(ProgramPreferences.Limits[param]) == 'table' and type(ProgramPreferences.Limits[param][lr_max]) == 'table' then
    lr_low = ProgramPreferences.Limits[param][lr_max][1] -- use preset limits as range if available
    lr_max = ProgramPreferences.Limits[param][lr_max][2]
  end
  local lr_width = (lr_max - lr_low) / Fineness
  local lr_halfwidth = lr_width / 2
  local span = {lr_value - lr_halfwidth, lr_value + lr_halfwidth}
  if span[1] < lr_low then
    FineLimits[param] = {lr_low, lr_low + lr_width}
    return
  end
  if span[2] > lr_max then
    FineLimits[param] = {lr_max - lr_width, lr_max}
    return
  end
  FineLimits[param] = span
end
--------------------------------------------------------------------------------
-- Provides min and max for given parameter and mode. Must be called in Develop
-- module with photo selected. When this function is called with fineness active,
-- it also ensures that the fineness bounds enclose current LR value for parameter.
-- So any time this is called, fineness values are properly fixed.
-- @param param Which parameter is being adjusted.
-- @param lr_value optional: LR value when in fine mode if already known
-- @param ignorefineness optional: true if ignore fineness (used for limits dialog)
-- @return lr_min for given param and mode.
-- @return lr_max for given param and mode.
--------------------------------------------------------------------------------
local function GetMinMax(param, lr_value, ignorefineness)
  if Fineness == nil or ignorefineness then
    local lr_low,lr_max = LrDevelopController.getRange(param)
    if LimitParameters[param] then --should have limits
      if type(ProgramPreferences.Limits[param]) == 'table' and lr_max ~= nil then -- B&W picture may not have temperature, tint. This avoids indexing a nil rangemax and blowing up the metatable _index
        if type(ProgramPreferences.Limits[param][lr_max]) == 'table' then
          return ProgramPreferences.Limits[param][lr_max][1], ProgramPreferences.Limits[param][lr_max][2]
        else
          ProgramPreferences.Limits[param][lr_max] = {lr_low, lr_max}
        end
      else
        ProgramPreferences.Limits[param] = {param = param,
          label = Database.CmdTrans[param][Database.LatestPVSupported],
          rangemax = {lr_low,lr_max}}
      end
    end
    return lr_low, lr_max
  else --getminmax when fineness in effect, recalc limits if lr value outside of finelimits or limits missing
    lr_value = lr_value or LrDevelopController.getValue(param)
    if FineLimits[param] ~= nil and FineLimits[param][1] <= lr_value and FineLimits[param][2] >= lr_value then
      return FineLimits[param][1], FineLimits[param][2]
    else
      local low,rangemax = LrDevelopController.getRange(param)
      if rangemax and lr_value and low then -- return nil if values don't exist
        CalcFineLimit(param, low, rangemax, lr_value)
        return FineLimits[param][1], FineLimits[param][2]
      end
    end
  end
end

--------------------------------------------------------------------------------
-- Limits a given parameter to the min,max set up.
-- This function is used to avoid the bug in pickup mode, in which the parameter's
-- value may be too low or high to be picked up by the limited control range. By
-- clamping value to min-max range, this forces parameter to be in the limited
-- control range. This function should not be called unless in develop with
-- photo selected. Also check for existence of limits before calling.
-- @param Parameter to clamp to limits.
-- @return nil.
--------------------------------------------------------------------------------
local function ClampValue(param)
  local value = LrDevelopController.getValue(param)
  local min, max = GetMinMax(param, value)
  if value < min then
    MIDI2LR.PARAM_OBSERVER[param] = min
    LrDevelopController.setValue(param, min)
  elseif value > max then
    MIDI2LR.PARAM_OBSERVER[param] = max
    LrDevelopController.setValue(param, max)
  end
  return nil
end

local function MIDIValueToLRValue(param, midi_value)
  -- must be called when in develop module with photo selected
  -- map midi range to develop parameter range
  -- expects midi_value 0.0-1.0, doesn't protect against out-of-range
  local min,max = GetMinMax(param)
  return midi_value * (max-min) + min
end

local function LRValueToMIDIValue(param, lr_value) -- lr_value optional
  -- needs to be called in Develop module with photo selected
  -- map develop parameter range to midi range
  lr_value = lr_value or LrDevelopController.getValue(param)
  local min,max = GetMinMax(param,lr_value)
  local retval = (lr_value-min)/(max-min)
  if retval > 1 then return 1 end
  if retval < 0 then return 0 end
  return retval
end

local function RefreshMidiController()
  if (LrApplication.activeCatalog():getTargetPhoto() == nil) or (LrApplicationView.getCurrentModuleName() ~= 'develop') then
    return
  end
  LrTasks.startAsyncTask (function ()
      --[[-----------debug section, enable by adding - to beginning this line
      LrMobdebug.on()
      --]]-----------end debug section
      local photoval = LrApplication.activeCatalog():getTargetPhoto():getDevelopSettings()
      -- refresh crop values NOTE: this function is repeated in Client
      local midi_val_bottom = LRValueToMIDIValue('CropBottom')
      local midi_val_top = LRValueToMIDIValue('CropTop')
      local midi_val_left = LRValueToMIDIValue('CropLeft')
      MIDI2LR.SERVER:send(string.format('CropBottomRight %g\nCropBottomLeft %g\nCropAll %g\nCropTopRight %g\nCropTopLeft %g\nCropMoveVertical %g\nCropMoveHorizontal %g\n',
          midi_val_bottom,midi_val_bottom,midi_val_bottom,midi_val_top,midi_val_top,midi_val_top,midi_val_left))
      local sel_mask = LrDevelopController.getSelectedMask()
      for param,altparam in pairs(Database.Parameters) do
        LrTasks.yield()
        local lrvalue
        if altparam == 'Direct' then
          if (param:sub(1,6) ~= 'local_') or sel_mask then lrvalue = LrDevelopController.getValue(param) end
        else
          if photoval[altparam] ~= nil then
            lrvalue = photoval[altparam]
          elseif photoval[param] ~= nil then
            lrvalue = photoval[param]
          else
            lrvalue = 0
          end
        end
        local min,max = GetMinMax(param,lrvalue)
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

--------------------------------------------------------------------------------
-- Changes fineness parameter and resets table. Must be called in develop module
-- @param param Amount of fineness.
-- @param tested true if LimitsCanBeSet already called
-- @return nil.
--------------------------------------------------------------------------------
local fine_off = LOC('$$$/AgCameraRaw/LocalizedCorrections/LocalHue/FineAdjustment=Fine correction')..' '..LOC('$$$/TouchWorkspace/Adjustments/Off=Off')
local function Fine(value, tested)
  if tested or LimitsCanBeSet() then
    if Fineness == nil or Fineness ~= value then
      Fineness = value
    else
      Fineness = nil
    end
    FineLimits = {}
    RefreshMidiController()
  end
  if ProgramPreferences.ClientShowBezelOnChange then
    if Fineness then
      LrDialogs.showBezel(LOC('$$$/AgCameraRaw/LocalizedCorrections/LocalHue/FineAdjustment=Fine correction')..' '..Fineness..'×')
    else
      LrDialogs.showBezel(fine_off)
    end
  end
end

local function FineDecrease()
  if Fineness ~= nil and LimitsCanBeSet() then
    if Fineness == 2 then
      Fine(nil, true)
    else
      Fine(Fineness / 2, true)
    end
  end
end

local function FineIncrease()
  if LimitsCanBeSet() then
    if Fineness ~= nil then
      Fine(Fineness * 2, true)
    else
      Fine(2, true)
    end
  end
end

--------------------------------------------------------------------------------
-- Provide rows of controls for dialog boxes.
-- For the current photo type (HDR, raw, jpg, etc) will produce
-- rows that allow user to set limits. Must be in develop module and must
-- have photo selected before calling this function.
-- @param f The LrView.osfactory to use.
-- @param obstable The observable table to bind to dialog controls.
-- @return Table of f:rows populated with the needed dialog controls.
--------------------------------------------------------------------------------
local function OptionsRows(f,obstable)
  local retval = {}
  for _, p in ipairs(DisplayOrder) do
    local low,high = LrDevelopController.getRange(p)
    local integral = high - 5 > low
    retval[#retval+1] = f:row {
      f:static_text {
        title = ProgramPreferences.Limits[p]['label']..' '..LOC('$$$/MIDI2LR/Limits/Limits=Limits'),
        width = LrView.share('limit_label'),
      }, -- static_text
      f:static_text {
        title = LrView.bind('Limits'..p..'Low'),
        alignment = 'right',
        width = LrView.share('limit_reading'),
      }, -- static_text
      f:slider {
        value = LrView.bind('Limits'..p..'Low'),
        min = low,
        max = high,
        integral = integral,
        width = LrView.share('limit_slider'),
      }, -- slider
      f:static_text {
        title = LrView.bind('Limits'..p..'High'),
        alignment = 'right',
        width = LrView.share('limit_reading'),
      }, -- static_text
      f:slider {
        value = LrView.bind('Limits'..p..'High'),
        min = low ,
        max = high,
        integral = integral,
        width = LrView.share('limit_slider'),
      }, -- slider
      f:push_button {
        title = LOC("$$$/AgLibrary/CameraRawView/PresetMenu/DefaultSettings=Default settings"),
        action = function ()
          if p == 'Temperature' and low > 0 then
            obstable.LimitsTemperatureLow = 3000
            obstable.LimitsTemperatureHigh = 9000
          else
            obstable['Limits'..p..'Low'] = low
            obstable['Limits'..p..'High'] = high
          end
        end,
      }, -- push_button
    } -- row
  end
  return retval -- array of rows
end

local function StartDialog(obstable,f)
  if LimitsCanBeSet() then
    for _,p in ipairs(DisplayOrder) do
      local min,max = GetMinMax(p,nil,true)--ignore fineness for getminmax for dialog purposes
      obstable['Limits'..p..'Low'] = min
      obstable['Limits'..p..'High'] = max
    end
    return OptionsRows(f,obstable)
  else
    return {f:row {f:spacer{height=10},f:static_text{title = LOC("$$$/MIDI2LR/Limits/Limits=Limits").."\226\128\148"..LOC('$$$/MIDI2LR/Limits/Limits/NotDevelop=Active in "Develop" module only')},f:spacer{height=10},}}
  end
end

local function EndDialog(obstable, status)
  if status == 'ok' and LimitsCanBeSet() then
    --assign limits
    for _,p in ipairs(DisplayOrder) do
      if obstable['Limits'..p..'Low'] ~= nil and obstable['Limits'..p..'High'] ~= nil then
        if obstable['Limits'..p..'Low'] > obstable['Limits'..p..'High'] then --swap values
          obstable['Limits'..p..'Low'], obstable['Limits'..p..'High'] = obstable['Limits'..p..'High'], obstable['Limits'..p..'Low']
        end
        local _,max = LrDevelopController.getRange(p) --limitsCanBeSet only when in Develop module, no need to check again
        ProgramPreferences.Limits[p][max] = {obstable['Limits'..p..'Low'], obstable['Limits'..p..'High']}
      end
    end
  end
end

return {
  ClampValue  = ClampValue,
  EndDialog   = EndDialog,
  Fine           = Fine,
  FineDecrease   = FineDecrease,
  FineIncrease   = FineIncrease,
  GetMinMax   = GetMinMax,
  LimitsCanBeSet = LimitsCanBeSet,
  Parameters  = LimitParameters,
  RefreshMidiController = RefreshMidiController,
  StartDialog = StartDialog,
}
