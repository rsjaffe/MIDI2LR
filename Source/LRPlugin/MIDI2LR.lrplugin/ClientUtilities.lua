--[[----------------------------------------------------------------------------

ClientUtilities.lua

Procedures used by Client.lua. Moved to separate file as size of Client.lua
became unwieldy.
 
This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

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
local ParamList  = require 'ParamList'
local Paste      = require 'Paste'
local Profiles   = require 'Profiles'
local Ut         = require 'Utilities'
local LrApplication       = import 'LrApplication'
local LrApplicationView   = import 'LrApplicationView'
local LrBinding           = import 'LrBinding'
local LrDevelopController = import 'LrDevelopController'
local LrDialogs           = import 'LrDialogs'
local LrFunctionContext   = import 'LrFunctionContext'
local LrTasks             = import 'LrTasks'
local LrView              = import 'LrView'

local function fApplyPreset(presetnumber)
  return function()
    local presetUuid = ProgramPreferences.Presets[presetnumber]
    if presetUuid == nil or LrApplication.activeCatalog():getTargetPhoto() == nil then return end
    local preset = LrApplication.developPresetByUuid(presetUuid)
    LrTasks.startAsyncTask ( function () 
        LrApplication.activeCatalog():withWriteAccessDo(
          'Apply preset '..preset:getName(), 
          function() LrApplication.activeCatalog():getTargetPhoto():applyDevelopPreset(preset) end,
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
    if Ut.execFOM(LrDevelopController.getValue(param)) == 0 then
      LrDevelopController.setValue(param,1)
    else
      LrDevelopController.setValue(param,0)
    end
  end
end

local function fToggleTF(param)
  return function()
    LrDevelopController.setValue(param,not Ut.execFOM(LrDevelopController.getValue,param))
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


local function PasteSelectedSettings ()
  if MIDI2LR.Copied_Settings == nil or LrApplication.activeCatalog():getTargetPhoto() == nil then return end 
  if ProgramPreferences.PastePopup then 
    LrFunctionContext.callWithContext( "checkPaste", 
      function( context )
        local f = LrView.osFactory()
        local properties = LrBinding.makePropertyTable( context )
        local result = LrDialogs.presentModalDialog (
          { 
            title = LOC('$$$/MIDI2LR/Options/pastesel=Paste selections') ,
            contents = f:view{ bind_to_object = properties, Paste.StartDialog(properties,f) }
          }
        )
        if result == 'ok' then
          Paste.EndDialog (properties,result)
        end
      end 
    )
  end
  if LrApplicationView.getCurrentModuleName() ~= 'develop' then
    LrApplicationView.switchToModule('develop')
  end
  LrTasks.startAsyncTask ( 
    function ()
      local TargetSettings = LrApplication.activeCatalog():getTargetPhoto():getDevelopSettings() 
      for _,param in ipairs(ParamList.SelectivePasteIteration) do 
        if (ProgramPreferences.PasteList[param] and MIDI2LR.Copied_Settings[param]~=nil) then
          TargetSettings[param] = MIDI2LR.Copied_Settings[param]
        end
      end
      LrApplication.activeCatalog():withWriteAccessDo(
        'MIDI2LR: Paste selected settings', 
        function() LrApplication.activeCatalog():getTargetPhoto():applyDevelopSettings(TargetSettings) end,
        { timeout = 4, 
          callback = function() 
            LrDialogs.showError(LOC("$$$/AgCustomMetadataRegistry/UpdateCatalog/Error=The catalog could not be updated with additional module metadata.")..' PasteSelectedSettings') 
          end, 
          asynchronous = true 
        }
      )
    end
  ) 
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

local function AddToCollection()
  local catalog = LrApplication.activeCatalog()
  local quickname = catalog.kQuickCollectionIdentifier
  local targetname = catalog.kTargetCollection
  local quickcollection, targetcollection
  LrTasks.startAsyncTask (
    function () 
      LrApplication.activeCatalog():withWriteAccessDo( 
        '',
        function()
          quickcollection = catalog:createCollection(quickname,nil,true)
          targetcollection = catalog:createCollection(targetname,nil,true)
        end,
        { timeout = 4, 
          callback = function() LrDialogs.showError(LOC("$$$/AgCustomMetadataRegistry/UpdateCatalog/Error=The catalog could not be updated with additional module metadata.")..' GetCollection.') end, 
          asynchronous = true 
        }
      )
    end
  )
  return function(collectiontype,photos)
    if collectiontype==nil or photos==nil then return end
    local CollectionName
    if collectiontype == 'quick' then
      CollectionName = "$$$/AgLibrary/ThumbnailBadge/AddToQuickCollection=Add to Quick Collection."
    else
      CollectionName = "$$$/AgLibrary/ThumbnailBadge/AddToTargetCollection=Add to Target Collection"
    end
    LrTasks.startAsyncTask ( 
      function () 
        LrApplication.activeCatalog():withWriteAccessDo( 
          CollectionName,
          function()
            if LrApplication.activeCatalog() ~= catalog then
              catalog = LrApplication.activeCatalog()
              quickname = catalog.kQuickCollectionIdentifier
              targetname = catalog.kTargetCollection
              quickcollection = catalog:createCollection(quickname,nil,true)
              targetcollection = catalog:createCollection(targetname,nil,true)
            elseif catalog.kTargetCollection ~= targetname and collectiontype ~= 'quick' then
              targetcollection = catalog:createCollection(targetname,nil,true)
            end
            local usecollection
            if collectiontype == 'quick' then
              usecollection = quickcollection
            else
              usecollection = targetcollection
            end
            if type(photos)==table then
              usecollection:addPhotos(photos)
            else
              usecollection:addPhotos {photos}
            end
          end,
          { timeout = 4, 
            callback = function() 
              LrDialogs.showError(LOC("$$$/AgCustomMetadataRegistry/UpdateCatalog/Error=The catalog could not be updated with additional module metadata.")..' AddToCollection.') 
            end, 
            asynchronous = true 
          }
        )
      end
    )
  end
end
AddToCollection = AddToCollection() --closure

local function FullRefresh()
  for _,param in ipairs(ParamList.SendToMidi) do
    local min,max = Limits.GetMinMax(param)
    local lrvalue = LrDevelopController.getValue(param)
    if type(min) == 'number' and type(max) == 'number' and type(lrvalue) == 'number' then
      local midivalue = (lrvalue-min)/(max-min)
      MIDI2LR.SERVER:send(string.format('%s %g\n', param, midivalue))
    end
  end
end


return {
  AddToCollection = AddToCollection,
  CopySettings = CopySettings,
  fApplyPreset = fApplyPreset,
  fChangeModule = fChangeModule,
  fChangePanel = fChangePanel,
  fToggle01 = fToggle01,
  fToggleTF = fToggleTF,
  fToggleTool = fToggleTool,
  FullRefresh = FullRefresh,
  PasteSelectedSettings = PasteSelectedSettings,
  PasteSettings = PasteSettings,

}
