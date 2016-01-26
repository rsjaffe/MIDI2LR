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
local CU              = require 'ClientUtilities'
local Limits          = require 'Limits'
local MenuList        = require 'MenuList'
local Parameters      = require 'Parameters'
local Preferences     = require 'Preferences'
local Profiles        = require 'Profiles'
local Ut              = require 'Utilities'
local LrApplication       = import 'LrApplication'
local LrApplicationView   = import 'LrApplicationView'
local LrDevelopController = import 'LrDevelopController'
local LrDialogs           = import 'LrDialogs'
local LrSelection         = import 'LrSelection'
local LrStringUtils       = import 'LrStringUtils'
local LrTasks             = import 'LrTasks'
local LrUndo              = import 'LrUndo'
-- signal for halt plugin if reloaded--LR doesn't kill main loop otherwise
math.randomseed(os.time())
currentLoadVersion = rawget (_G, 'currentLoadVersion') or math.random()  
currentLoadVersion = currentLoadVersion + 1 

MIDI2LR = {RECEIVE_PORT = 58763, SEND_PORT = 58764, PICKUP_THRESHOLD = 4, CONTROL_MAX = 127, BUTTON_ON = 127; --constants
  LAST_PARAM = '', PARAM_OBSERVER = {}, PICKUP_ENABLED = true, SERVER = {} } --non-local but in MIDI2LR namespace
-------------preferences
Preferences.Load() 
-------------end preferences section
local ACTIONS = {
  AdjustmentBrush          = CU.fToggleTool('localized'),
  AutoLateralCA            = CU.fToggle01('AutoLateralCA'),
  ConvertToGrayscale       = CU.fToggleTF('ConvertToGrayscale'),
  CopySettings             = CU.CopySettings,
  CropOverlay              = CU.fToggleTool('crop'),
  DecreaseRating           = LrSelection.decreaseRating,
  DecrementLastDevelopParameter = function() Ut.execFOM(LrDevelopController.decrement,MIDI2LR.LAST_PARAM) end,
  EnableCalibration                      = CU.fToggleTF('EnableCalibration'),
  EnableCircularGradientBasedCorrections = CU.fToggleTF('EnableCircularGradientBasedCorrections'),
  EnableColorAdjustments                 = CU.fToggleTF('EnableColorAdjustments'),
  EnableDetail                           = CU.fToggleTF('EnableDetail'),
  EnableEffects                          = CU.fToggleTF('EnableEffects'),
  EnableGradientBasedCorrections         = CU.fToggleTF('EnableGradientBasedCorrections'),
  EnableGrayscaleMix                     = CU.fToggleTF('EnableGrayscaleMix'),
  EnableLensCorrections                  = CU.fToggleTF('EnableLensCorrections'),
  EnablePaintBasedCorrections            = CU.fToggleTF('EnablePaintBasedCorrections'),
  EnableRedEye                           = CU.fToggleTF('EnableRedEye'),
  EnableRetouch                          = CU.fToggleTF('EnableRetouch'),
  EnableSplitToning                      = CU.fToggleTF('EnableSplitToning'),
  GraduatedFilter                        = CU.fToggleTool('gradient'),
  IncreaseRating                         = LrSelection.increaseRating,
  IncrementLastDevelopParameter = function() Ut.execFOM(LrDevelopController.increment,MIDI2LR.LAST_PARAM) end,
  LensProfileEnable        = CU.fToggle01('LensProfileEnable'),
  Loupe                    = CU.fToggleTool('loupe'),
  Next                     = LrSelection.nextPhoto,
  PasteSelectedSettings    = CU.PasteSelectedSettings,
  PasteSettings            = CU.PasteSettings,
  Pick                     = LrSelection.flagAsPick,
  Preset_1                 = CU.fApplyPreset(1),
  Preset_2                 = CU.fApplyPreset(2),
  Preset_3                 = CU.fApplyPreset(3),
  Preset_4                 = CU.fApplyPreset(4),
  Preset_5                 = CU.fApplyPreset(5),
  Preset_6                 = CU.fApplyPreset(6),
  Preset_7                 = CU.fApplyPreset(7),
  Preset_8                 = CU.fApplyPreset(8),
  Preset_9                 = CU.fApplyPreset(9),
  Preset_10                = CU.fApplyPreset(10),
  Preset_11                = CU.fApplyPreset(11),
  Preset_12                = CU.fApplyPreset(12),
  Preset_13                = CU.fApplyPreset(13),
  Preset_14                = CU.fApplyPreset(14),
  Preset_15                = CU.fApplyPreset(15),
  Preset_16                = CU.fApplyPreset(16),
  Preset_17                = CU.fApplyPreset(17),
  Preset_18                = CU.fApplyPreset(18),
  Preset_19                = CU.fApplyPreset(19),
  Preset_20                = CU.fApplyPreset(20),
  Prev                     = LrSelection.previousPhoto,
  Profile_Adobe_Standard   = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Adobe Standard'),
  Profile_Camera_Clear     = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Clear'),
  Profile_Camera_Deep      = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Deep'),
  Profile_Camera_Landscape = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Landscape'),
  Profile_Camera_Light     = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Light'),
  Profile_Camera_Neutral   = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Neutral'),
  Profile_Camera_Portrait  = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Portrait'),
  Profile_Camera_Standard  = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Standard'),
  Profile_Camera_Vivid     = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Vivid'),
  profile1                 = function() Profiles.changeProfile('profile1', true) end,
  profile2                 = function() Profiles.changeProfile('profile2', true) end,
  profile3                 = function() Profiles.changeProfile('profile3', true) end,
  profile4                 = function() Profiles.changeProfile('profile4', true) end,
  profile5                 = function() Profiles.changeProfile('profile5', true) end,
  profile6                 = function() Profiles.changeProfile('profile6', true) end,
  profile7                 = function() Profiles.changeProfile('profile7', true) end,
  profile8                 = function() Profiles.changeProfile('profile8', true) end,
  profile9                 = function() Profiles.changeProfile('profile9', true) end,
  profile10                = function() Profiles.changeProfile('profile10', true) end,
  RadialFilter             = CU.fToggleTool('circularGradient'),
  RedEye                   = CU.fToggleTool('redeye'),
  Redo                     = LrUndo.redo,
  Reject                   = LrSelection.flagAsReject,
  RemoveFlag               = LrSelection.removeFlag,
  ResetAll                 = Ut.wrapFOM(LrDevelopController.resetAllDevelopAdjustments),
  ResetBrushing            = Ut.wrapFOM(LrDevelopController.resetBrushing),
  ResetCircGrad            = Ut.wrapFOM(LrDevelopController.resetCircularGradient),
  ResetCrop                = Ut.wrapFOM(LrDevelopController.resetCrop),
  ResetGradient            = Ut.wrapFOM(LrDevelopController.resetGradient),
  ResetLast                = function() Ut.execFOM(LrDevelopController.resetToDefault,MIDI2LR.LAST_PARAM) end,
  ResetRedeye              = Ut.wrapFOM(LrDevelopController.resetRedeye),
  ResetSpotRem             = Ut.wrapFOM(LrDevelopController.resetSpotRemoval),
  RevealPanelAdjust        = CU.fChangePanel('adjustPanel'),
  RevealPanelCalibrate     = CU.fChangePanel('calibratePanel'),
  RevealPanelDetail        = CU.fChangePanel('detailPanel'), 
  RevealPanelEffects       = CU.fChangePanel('effectsPanel'),
  RevealPanelLens          = CU.fChangePanel('lensCorrectionsPanel'),
  RevealPanelMixer         = CU.fChangePanel('mixerPanel'),
  RevealPanelSplit         = CU.fChangePanel('splitToningPanel'),
  RevealPanelTone          = CU.fChangePanel('tonePanel'),
  Select1Left              = function() LrSelection.extendSelection('left',1) end,
  Select1Right             = function() LrSelection.extendSelection('right',1) end,
  SetRating0               = function() LrSelection.setRating(0) end,
  SetRating1               = function() LrSelection.setRating(1) end,
  SetRating2               = function() LrSelection.setRating(2) end,
  SetRating3               = function() LrSelection.setRating(3) end,
  SetRating4               = function() LrSelection.setRating(4) end,
  SetRating5               = function() LrSelection.setRating(5) end,
  ShoScndVwloupe           = function() LrApplicationView.showSecondaryView('loupe') end,
  ShoScndVwlive_loupe      = function() LrApplicationView.showSecondaryView('live_loupe') end,
  ShoScndVwlocked_loupe    = function() LrApplicationView.showSecondaryView('locked_loupe') end,
  ShoScndVwgrid            = function() LrApplicationView.showSecondaryView('grid') end,
  ShoScndVwcompare         = function() LrApplicationView.showSecondaryView('compare') end,
  ShoScndVwsurvey          = function() LrApplicationView.showSecondaryView('survey') end,
  ShoScndVwslideshow       = function() LrApplicationView.showSecondaryView('slideshow') end,  
  ShoVwloupe               = function() LrApplicationView.showView('loupe') end,
  ShoVwgrid                = function() LrApplicationView.showView('grid') end,
  ShoVwcompare             = function() LrApplicationView.showView('compare') end,
  ShoVwsurvey              = function() LrApplicationView.showView('survey') end,
  ShoVwpeople              = function() LrApplicationView.showView('people') end,
  ShoVwdevelop_loupe              = function() LrApplicationView.showView('develop_loupe') end,
  ShoVwdevelop_before_after_horiz = function() LrApplicationView.showView('develop_before_after_horiz') end,
  ShoVwdevelop_before_after_vert  = function() LrApplicationView.showView('develop_before_after_vert') end,
  ShoVwdevelop_before             = function() LrApplicationView.showView('develop_before') end,
  SpotRemoval              = CU.fToggleTool('dust'),
  SwToMlibrary             = CU.fChangeModule('library'),
  SwToMdevelop             = CU.fChangeModule('develop'),
  SwToMmap                 = CU.fChangeModule('map'),
  SwToMbook                = CU.fChangeModule('book'),
  SwToMslideshow           = CU.fChangeModule('slideshow'),
  SwToMprint               = CU.fChangeModule('print'),
  SwToMweb                 = CU.fChangeModule('web'),
  ToggleBlue               = LrSelection.toggleBlueLabel,
  ToggleGreen              = LrSelection.toggleGreenLabel,
  TogglePurple             = LrSelection.togglePurpleLabel,
  ToggleRed                = LrSelection.toggleRedLabel,
  ToggleScreenTwo          = LrApplicationView.toggleSecondaryDisplay,
  ToggleYellow             = LrSelection.toggleYellowLabel,
  ToggleZoomOffOn          = LrApplicationView.toggleZoom,
  Undo                     = LrUndo.undo,
  UprightAuto              = Ut.wrapFOM(LrDevelopController.setValue,'PerspectiveUpright',1),
  UprightFull              = Ut.wrapFOM(LrDevelopController.setValue,'PerspectiveUpright',2),
  UprightLevel             = Ut.wrapFOM(LrDevelopController.setValue,'PerspectiveUpright',3),
  UprightOff               = Ut.wrapFOM(LrDevelopController.setValue,'PerspectiveUpright',0),
  UprightVertical          = Ut.wrapFOM(LrDevelopController.setValue,'PerspectiveUpright',4),
  VirtualCopy              = function() LrApplication.activeCatalog():createVirtualCopies() end,
  WhiteBalanceAs_Shot      = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','As Shot'),
  WhiteBalanceAuto         = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','Auto'),
  WhiteBalanceCloudy       = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','Cloudy'),
  WhiteBalanceDaylight     = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','Daylight'),
  WhiteBalanceFlash        = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','Flash'),  
  WhiteBalanceFluorescent  = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','Fluorescent'),
  WhiteBalanceShade        = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','Shade'),
  WhiteBalanceTungsten     = Ut.wrapFOM(LrDevelopController.setValue,'WhiteBalance','Tungsten'),
  ZoomInLargeStep          = LrApplicationView.zoomIn,
  ZoomInSmallStep          = LrApplicationView.zoomInSome,
  ZoomOutLargeStep         = LrApplicationView.zoomOut,
  ZoomOutSmallStep         = LrApplicationView.zoomOutSome,
}

local SETTINGS = {
  ChangedToDirectory = function(value) Profiles.setDirectory(value) end,
  ChangedToFile      = function(value) Profiles.setFile(value) end,
  ChangedToFullPath  = function(value) Profiles.setFullPath(value) end,
  Pickup             = function(enabled) MIDI2LR.PICKUP_ENABLED = (tonumber(enabled) == 1) end, 
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
    local value
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
      value = midi_lerp_to_develop(param, midi_value)
      MIDI2LR.PARAM_OBSERVER[param] = value
      LrDevelopController.setValue(param, value)
      MIDI2LR.LAST_PARAM = param
    end
    if ProgramPreferences.ClientShowBezelOnChange then
      if value == nil then -- didn't do an update--pickup failed, so show target value as well
        value = midi_lerp_to_develop(param, midi_value)
        local actualvalue = LrDevelopController.getValue(param)
        local precision = Ut.precision(value)
        LrDialogs.showBezel(MenuList.MenuListHashed[param][2]..'  '..LrStringUtils.numberToStringWithSeparators(value,precision)..'  '..LrStringUtils.numberToStringWithSeparators(actualvalue,precision))
      else
        LrDialogs.showBezel(MenuList.MenuListHashed[param][2]..'  '..LrStringUtils.numberToStringWithSeparators(value,Ut.precision(value)))
      end
    end
    Profiles.changeProfile(Parameters.Names[param][3])
  end
end
updateParam = updateParam() --complete closure

-- Main task
LrTasks.startAsyncTask( function() 
    -- LrMobdebug.on()
    local LrFunctionContext   = import 'LrFunctionContext'
    local LrPathUtils = import 'LrPathUtils'
    do --save localized file for app
      local LrFileUtils    = import 'LrFileUtils'
      local LrLocalization = import 'LrLocalization'
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
        require 'Database'
        ProgramPreferences.DataStructure = {version={},language = LrLocalization.currentLanguage()}
        for k,v in pairs(Info.VERSION) do
          ProgramPreferences.DataStructure.version[k] = v
        end
        Preferences.Save() --ensure that new version/language info saved
      end
    end --save localized file for app

    LrFunctionContext.callWithContext( 'socket_remote', function( context )
      local LrRecursionGuard    = import 'LrRecursionGuard'
      local LrShell             = import 'LrShell'
      local LrSocket            = import 'LrSocket'
      local guard = LrRecursionGuard('AdjustmentChangeObserver')
      --call following within guard
      local function AdjustmentChangeObserver(observer)
        for _,param in ipairs(Parameters.Iterate) do
          local lrvalue = LrDevelopController.getValue(param)
          if observer[param] ~= lrvalue and type(lrvalue) == 'number' then
            MIDI2LR.SERVER:send(string.format('%s %g\n', param, develop_lerp_to_midi(param)))
            observer[param] = lrvalue
            MIDI2LR.LAST_PARAM = param
          end
        end
      end
      -- wrapped in function so can be called when connection lost
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
      -- LrMobdebug.on()
      local client = LrSocket.bind {
        functionContext = context,
        plugin = _PLUGIN,
        port = MIDI2LR.RECEIVE_PORT,
        mode = 'receive',
        onMessage = function(socket, message) --message processor
          if type(message) == 'string' then
            local split = message:find(' ',1,true)
            local param = message:sub(1,split-1)
            local value = message:sub(split+1)
            if(ACTIONS[param]) then -- perform a one time action
              if(tonumber(value) == MIDI2LR.BUTTON_ON) then ACTIONS[param]() end
            elseif(param:find('Reset') == 1) then -- perform a reset other than those explicitly coded in ACTIONS array
              if(tonumber(value) == MIDI2LR.BUTTON_ON) then Ut.execFOM(LrDevelopController.resetToDefault,param:sub(6)) end
            elseif(SETTINGS[param]) then -- do something requiring the transmitted value to be known
              SETTINGS[param](value)
            else -- otherwise update a develop parameter
              updateParam(param, tonumber(value))
            end
          end
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

      if(WIN_ENV) then
        LrShell.openFilesInApp({LrPathUtils.child(_PLUGIN.path, 'Info.lua')}, LrPathUtils.child(_PLUGIN.path, 'MIDI2LR.exe'))
      else
        LrShell.openFilesInApp({LrPathUtils.child(_PLUGIN.path, 'Info.lua')}, LrPathUtils.child(_PLUGIN.path, 'MIDI2LR.app')) 
      end

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
          if LrApplicationView.getCurrentModuleName() == 'develop' then
            guard:performWithGuard(AdjustmentChangeObserver,observer)
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

