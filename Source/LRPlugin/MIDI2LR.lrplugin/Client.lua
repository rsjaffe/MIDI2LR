--[[----------------------------------------------------------------------------

Client.lua

Receives and processes commands from MIDI2LR
Sends parameters to MIDI2LR
 
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
--[[-----------debug section, enable by adding - to beginning this line
local LrMobdebug = import 'LrMobdebug'
LrMobdebug.start()
--]]-----------end debug section
local Limits      = require 'Limits'
local Parameters  = require 'Parameters'
local Paste       = require 'Paste'
local Preferences = require 'Preferences'
local Profiles    = require 'Profiles'
local Ut          = require 'Utilities'
local LrApplication       = import 'LrApplication'
local LrApplicationView   = import 'LrApplicationView'
local LrBinding           = import 'LrBinding'
local LrDevelopController = import 'LrDevelopController'
local LrDialogs           = import 'LrDialogs'
local LrFunctionContext   = import 'LrFunctionContext'
local LrSelection         = import 'LrSelection'
local LrShell             = import 'LrShell'
local LrSocket            = import 'LrSocket'
local LrTasks             = import 'LrTasks'
local LrUndo              = import 'LrUndo'
local LrView              = import 'LrView'
-- signal for halt plugin if reloaded--LR doesn't kill main loop otherwise
math.randomseed(os.time())
currentLoadVersion = rawget (_G, 'currentLoadVersion') or math.random()  
currentLoadVersion = currentLoadVersion + 1 

MIDI2LR = {RECEIVE_PORT = 58763, SEND_PORT = 58764, PICKUP_THRESHOLD = 4, CONTROL_MAX = 127, BUTTON_ON = 127; --constants
  LAST_PARAM = '', PARAM_OBSERVER = {}, PICKUP_ENABLED = true, SERVER = {} } --non-local but in MIDI2LR namespace

-------------preferences
Preferences.Load() 
-------------end preferences section


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
        Paste.EndDialog (properties,result)
      end 
    )
  end
  if LrApplicationView.getCurrentModuleName() ~= 'develop' then
    LrApplicationView.switchToModule('develop')
  end
  for _,param in ipairs(Parameters.Order) do 
    if (ProgramPreferences.PasteList[param] and MIDI2LR.Copied_Settings[param]~=nil) then
      MIDI2LR.PARAM_OBSERVER[param] = MIDI2LR.Copied_Settings[param]
      LrDevelopController.setValue(param,MIDI2LR.Copied_Settings[param])
    end
  end
  for param in pairs(Paste.Duplicates) do
    if (ProgramPreferences.PasteList[param] and MIDI2LR.Copied_Settings[param]~=nil) then
      MIDI2LR.PARAM_OBSERVER[param] = MIDI2LR.Copied_Settings[param]
      LrDevelopController.setValue(param,MIDI2LR.Copied_Settings[param])
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

local function CopySettings ()
  if LrApplication.activeCatalog():getTargetPhoto() == nil then return end
  LrTasks.startAsyncTask ( 
    function () 
      MIDI2LR.Copied_Settings = LrApplication.activeCatalog():getTargetPhoto():getDevelopSettings() 
    end
  ) 
end

local function ApplyPreset(presetUuid)
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

local function addToCollection()
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
    if LrApplication.activeCatalog():getTargetPhoto() == nil then return end
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

addToCollection = addToCollection()

local ACTIONS = {
  CopySettings                  = CopySettings,
  DecreaseRating                = LrSelection.decreaseRating,
  DecrementLastDevelopParameter = function() Ut.execFOM(LrDevelopController.decrement,MIDI2LR.LAST_PARAM) end,
  IncreaseRating                = LrSelection.increaseRating,
  IncrementLastDevelopParameter = function() Ut.execFOM(LrDevelopController.increment,MIDI2LR.LAST_PARAM) end,
  Next                          = LrSelection.nextPhoto,
  PasteSelectedSettings         = PasteSelectedSettings,
  PasteSettings                 = PasteSettings,
  Pick                          = LrSelection.flagAsPick,
  Prev                          = LrSelection.previousPhoto,
  Profile_Adobe_Standard   = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Adobe Standard'),
  Profile_Camera_Clear     = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Clear'),
  Profile_Camera_Deep      = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Deep'),
  Profile_Camera_Landscape = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Landscape'),
  Profile_Camera_Light     = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Light'),
  Profile_Camera_Neutral   = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Neutral'),
  Profile_Camera_Portrait  = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Portrait'),
  Profile_Camera_Standard  = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Standard'),
  Profile_Camera_Vivid     = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Vivid'),
  profile1 = function() Profiles.changeProfile('profile1', true) end,
  profile2 = function() Profiles.changeProfile('profile2', true) end,
  profile3 = function() Profiles.changeProfile('profile3', true) end,
  profile4 = function() Profiles.changeProfile('profile4', true) end,
  profile5 = function() Profiles.changeProfile('profile5', true) end,
  profile6 = function() Profiles.changeProfile('profile6', true) end,
  profile7 = function() Profiles.changeProfile('profile7', true) end,
  profile8 = function() Profiles.changeProfile('profile8', true) end,
  profile9 = function() Profiles.changeProfile('profile9', true) end,
  profile10 = function() Profiles.changeProfile('profile10', true) end,
  Redo             = LrUndo.redo,
  Reject           = LrSelection.flagAsReject,
  RemoveFlag       = LrSelection.removeFlag,
  ResetAll         = Ut.wrapFOM(LrDevelopController.resetAllDevelopAdjustments),
  ResetBrushing    = Ut.wrapFOM(LrDevelopController.resetBrushing),
  ResetCircGrad    = Ut.wrapFOM(LrDevelopController.resetCircularGradient),
  ResetCrop        = Ut.wrapFOM(LrDevelopController.resetCrop),
  ResetGradient    = Ut.wrapFOM(LrDevelopController.resetGradient),
  ResetLast        = function() Ut.execFOM(LrDevelopController.resetToDefault,MIDI2LR.LAST_PARAM) end,
  ResetRedeye      = Ut.wrapFOM(LrDevelopController.resetRedeye),
  ResetSpotRem     = Ut.wrapFOM(LrDevelopController.resetSpotRemoval),
  RevealPanelAdjust = function() 
    Ut.execFOM(LrDevelopController.revealPanel,'adjustPanel')
    Profiles.changeProfile('adjustPanel') 
  end,
  RevealPanelCalibrate = function() 
    Ut.execFOM(LrDevelopController.revealPanel,'calibratePanel')
    Profiles.changeProfile('calibratePanel') 
  end,
  RevealPanelDetail = function() 
    Ut.execFOM(LrDevelopController.revealPanel,'detailPanel')
    Profiles.changeProfile('detailPanel') 
  end,
  RevealPanelEffects = function() 
    Ut.execFOM(LrDevelopController.revealPanel,'effectsPanel')
    Profiles.changeProfile('effectsPanel') 
  end,
  RevealPanelLens = function() 
    Ut.execFOM(LrDevelopController.revealPanel,'lensCorrectionsPanel')
    Profiles.changeProfile('lensCorrectionsPanel') 
  end,
  RevealPanelMixer = function() 
    Ut.execFOM(LrDevelopController.revealPanel,'mixerPanel')
    Profiles.changeProfile('mixerPanel') 
  end,
  RevealPanelSplit = function() 
    Ut.execFOM(LrDevelopController.revealPanel,'splitToningPanel')
    Profiles.changeProfile('splitToningPanel') 
  end,
  RevealPanelTone = function() 
    Ut.execFOM(LrDevelopController.revealPanel,'tonePanel')
    Profiles.changeProfile('tonePanel') 
  end,
  Select1Left = LrSelection.extendSelection('left',1),
  Select1Right = LrSelection.extendSelection('right',1),
  SetRating0 = function() LrSelection.setRating(0) end,
  SetRating1 = function() LrSelection.setRating(1) end,
  SetRating2 = function() LrSelection.setRating(2) end,
  SetRating3 = function() LrSelection.setRating(3) end,
  SetRating4 = function() LrSelection.setRating(4) end,
  SetRating5 = function() LrSelection.setRating(5) end,
  ShoScndVwloupe        = function() LrApplicationView.showSecondaryView('loupe') end,
  ShoScndVwlive_loupe   = function() LrApplicationView.showSecondaryView('live_loupe') end,
  ShoScndVwlocked_loupe = function() LrApplicationView.showSecondaryView('locked_loupe') end,
  ShoScndVwgrid         = function() LrApplicationView.showSecondaryView('grid') end,
  ShoScndVwcompare      = function() LrApplicationView.showSecondaryView('compare') end,
  ShoScndVwsurvey       = function() LrApplicationView.showSecondaryView('survey') end,
  ShoScndVwslideshow    = function() LrApplicationView.showSecondaryView('slideshow') end,  
  ShoVwloupe   = function() LrApplicationView.showView('loupe') end,
  ShoVwgrid    = function() LrApplicationView.showView('grid') end,
  ShoVwcompare = function() LrApplicationView.showView('compare') end,
  ShoVwsurvey  = function() LrApplicationView.showView('survey') end,
  ShoVwpeople  = function() LrApplicationView.showView('people') end,
  ShoVwdevelop_loupe              = function() LrApplicationView.showView('develop_loupe') end,
  ShoVwdevelop_before_after_horiz = function() LrApplicationView.showView('develop_before_after_horiz') end,
  ShoVwdevelop_before_after_vert  = function() LrApplicationView.showView('develop_before_after_vert') end,
  ShoVwdevelop_before             = function() LrApplicationView.showView('develop_before') end,
  SwToMlibrary = function() 
    LrApplicationView.switchToModule('library') 
    Profiles.changeProfile('library') 
  end,
  SwToMdevelop = function() 
    LrApplicationView.switchToModule('develop') 
    Profiles.changeProfile('develop') 
  end,
  SwToMmap = function() 
    LrApplicationView.switchToModule('map') 
    Profiles.changeProfile('map') 
  end,
  SwToMbook = function() 
    LrApplicationView.switchToModule('book') 
    Profiles.changeProfile('book') 
  end,
  SwToMslideshow = function() 
    LrApplicationView.switchToModule('slideshow') 
    Profiles.changeProfile('slideshow') 
  end,
  SwToMprint = function() 
    LrApplicationView.switchToModule('print') 
    Profiles.changeProfile('print') 
  end,
  SwToMweb = function() 
    LrApplicationView.switchToModule('web') 
    Profiles.changeProfile('web') 
  end,
  ToggleBlue       = LrSelection.toggleBlueLabel,
  ToggleGreen      = LrSelection.toggleGreenLabel,
  TogglePurple     = LrSelection.togglePurpleLabel,
  ToggleRed        = LrSelection.toggleRedLabel,
  ToggleScreenTwo  = LrApplicationView.toggleSecondaryDisplay,
  ToggleYellow     = LrSelection.toggleYellowLabel,
  ToggleZoomOffOn  = LrApplicationView.toggleZoom,
  Undo             = LrUndo.undo,
  UprightAuto      = Ut.wrapFOM(LrDevelopController.setValue,'PerspectiveUpright',1),
  UprightFull      = Ut.wrapFOM(LrDevelopController.setValue,'PerspectiveUpright',2),
  UprightLevel     = Ut.wrapFOM(LrDevelopController.setValue,'PerspectiveUpright',3),
  UprightOff       = Ut.wrapFOM(LrDevelopController.setValue,'PerspectiveUpright',0),
  UprightVertical  = Ut.wrapFOM(LrDevelopController.setValue,'PerspectiveUpright',4),
  VirtualCopy      = function() LrApplication.activeCatalog():createVirtualCopies() end,
  WhiteBalanceAs_Shot     = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','As Shot'),
  WhiteBalanceAuto        = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','Auto'),
  WhiteBalanceCloudy      = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','Cloudy'),
  WhiteBalanceDaylight    = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','Daylight'),
  WhiteBalanceFlash       = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','Flash'),  
  WhiteBalanceFluorescent = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','Fluorescent'),
  WhiteBalanceShade       = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','Shade'),
  WhiteBalanceTungsten    = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','Tungsten'),
  ZoomInLargeStep  = LrApplicationView.zoomIn,
  ZoomInSmallStep  = LrApplicationView.zoomInSome,
  ZoomOutLargeStep = LrApplicationView.zoomOut,
  ZoomOutSmallStep = LrApplicationView.zoomOutSome,
}

local TOOL_ALIASES = {
  AdjustmentBrush = 'localized',
  CropOverlay     = 'crop',
  GraduatedFilter = 'gradient',
  Loupe           = 'loupe',
  RadialFilter    = 'circularGradient',
  RedEye          = 'redeye',
  SpotRemoval     = 'dust',
}

local TOGGLE_PARAMETERS = { --alternate on/off by button presses
  ConvertToGrayscale                     = true,
  EnableCalibration                      = true,
  EnableCircularGradientBasedCorrections = true,
  EnableColorAdjustments                 = true,
  EnableDetail                           = true,
  EnableEffects                          = true,
  EnableGradientBasedCorrections         = true,
  EnableGrayscaleMix                     = true,
  EnableLensCorrections                  = true,
  EnablePaintBasedCorrections            = true,
  EnableRedEye                           = true,
  EnableRetouch                          = true,
  EnableSplitToning                      = true,
}

local TOGGLE_PARAMETERS_01 = { --alternate on/off, but 0/1 rather than false/true
  AutoLateralCA        = true,
  LensProfileEnable    = true,
}


local SETTINGS = {
  Pickup = function(enabled) MIDI2LR.PICKUP_ENABLED = (enabled == 1) end,
}

local function midi_lerp_to_develop(param, midi_value)
  -- map midi range to develop parameter range
  local min,max = Limits.GetMinMax(param)
  return midi_value/MIDI2LR.CONTROL_MAX * (max-min) + min
end

local function develop_lerp_to_midi(param)
  -- map develop parameter range to midi range
  local min,max = Limits.GetMinMax(param)
  return (LrDevelopController.getValue(param)-min)/(max-min) * MIDI2LR.CONTROL_MAX
end

local function updateParam() --closure
  local lastclock, lastparam --tracking for pickup when scrubbing control rapidly
  return function(param, midi_value)
    -- this function does a 'pickup' type of check
    -- that is, it will ensure the develop parameter is close 
    -- to what the inputted command value is before updating it
    if LrApplicationView.getCurrentModuleName() ~= 'develop' then
      LrApplicationView.switchToModule('develop')
    end
    -- if pickup mode, keep LR value within pickup limits so pickup can work
    if Limits.Parameters[param] and MIDI2LR.PICKUP_ENABLED then
      Limits.ClampValue(param)
    end
    -- enable movement if pickup mode is off; controller is within pickup range; 
    -- or control was last used recently and rapidly moved out of pickup range
    if(
      (not MIDI2LR.PICKUP_ENABLED) or
      (math.abs(midi_value - develop_lerp_to_midi(param)) <= MIDI2LR.PICKUP_THRESHOLD) or
      (lastclock + 0.5 > os.clock() and lastparam == param) 
    )
    then
      if MIDI2LR.PICKUP_ENABLED then -- update info to use for detecting fast control changes
        lastclock = os.clock()
        lastparam = param
      end
      local value = midi_lerp_to_develop(param, midi_value)
      MIDI2LR.PARAM_OBSERVER[param] = value
      LrDevelopController.setValue(param, value)
      MIDI2LR.LAST_PARAM = param
    end
  end
end
updateParam = updateParam() --complete closure

-- message processor
local function processMessage(message)
  if type(message) == 'string' then
    -- messages are in the format 'param value'
    local _, _, param, value = message:find( '(%S+)%s(%S+)' )

    if(ACTIONS[param]) then -- perform a one time action
      if(tonumber(value) == MIDI2LR.BUTTON_ON) then ACTIONS[param]() end
    elseif(param:find('Reset') == 1) then -- perform a reset other than those explicitly coded in ACTIONS array
      if(tonumber(value) == MIDI2LR.BUTTON_ON) then Ut.execFOM(LrDevelopController.resetToDefault,param:sub(6)) end
    elseif(param:find('Preset_') == 1) then --apply preset by #
      if(tonumber(value) == MIDI2LR.BUTTON_ON) then ApplyPreset(ProgramPreferences.Presets[tonumber(param:sub(8))]) end
    elseif(TOGGLE_PARAMETERS[param]) then --enable/disable 
      if(tonumber(value) == MIDI2LR.BUTTON_ON) then LrDevelopController.setValue(param,not Ut.execFOM(LrDevelopController.getValue,param)) end -- toggle parameters if button on
    elseif(TOGGLE_PARAMETERS_01[param]) then --enable/disable
      if(tonumber(value) == MIDI2LR.BUTTON_ON) then 
        if Ut.execFOM(LrDevelopController.getValue(param)) == 0 then
          LrDevelopController.setValue(param,1)
        else
          LrDevelopController.setValue(param,0)
        end
      end
    elseif(TOOL_ALIASES[param]) then -- switch to desired tool
      if(tonumber(value) == MIDI2LR.BUTTON_ON) then 
        if(LrDevelopController.getSelectedTool() == TOOL_ALIASES[param]) then -- toggle between the tool/loupe
          Ut.execFOM(LrDevelopController.selectTool,'loupe')
          Profiles.changeProfile('loupe', true)
        else
          Ut.execFOM(LrDevelopController.selectTool,TOOL_ALIASES[param])
          Profiles.changeProfile(param, true)
        end
      end
    elseif(SETTINGS[param]) then
      SETTINGS[param](tonumber(value))
    elseif (param == 'ChangedToDirectory') then
      Profiles.setDirectory(message:sub(message:find(' ',1,true)+1))
    elseif (param == 'ChangedToFile') then
      Profiles.setFile(message:sub(message:find(' ',1,true)+1))
    elseif (param == 'ChangedToFullPath') then
      Profiles.setFullPath(message:sub(message:find(' ',1,true)+1)) --value stops at first space
    else -- otherwise update a develop parameter
      updateParam(param, tonumber(value))
      if ProgramPreferences.ClientShowBezelOnChange then
        LrDialogs.showBezel(Parameters.Names[param][1]..'  '..midi_lerp_to_develop(param, tonumber(value)))
      end
      Profiles.changeProfile(Parameters.Names[param][3])
    end
  end
end

local function startServer(context)
  MIDI2LR.SERVER = LrSocket.bind {
    functionContext = context,
    plugin = _PLUGIN,
    port = MIDI2LR.SEND_PORT,
    mode = 'send',
    onClosed = function( socket ) -- this callback never seems to get called...
      -- MIDI2LR closed connection, allow for reconnection
      -- socket:reconnect()
    end,
    onError = function( socket, err )
      socket:reconnect()
    end,
  }
end

-- Main task
LrTasks.startAsyncTask( function() 
    -- LrMobdebug.on()
    do --save localized file for app
      local LrFileUtils = import 'LrFileUtils'
      local LrLocalization = import 'LrLocalization'
      local LrPathUtils = import 'LrPathUtils'
      local Info = require 'Info'
      local versionmismatch = false
      local datafile = LrPathUtils.child(_PLUGIN.path, 'MenuList.lua')
      if ProgramPreferences.DataStructure == nil then
        versionmismatch = true
      else
        for k,v in pairs(Info.VERSION) do
          versionmismatch = versionmismatch or ProgramPreferences.DataStructure.version[k] ~= v
        end
      end
      if versionmismatch or LrFileUtils.exists(datafile) ~= 'file' or
      ProgramPreferences.DataStructure.language ~= LrLocalization.currentLanguage()
      then
        local serpent = require 'serpent'
        local MenuListPreTrans = require 'MenuListPreTrans'
        local file = io.open(datafile,'w')
        file:write([=[
--[[----------------------------------------------------------------------------

	This file was auto-generated by MIDI2LR and contains the translated menus used
  by the app and the plugin. Edits to this file will be lost any time MIDI2LR
  is updated or the language used by Lightroom changes. Edit MenuListPreTrans.lua
  if you want to have persistent changes to the translations or menu structure.
 
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
local MenuList = ]=],serpent.block(MenuListPreTrans.MenuList, {comment = false}), [==[
      
local MenuListHashed = {}
for i,v in ipairs(MenuList) do
  MenuListHashed[v[1]]={i*100,v[2],v[3],v[4]}
end

return {
  MenuList = MenuList,
  MenuListHashed = MenuListHashed,
}]==])
      file:close()
      ProgramPreferences.DataStructure = {version={},language = ''} --empty it out, fill it up
      ProgramPreferences.DataStructure.language = LrLocalization.currentLanguage()
      for k,v in pairs(Info.VERSION) do
        ProgramPreferences.DataStructure.version[k] = v
      end
    end
  end --save localized file for app
  
  LrFunctionContext.callWithContext( 'socket_remote', function( context )
      -- LrMobdebug.on()
      local client = LrSocket.bind {
        functionContext = context,
        plugin = _PLUGIN,
        port = MIDI2LR.RECEIVE_PORT,
        mode = 'receive',
        onMessage = function(socket, message)
          processMessage(message)
        end,
        onClosed = function( socket )
          -- MIDI2LR closed connection, allow for reconnection
          socket:reconnect()

          -- calling SERVER:reconnect causes LR to hang for some reason...
          MIDI2LR.SERVER:close()
          startServer(context)
        end,
        onError = function(socket, err)
          if err == 'timeout' then -- reconnect if timed out
            socket:reconnect()
          end
        end
      }

      startServer(context)

      math.randomseed(os.time())
      currentLoadVersion = math.random() --in case currentLoadVersion gets initialized to 0 each load
      local loadVersion = currentLoadVersion  

      -- add an observer for develop param changes--needs to occur in develop module
      while (loadVersion == currentLoadVersion) and (LrApplicationView.getCurrentModuleName() ~= 'develop') do
        LrTasks.sleep ( .29 )
        Profiles.checkProfile()
      end --sleep away until ended or until develop module activated
      LrDevelopController.revealAdjustedControls( true ) -- reveal affected parameter in panel track
      LrDevelopController.addAdjustmentChangeObserver(
        context, 
        MIDI2LR.PARAM_OBSERVER, 
        function ( observer ) 
          if LrApplicationView.getCurrentModuleName() ~= 'develop' then return end
          for _,param in ipairs(Parameters.Order) do
            local lrvalue = LrDevelopController.getValue(param)
            if observer[param] ~= lrvalue and type(lrvalue) == 'number' then
              MIDI2LR.SERVER:send(string.format('%s %g\n', param, develop_lerp_to_midi(param)))
              observer[param] = lrvalue
              MIDI2LR.LAST_PARAM = param
            end
          end
        end 
      )

      while (loadVersion == currentLoadVersion)  do --detect halt or reload
        LrTasks.sleep( .29 )
        Profiles.checkProfile()
      end

      client:close()
      MIDI2LR.SERVER:close()
    end )
end )

LrTasks.startAsyncTask( function()
    if(WIN_ENV) then
      LrShell.openFilesInApp({_PLUGIN.path..'/Info.lua'}, _PLUGIN.path..'/MIDI2LR.exe')
    else
      LrShell.openFilesInApp({_PLUGIN.path..'/Info.lua'}, _PLUGIN.path..'/MIDI2LR.app') 
    end
  end
)
