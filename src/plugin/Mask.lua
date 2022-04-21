--[[----------------------------------------------------------------------------

Mask.lua

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

local LrApplication       = import 'LrApplication'
local LrApplicationView   = import 'LrApplicationView'
local LrDevelopController = import 'LrDevelopController'
local LrDialogs           = import 'LrDialogs'
local Database            = require 'Database'

local function DeleteMask()
  if  (LrApplication.activeCatalog():getTargetPhoto() == nil) or
  (LrApplicationView.getCurrentModuleName() ~= 'develop') or
  (LrDevelopController.getSelectedTool() ~= 'masking') then
    return
  end
  local currentmask = LrDevelopController.getSelectedMask()
  if currentmask then
    LrDevelopController.deleteMask(currentmask)
    if ProgramPreferences.ClientShowBezelOnChange then
      LrDialogs.showBezel(Database.CmdTrans.MaskDelete)
    end
  end
end

local function DeleteMaskTool()
  if  (LrApplication.activeCatalog():getTargetPhoto() == nil) or
  (LrApplicationView.getCurrentModuleName() ~= 'develop') or
  (LrDevelopController.getSelectedTool() ~= 'masking') then
    return
  end
  local currenttool = LrDevelopController.getSelectedMaskTool()
  if currenttool then
    LrDevelopController.deleteMaskTool(currenttool)
    if ProgramPreferences.ClientShowBezelOnChange then
      LrDialogs.showBezel(Database.CmdTrans.MaskDeleteTool)
    end
  end
end

local function ToggleInvertMaskTool()
  if  (LrApplication.activeCatalog():getTargetPhoto() == nil) or
  (LrApplicationView.getCurrentModuleName() ~= 'develop') or
  (LrDevelopController.getSelectedTool() ~= 'masking') then
    return
  end
  local currenttool = LrDevelopController.getSelectedMaskTool()
  if currenttool then
    LrDevelopController.toggleInvertMaskTool(currenttool)
    if ProgramPreferences.ClientShowBezelOnChange then
      LrDialogs.showBezel(Database.CmdTrans.MaskInvertTool)
    end
  end
end

local function ToggleHideMask()
  if  (LrApplication.activeCatalog():getTargetPhoto() == nil) or
  (LrApplicationView.getCurrentModuleName() ~= 'develop') or
  (LrDevelopController.getSelectedTool() ~= 'masking') then
    return
  end
  local currentmask = LrDevelopController.getSelectedMask()
  if currentmask then
    LrDevelopController.toggleHideMask(currentmask)
    if ProgramPreferences.ClientShowBezelOnChange then
      LrDialogs.showBezel(Database.CmdTrans.MaskHide)
    end
  end
end

local function ToggleHideMaskTool()
  if  (LrApplication.activeCatalog():getTargetPhoto() == nil) or
  (LrApplicationView.getCurrentModuleName() ~= 'develop') or
  (LrDevelopController.getSelectedTool() ~= 'masking') then
    return
  end
  local currenttool = LrDevelopController.getSelectedMaskTool()
  if currenttool then
    LrDevelopController.toggleHideMaskTool(currenttool)
    if ProgramPreferences.ClientShowBezelOnChange then
      LrDialogs.showBezel(Database.CmdTrans.MaskHideTool)
    end
  end
end


local function PreviousMask()
  if  (LrApplication.activeCatalog():getTargetPhoto() == nil) or
  (LrApplicationView.getCurrentModuleName() ~= 'develop') or
  (LrDevelopController.getSelectedTool() ~= 'masking') then
    return
  end
  local allmasks = LrDevelopController.getAllMasks()
  local currentmask = LrDevelopController.getSelectedMask()
  for idx,value in ipairs(allmasks) do
    if value.ID == currentmask then
      if idx > 1 then
        LrDevelopController.selectMask(allmasks[idx-1]['ID'])
        LrDevelopController.selectMaskTool(allmasks[idx-1][1]['ID'])
        if ProgramPreferences.ClientShowBezelOnChange then
          LrDialogs.showBezel(allmasks[idx-1]['Name']..'\226\134\146'..allmasks[idx-1][1]['Name'])
        end
      end
      return
    end
  end
end

local function NextMask()
  if  (LrApplication.activeCatalog():getTargetPhoto() == nil) or
  (LrApplicationView.getCurrentModuleName() ~= 'develop') or
  (LrDevelopController.getSelectedTool() ~= 'masking') then
    return
  end
  local allmasks = LrDevelopController.getAllMasks()
  local currentmask = LrDevelopController.getSelectedMask()
  for idx,amask in ipairs(allmasks) do
    if amask.ID == currentmask then
      if idx < #allmasks then
        LrDevelopController.selectMask(allmasks[idx+1]['ID'])
        LrDevelopController.selectMaskTool(allmasks[idx+1][1]['ID'])
        if ProgramPreferences.ClientShowBezelOnChange then
          LrDialogs.showBezel(allmasks[idx+1]['Name']..'\226\134\146'..allmasks[idx+1][1]['Name'])
        end
      end
      return
    end
  end
end

local function PreviousTool()
  if  (LrApplication.activeCatalog():getTargetPhoto() == nil) or
  (LrApplicationView.getCurrentModuleName() ~= 'develop') or
  (LrDevelopController.getSelectedTool() ~= 'masking') then
    return
  end
  local allmasks = LrDevelopController.getAllMasks()
  local currentmask = LrDevelopController.getSelectedMask()
  local currenttool = LrDevelopController.getSelectedMaskTool()
  for _,amask in ipairs(allmasks) do
    if amask.ID == currentmask then
      for idx2,atool in ipairs(amask) do
        if atool.ID == currenttool then
          if idx2 > 1 then
            LrDevelopController.selectMask(currentmask)
            LrDevelopController.selectMaskTool(amask[idx2-1]['ID'])
            if ProgramPreferences.ClientShowBezelOnChange then
              LrDialogs.showBezel(amask.Name..'\226\134\146'..amask[idx2-1]['Name'])
            end
          end
          return
        end
      end
      return
    end
  end
end

local function NextTool()
  if  (LrApplication.activeCatalog():getTargetPhoto() == nil) or
  (LrApplicationView.getCurrentModuleName() ~= 'develop') or
  (LrDevelopController.getSelectedTool() ~= 'masking') then
    return
  end
  local allmasks = LrDevelopController.getAllMasks()
  local currentmask = LrDevelopController.getSelectedMask()
  local currenttool = LrDevelopController.getSelectedMaskTool()
  for _,amask in ipairs(allmasks) do
    if amask.ID == currentmask then
      for idx2,atool in ipairs(amask) do
        if atool.ID == currenttool then
          if idx2 < #amask then
            LrDevelopController.selectMask(currentmask)
            LrDevelopController.selectMaskTool(amask[idx2+1]['ID'])
            if ProgramPreferences.ClientShowBezelOnChange then
              LrDialogs.showBezel(amask.Name..'\226\134\146'..amask[idx2+1]['Name'])
            end
          end
          return
        end
      end
      return
    end
  end
end

local function InvertMask()
  if  (LrApplication.activeCatalog():getTargetPhoto() == nil) or
  (LrApplicationView.getCurrentModuleName() ~= 'develop') or
  (LrDevelopController.getSelectedTool() ~= 'masking') then
    return
  end
  local currentmask = LrDevelopController.getSelectedMask()
  if currentmask then
    LrDevelopController.invertMask(currentmask)
    if ProgramPreferences.ClientShowBezelOnChange then
      LrDialogs.showBezel(Database.CmdTrans.MaskInvert)
    end
  end
end

local function InvertDuplicateMask()
  if  (LrApplication.activeCatalog():getTargetPhoto() == nil) or
  (LrApplicationView.getCurrentModuleName() ~= 'develop') or
  (LrDevelopController.getSelectedTool() ~= 'masking') then
    return
  end
  local currentmask = LrDevelopController.getSelectedMask()
  if currentmask then
    LrDevelopController.duplicateAndInvertMask(currentmask)
    if ProgramPreferences.ClientShowBezelOnChange then
      LrDialogs.showBezel(Database.CmdTrans.MaskInvertDup)
    end
  end
end

return {
  DeleteMask           = DeleteMask,
  DeleteMaskTool       = DeleteMaskTool,
  InvertDuplicateMask  = InvertDuplicateMask,
  InvertMask           = InvertMask,
  NextMask             = NextMask,
  NextTool             = NextTool,
  PreviousMask         = PreviousMask,
  PreviousTool         = PreviousTool,
  ToggleHideMask       = ToggleHideMask,
  ToggleHideMaskTool   = ToggleHideMaskTool,
  ToggleInvertMaskTool = ToggleInvertMaskTool,
}
