--------------------------------------------------------------------------------
-- Provides utility methods for MIDI2LR.
-- @module Utilities
-- @license GNU GPLv3
--------------------------------------------------------------------------------

--[[
Utilities.lua
Provides utility methods for MIDI2LR

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
--All variables and functions defined here must be local, except for return table.
--The only global identifiers used in this module are 'import'.
--In ZeroBrane IDE, check for global identifiers by pressing shift-F7 while 
--viewing this file.

--imports
local LrApplicationView   = import 'LrApplicationView'
local LrDevelopController = import 'LrDevelopController'

--hidden 
local needsModule = {
  [LrDevelopController.addAdjustmentChangeObserver]    = 'develop',
  [LrDevelopController.decrement]                      = 'develop',
  [LrDevelopController.getProcessVersion]              = 'develop',
  [LrDevelopController.getRange]                       = 'develop',
  [LrDevelopController.getSelectedTool]                = 'develop',
  [LrDevelopController.getValue]                       = 'develop',
  [LrDevelopController.increment]                      = 'develop',
  [LrDevelopController.resetAllDevelopAdjustments]     = 'develop',
  [LrDevelopController.resetBrushing]                  = 'develop',
  [LrDevelopController.resetCircularGradient]          = 'develop',
  [LrDevelopController.resetCrop]                      = 'develop',
  [LrDevelopController.resetGradient]                  = 'develop',
  [LrDevelopController.resetRedeye]                    = 'develop',
  [LrDevelopController.resetSpotRemoval]               = 'develop',
  [LrDevelopController.resetToDefault]                 = 'develop',
  [LrDevelopController.revealAdjustedControls]         = 'develop',
  [LrDevelopController.revealPanel]                    = 'develop',
  [LrDevelopController.selectTool]                     = 'develop',
  [LrDevelopController.setMultipleAdjustmentThreshold] = 'develop',
  [LrDevelopController.setProcessVersion]              = 'develop',
  [LrDevelopController.setTrackingDelay]               = 'develop',
  [LrDevelopController.setValue]                       = 'develop',
  [LrDevelopController.startTracking]                  = 'develop',
  [LrDevelopController.stopTracking]                   = 'develop',
}

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
  local openModule = needsModule[F]
  if openModule == nil then
    return function() 
      return F(unpack(arg))  --proper tail call
    end
  end
  return function()
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
  local openModule = needsModule[F]
  if openModule == nil then
    return function() 
      return F(unpack(arg))  --proper tail call
    end
  end
  return function()
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
  local openModule = needsModule[F]
  if openModule == nil then
    return F(...) --proper tail call
  end
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
  local openModule = needsModule[F]
  if openModule == nil then
    return F(...) --proper tail call
  end
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

--- @export
return { --table of exports, setting table member name and module function it points to
  wrapFOM = wrapFOM,
  wrapFCM = wrapFCM,
  execFOM = execFOM,
  execFCM = execFCM,
}
