--[[----------------------------------------------------------------------------

Unused.lua
Code that may be of value later but is currently unused
 
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

local function fToggleTF(param)
  return function()
    local v = execFOM(LrDevelopController.getValue, param)
    v = not v
    LrDevelopController.setValue(param,v)    
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

local function CopySettings ()
  if LrApplication.activeCatalog():getTargetPhoto() == nil then return end
  LrTasks.startAsyncTask ( 
    function () 
      MIDI2LR.Copied_Settings = LrApplication.activeCatalog():getTargetPhoto():getDevelopSettings() 
    end
  ) 
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