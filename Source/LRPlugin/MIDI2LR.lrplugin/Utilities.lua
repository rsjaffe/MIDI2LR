--------------------------------------------------------------------------------
-- Provides utility methods for MIDI2LR.
-- @module Utilities
-- @license GNU GPLv3
--------------------------------------------------------------------------------

--[[
Utilities.lua
Provides utility methods for MIDI2LR

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>. 
--]]
--All variables and functions defined here must be local, except for return table.
--The only global identifiers used in this module are 'import'.
--In ZeroBrane IDE, check for global identifiers by pressing shift-F7 while 
--viewing this file.

--imports
local LrApplication       = import 'LrApplication'
local LrApplicationView   = import 'LrApplicationView'
local LrDevelopController = import 'LrDevelopController'

--hidden 
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
    end
    return F(unpack(arg)) --proper tail call
  end
end

--------------------------------------------------------------------------------
-- Returns function passed to it, with appropriate temporary switch to Lightroom 
-- module if needed.
-- This should wrap a module-specific Lightroom SDK function call unless you 
-- already know that Lightroom is set to the correct module. Returns to previous 
-- module after function ends.
-- @tparam function F The function to use.
-- @param ... Any arguments to the function.
-- @treturn function Function closure.
--------------------------------------------------------------------------------
local function wrapFCM(F,...)
  local openModule = needsModule[F]['module']
  if openModule == nil then
    return function() 
      return F(unpack(arg))  --proper tail call
    end
  end
  return function()
    if needsModule[F]['photoSelected'] and LrApplication.activeCatalog():getTargetPhoto() == nil then return end
    local currentMod = LrApplicationView.getCurrentModuleName()
    if currentMod ~= openModule then
      LrApplicationView.switchToModule(openModule)
    end
    F(unpack(arg))
    if currentMod ~= openModule then
      LrApplicationView.switchToModule(currentMod)
    end
  end
end

--------------------------------------------------------------------------------
-- Returns function passed to it, that will return without doing anything
-- if the needed module is not active.
-- This should wrap a module-specific Lightroom SDK function call unless you 
-- already know that Lightroom is set to the correct module. 
-- @tparam function F The function to use.
-- @param Rnil The return value to use if the correct module is not active.
-- @param ... Any arguments to the function.
-- @treturn function Function closure.
--------------------------------------------------------------------------------
local function wrapFIM(F,Rnil,...)
  local openModule = needsModule[F]['module']
  if openModule == nil then
    return function() 
      return F(unpack(arg))  --proper tail call
    end
  end
  return function ()
    if needsModule[F]['photoSelected'] and LrApplication.activeCatalog():getTargetPhoto() == nil then return Rnil end
    if LrApplicationView.getCurrentModuleName() ~= openModule then
      return Rnil
    end
    return F(unpack(arg)) -- proper tail call
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
  end
  return F(...) --proper tail call
end

--------------------------------------------------------------------------------
-- Executes function passed to it, with appropriate temporary switch to Lightroom 
-- module if needed.
-- This should wrap a module-specific Lightroom SDK function call unless you 
-- already know that Lightroom is set to the correct module. Returns to previous 
-- module after function ends.
-- @tparam function F The function to use.
-- @param ... Any arguments to the function.
-- @return Results of passed function.
--------------------------------------------------------------------------------
local function execFCM(F,...)
  local openModule = needsModule[F]['module']
  if openModule == nil then
    return F(...) --proper tail call
  end
  if needsModule[F]['photoSelected'] and LrApplication.activeCatalog():getTargetPhoto() == nil then return end
  local retval 
  local currentMod = LrApplicationView.getCurrentModuleName()
  if currentMod ~= openModule then
    LrApplicationView.switchToModule(openModule)
  end
  retval = F(...)
  if currentMod ~= openModule then
    LrApplicationView.switchToModule(currentMod)
  end
  return retval
end

--------------------------------------------------------------------------------
-- Executes function passed to it if the needed module is active.
-- This should wrap a module-specific Lightroom SDK function call unless you 
-- already know that Lightroom is set to the correct module. Returns Rnil
-- if it cannot execute the function.
-- @tparam function F The function to use.
-- @param Rnil The return value to use if the correct module is not active.
-- @param ... Any arguments to the function.
-- @treturn function Function closure.
--------------------------------------------------------------------------------
local function execFIM(F,Rnil,...)
  if needsModule[F]['photoSelected'] and LrApplication.activeCatalog():getTargetPhoto() == nil then return Rnil end
  local openModule = needsModule[F]['module']
  if openModule == nil or openModule == LrApplicationView.getCurrentModuleName() then
    return  F(...)  --proper tail call
  end
  return Rnil
end

--------------------------------------------------------------------------------
-- Calculates number of digits to display to right of decimal point.
-- Tries to maintain 4 digits of precision
-- @tparam number value The value that will be displayed
-- @treturn number Number of digits to display to right of decimal point
--------------------------------------------------------------------------------
local function precision(value)
  if value == 0 then
    return 0 
  else
    return math.max(0,3-math.floor(math.log10(math.abs(value))))
  end
end

--- @export
return { --table of exports, setting table member name and module function it points to
  wrapFOM = wrapFOM,
  wrapFCM = wrapFCM,
  wrapFIM = wrapFIM,
  execFOM = execFOM,
  execFCM = execFCM,
  execFIM = execFIM,
  precision = precision,
}
