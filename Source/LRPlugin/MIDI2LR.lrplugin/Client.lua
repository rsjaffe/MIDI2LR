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

local LrTasks = import 'LrTasks'
-- Main task
LrTasks.startAsyncTask( 
  function() 
    -- LrMobdebug.on()
-------------preferences
    local Preferences     = require 'Preferences'
    Preferences.Load() 
-------------end preferences section    


    local LrFunctionContext   = import 'LrFunctionContext'
    local LrPathUtils         = import 'LrPathUtils'

    do --save localized file for app
      local LrFileUtils    = import 'LrFileUtils'
      local LrLocalization = import 'LrLocalization'
      local Info           = require 'Info'
      local appdatafile     = LrPathUtils.child(_PLUGIN.path, 'MenuList.lua')
      local plugindatafile  = LrPathUtils.child(_PLUGIN.path, 'ParamList.lua')
      local versionmismatch = false

      if ProgramPreferences.DataStructure == nil then
        versionmismatch = true
      else
        for k,v in pairs(Info.VERSION) do
          versionmismatch = versionmismatch or ProgramPreferences.DataStructure.version[k] ~= v
        end
      end

      if 
      versionmismatch or 
      LrFileUtils.exists(appdatafile) ~= 'file' or
      LrFileUtils.exists(plugindatafile) ~= 'file' or
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

    --delay loading most modules until after data structure refreshed
    local CU              = require 'ClientUtilities'
    local Keys            = require 'Keys'
    local Limits          = require 'Limits'
    local ParamList       = require 'ParamList'
    local Profiles        = require 'Profiles'
    local Ut              = require 'Utilities'
    local LrApplication       = import 'LrApplication'
    local LrApplicationView   = import 'LrApplicationView'
    local LrDevelopController = import 'LrDevelopController'
    local LrDialogs           = import 'LrDialogs'
    local LrSelection         = import 'LrSelection'
    local LrStringUtils       = import 'LrStringUtils'
    local LrUndo              = import 'LrUndo'
    --global variables
    MIDI2LR = {PARAM_OBSERVER = {}, SERVER = {}, RUNNING = true} --non-local but in MIDI2LR namespace
    --local variables
    local LastParam           = ''
    local UpdateParamPickup, UpdateParamNoPickup, UpdateParam
    --local constants--may edit these to change program behaviors
    local BUTTON_ON        = 0.99 -- sending 1.0, but use > BUTTON_ON in case of rounding error
    local PICKUP_THRESHOLD = 0.03 -- roughly equivalent to 4/127
    local RECEIVE_PORT     = 58763
    local SEND_PORT        = 58764

    local ACTIONS = {
      --   AddToQuickCollection     = CU.addToCollection('quick',LrApplication.activeCatalog():getTargetPhotos()),
      --   AddToTargetCollection    = CU.addToCollection('target',LrApplication.activeCatalog():getTargetPhotos()),
      AdjustmentBrush          = CU.fToggleTool('localized'),
      AutoLateralCA            = CU.fToggle01('AutoLateralCA'),
      ConvertToGrayscale       = CU.fToggleTF('ConvertToGrayscale'),
      CopySettings             = CU.CopySettings,
      CropOverlay              = CU.fToggleTool('crop'),
      DecreaseRating           = LrSelection.decreaseRating,
      DecrementLastDevelopParameter = function() Ut.execFOM(LrDevelopController.decrement,LastParam) end,
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
      EnableToneCurve                        = CU.fToggleTF('EnableToneCurve'),
      FullRefresh                            = CU.FullRefresh,
      GraduatedFilter                        = CU.fToggleTool('gradient'),
      IncreaseRating                         = LrSelection.increaseRating,
      IncrementLastDevelopParameter = function() Ut.execFOM(LrDevelopController.increment,LastParam) end,
      Key1  = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(1))) end,
      Key2  = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(2))) end,
      Key3  = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(3))) end,
      Key4  = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(4))) end,
      Key5  = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(5))) end,
      Key6  = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(6))) end,
      Key7  = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(7))) end,
      Key8  = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(8))) end,
      Key9  = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(9))) end,
      Key10 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(10))) end,
      Key11 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(11))) end,
      Key12 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(12))) end,
      Key13 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(13))) end,
      Key14 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(14))) end,
      Key15 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(15))) end,
      Key16 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(16))) end,
      Key17 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(17))) end,
      Key18 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(18))) end,
      Key19 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(19))) end,
      Key20 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(20))) end,
      Key21 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(21))) end,
      Key22 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(22))) end,
      Key23 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(23))) end,
      Key24 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(24))) end,
      Key25 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(25))) end,
      Key26 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(26))) end,
      Key27 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(27))) end,
      Key28 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(28))) end,
      Key29 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(29))) end,
      Key30 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(30))) end,
      Key31 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(31))) end,
      Key32 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(32))) end,
      Key33 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(33))) end,
      Key34 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(34))) end,
      Key35 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(35))) end,
      Key36 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(36))) end,
      Key37 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(37))) end,
      Key38 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(38))) end,
      Key39 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(39))) end,
      Key40 = function() MIDI2LR.SERVER:send(string.format('SendKey %s\n', Keys.GetKey(40))) end,
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
      Preset_21                = CU.fApplyPreset(21),
      Preset_22                = CU.fApplyPreset(22),
      Preset_23                = CU.fApplyPreset(23),
      Preset_24                = CU.fApplyPreset(24),
      Preset_25                = CU.fApplyPreset(25),
      Preset_26                = CU.fApplyPreset(26),
      Preset_27                = CU.fApplyPreset(27),
      Preset_28                = CU.fApplyPreset(28),
      Preset_29                = CU.fApplyPreset(29),
      Preset_30                = CU.fApplyPreset(30),
      Preset_31                = CU.fApplyPreset(31),
      Preset_32                = CU.fApplyPreset(32),
      Preset_33                = CU.fApplyPreset(33),
      Preset_34                = CU.fApplyPreset(34),
      Preset_35                = CU.fApplyPreset(35),
      Preset_36                = CU.fApplyPreset(36),
      Preset_37                = CU.fApplyPreset(37),
      Preset_38                = CU.fApplyPreset(38),
      Preset_39                = CU.fApplyPreset(39),
      Preset_40                = CU.fApplyPreset(40),
      Prev                     = LrSelection.previousPhoto,
      Profile_Adobe_Standard          = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Adobe Standard'),
      Profile_Camera_Clear            = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Clear'),
      Profile_Camera_Darker_Skin_Tone = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Darker Skin Tone'),
      Profile_Camera_Deep             = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Deep'),
      Profile_Camera_Faithful         = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Faithful'),
      Profile_Camera_Flat             = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Flat'),
      Profile_Camera_Landscape        = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Landscape'),
      Profile_Camera_Light            = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Light'),
      Profile_Camera_Lighter_Skin_Tone= Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Lighter Skin Tone'),
      Profile_Camera_Monochrome       = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Monochrome'),
      Profile_Camera_Monotone         = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Monotone'),
      Profile_Camera_Muted            = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Muted'),
      Profile_Camera_Natural          = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Natural'),
      Profile_Camera_Neutral          = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Neutral'),
      Profile_Camera_Portrait         = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Portrait'),
      Profile_Camera_Positive_Film    = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Positive Film'),
      Profile_Camera_Standard         = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Standard'),
      Profile_Camera_Vivid            = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Vivid'),
      Profile_Camera_Vivid_Blue       = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Vivid Blue'),
      Profile_Camera_Vivid_Green      = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Vivid Green'),
      Profile_Camera_Vivid_Red        = Ut.wrapFOM(LrDevelopController.setValue,'CameraProfile','Camera Vivid Red'),
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
      ResetLast                = function() Ut.execFOM(LrDevelopController.resetToDefault,LastParam) end,
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
      Pickup             = function(enabled) 
        if tonumber(enabled) == 1 then -- state machine
          UpdateParam = UpdateParamPickup
        else
          UpdateParam = UpdateParamNoPickup
        end
      end, 
    }

    local function MIDIValueToLRValue(param, midi_value)
      -- map midi range to develop parameter range
      -- expects midi_value 0.0-1.0, doesn't protect against out-of-range
      local min,max = Limits.GetMinMax(param)
      return midi_value * (max-min) + min
    end

    local function LRValueToMIDIValue(param)
      -- map develop parameter range to midi range
      local min,max = Limits.GetMinMax(param)
      local retval = (LrDevelopController.getValue(param)-min)/(max-min)
      if retval > 1 then return 1 end
      if retval < 0 then return 0 end
      return retval
    end

    --called within LrRecursionGuard for setting
    function UpdateParamPickup() --closure
      local paramlastmoved = {}
      local lastfullrefresh = 0
      return function(param, midi_value)
        local value
        if LrApplicationView.getCurrentModuleName() ~= 'develop' then
          LrApplicationView.switchToModule('develop')
        end
        if Limits.Parameters[param] then
          Limits.ClampValue(param)
        end
        if((math.abs(midi_value - LRValueToMIDIValue(param)) <= PICKUP_THRESHOLD) or (paramlastmoved[param] ~= nil and paramlastmoved[param] + 0.5 > os.clock())) then -- pickup succeeded
          paramlastmoved[param] = os.clock()
          value = MIDIValueToLRValue(param, midi_value)
          MIDI2LR.PARAM_OBSERVER[param] = value
          LrDevelopController.setValue(param, value)
          LastParam = param
          if ProgramPreferences.ClientShowBezelOnChange then
            LrDialogs.showBezel(param..'  '..LrStringUtils.numberToStringWithSeparators(value,Ut.precision(value)))
          end
          if ParamList.ProfileMap[param] then
            Profiles.changeProfile(ParamList.ProfileMap[param])
          end
        else --failed pickup
          if ProgramPreferences.ClientShowBezelOnChange then -- failed pickup. do I display bezel?
            value = MIDIValueToLRValue(param, midi_value)
            local actualvalue = LrDevelopController.getValue(param)
            local precision = Ut.precision(value)
            LrDialogs.showBezel(param..'  '..LrStringUtils.numberToStringWithSeparators(value,precision)..'  '..LrStringUtils.numberToStringWithSeparators(actualvalue,precision))
          end
          if lastfullrefresh + 1 < os.clock() then --try refreshing controller once a second
            CU.FullRefresh()
            lastfullrefresh = os.clock()
          end
        end -- end of if pickup/elseif bezel group
      end -- end of returned function
    end
    UpdateParamPickup = UpdateParamPickup() --complete closure
    --called within LrRecursionGuard for setting
    function UpdateParamNoPickup(param, midi_value) 
      local value
      if LrApplicationView.getCurrentModuleName() ~= 'develop' then
        LrApplicationView.switchToModule('develop')
      end
      value = MIDIValueToLRValue(param, midi_value)
      MIDI2LR.PARAM_OBSERVER[param] = value
      LrDevelopController.setValue(param, value)
      LastParam = param
      if ProgramPreferences.ClientShowBezelOnChange then
        LrDialogs.showBezel(param..'  '..LrStringUtils.numberToStringWithSeparators(value,Ut.precision(value)))
      end
      if ParamList.ProfileMap[param] then
        Profiles.changeProfile(ParamList.ProfileMap[param])
      end
    end
    UpdateParam = UpdateParamPickup --initial state


    LrFunctionContext.callWithContext( 
      'socket_remote', 
      function( context )
        -- LrMobdebug.on()
        local LrRecursionGuard    = import 'LrRecursionGuard'
        local LrShell             = import 'LrShell'
        local LrSocket            = import 'LrSocket'
        local guardreading = LrRecursionGuard('reading')
        local guardsetting = LrRecursionGuard('setting')
        local CurrentObserver
        --call following within guard for reading
        local function AdjustmentChangeObserver(observer)
          if LrApplicationView.getCurrentModuleName() == 'develop' then
            for _,param in ipairs(ParamList.SendToMidi) do
              local lrvalue = LrDevelopController.getValue(param)
              if observer[param] ~= lrvalue and type(lrvalue) == 'number' then
                MIDI2LR.SERVER:send(string.format('%s %g\n', param, LRValueToMIDIValue(param)))
                observer[param] = lrvalue
                LastParam = param
              end
            end
          end
        end
        local function InactiveObserver() end
        CurrentObserver = AdjustmentChangeObserver -- will change when detect loss of MIDI controller

        -- wrapped in function so can be called when connection lost
        local function startServer(context)
          MIDI2LR.SERVER = LrSocket.bind {
            functionContext = context,
            plugin = _PLUGIN,
            port = SEND_PORT,
            mode = 'send',
            onClosed = function( ) -- this callback never seems to get called...
              -- MIDI2LR closed connection, allow for reconnection
              -- socket:reconnect()
            end,
            onError = function( socket )
              socket:reconnect()
            end,
          }
        end

        local client = LrSocket.bind {
          functionContext = context,
          plugin = _PLUGIN,
          port = RECEIVE_PORT,
          mode = 'receive',
          onMessage = function(_, message) --message processor
            if type(message) == 'string' then
              local split = message:find(' ',1,true)
              local param = message:sub(1,split-1)
              local value = message:sub(split+1)
              if(ACTIONS[param]) then -- perform a one time action
                if(tonumber(value) > BUTTON_ON) then ACTIONS[param]() end
              elseif(param:find('Reset') == 1) then -- perform a reset other than those explicitly coded in ACTIONS array
                if(tonumber(value) > BUTTON_ON) then Ut.execFOM(LrDevelopController.resetToDefault,param:sub(6)) end
              elseif(SETTINGS[param]) then -- do something requiring the transmitted value to be known
                SETTINGS[param](value)
              else -- otherwise update a develop parameter
                guardsetting:performWithGuard(UpdateParam,param,tonumber(value))
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

        -- add an observer for develop param changes--needs to occur in develop module
        -- will drop out of loop if loadversion changes or if in develop module with selected photo
        while  MIDI2LR.RUNNING and ((LrApplicationView.getCurrentModuleName() ~= 'develop') or (LrApplication.activeCatalog():getTargetPhoto() == nil)) do
          LrTasks.sleep ( .29 )
          guardsetting:performWithGuard(Profiles.checkProfile)
        end --sleep away until ended or until develop module activated
        if MIDI2LR.RUNNING then --didn't drop out of loop because of program termination
          LrDevelopController.revealAdjustedControls( true ) -- reveal affected parameter in panel track
          if ProgramPreferences.TrackingDelay ~= nil then
            LrDevelopController.setTrackingDelay(ProgramPreferences.TrackingDelay)
          end
          LrDevelopController.addAdjustmentChangeObserver(
            context, 
            MIDI2LR.PARAM_OBSERVER, 
            function ( observer ) 
              guardreading:performWithGuard(CurrentObserver,observer)
            end 
          )
          while MIDI2LR.RUNNING do --detect halt or reload
            LrTasks.sleep( .29 )
            guardsetting:performWithGuard(Profiles.checkProfile)
          end
        end
        client:close()
        MIDI2LR.SERVER:close()
      end 
    )
  end 
)

