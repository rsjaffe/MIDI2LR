--[[----------------------------------------------------------------------------

Client.lua

Receives and processes commands from MIDI2LR
Sends parameters to MIDI2LR

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
--[[-----------debug section, enable by adding - to beginning this line
local LrMobdebug = import 'LrMobdebug'
LrMobdebug.start()
--]]-----------end debug section

local LrTasks = import 'LrTasks'
-- Main task
LrTasks.startAsyncTask(
  function()
    --[[-----------debug section, enable by adding - to beginning this line
    LrMobdebug.on()
    --]]-----------end debug section
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
    local DebugInfo       = require 'DebugInfo'
    local Info            = require 'Info'
    local Keys            = require 'Keys'
    local Limits          = require 'Limits'
    local LocalPresets    = require 'LocalPresets'
    local ParamList       = require 'ParamList'
    local Profiles        = require 'Profiles'
    local Ut              = require 'Utilities'
    local Virtual         = require 'Virtual'
    local LrApplication       = import 'LrApplication'
    local LrApplicationView   = import 'LrApplicationView'
    local LrDevelopController = import 'LrDevelopController'
    local LrDialogs           = import 'LrDialogs'
    local LrSelection         = import 'LrSelection'
    local LrStringUtils       = import 'LrStringUtils'
    local LrUndo              = import 'LrUndo'
    --global variables
    MIDI2LR = {PARAM_OBSERVER = {}, SERVER = {}, CLIENT = {}, RUNNING = true} --non-local but in MIDI2LR namespace
    --local variables
    local LastParam           = ''
    local UpdateParamPickup, UpdateParamNoPickup, UpdateParam
    --local constants--may edit these to change program behaviors
    local BUTTON_ON        = 0.40 -- sending 1.0, but use > BUTTON_ON because of note keypressess not hitting 100%
    local PICKUP_THRESHOLD = 0.03 -- roughly equivalent to 4/127
    local RECEIVE_PORT     = 58763
    local SEND_PORT        = 58764

    local ACTIONS = {
      AdjustmentBrush          = CU.fToggleTool('localized'),
      AutoLateralCA            = CU.fToggle01('AutoLateralCA'),
      AutoTone                 = function() CU.fChangePanel('tonePanel'); CU.ApplySettings({AutoTone = true}); CU.FullRefresh(); end,
      CropConstrainToWarp      = CU.fToggle01('CropConstrainToWarp'),
      ConvertToGrayscale       = CU.fToggleTF('ConvertToGrayscale'),
      CopySettings             = CU.CopySettings,
      CropOverlay              = CU.fToggleTool('crop'),
      CycleMaskOverlayColor    = CU.SimulateKeys('4o'),
      DecreaseRating           = LrSelection.decreaseRating,
      DecrementLastDevelopParameter          = function() Ut.execFOM(LrDevelopController.decrement,LastParam) end,
      EnableCalibration                      = CU.fToggleTF('EnableCalibration'),
      EnableCircularGradientBasedCorrections = CU.fToggleTF('EnableCircularGradientBasedCorrections'),
      EnableColorAdjustments                 = CU.fToggleTF('EnableColorAdjustments'),
      EnableDetail                           = CU.fToggleTF('EnableDetail'),
      EnableEffects                          = CU.fToggleTF('EnableEffects'),
      EnableGradientBasedCorrections         = CU.fToggleTF('EnableGradientBasedCorrections'),
      EnableGrayscaleMix                     = CU.fToggleTF('EnableGrayscaleMix'),
      EnableLensCorrections                  = CU.fToggleTF('EnableLensCorrections'),
      EnableTransform                        = CU.fToggleTF('EnableTransform'),
      EnablePaintBasedCorrections            = CU.fToggleTF('EnablePaintBasedCorrections'),
      EnableRedEye                           = CU.fToggleTF('EnableRedEye'),
      EnableRetouch                          = CU.fToggleTF('EnableRetouch'),
      EnableSplitToning                      = CU.fToggleTF('EnableSplitToning'),
      EnableToneCurve                        = CU.fToggleTF('EnableToneCurve'),
      Filter_1                               = CU.fApplyFilter(1),
      Filter_2                               = CU.fApplyFilter(2),
      Filter_3                               = CU.fApplyFilter(3),
      Filter_4                               = CU.fApplyFilter(4),
      Filter_5                               = CU.fApplyFilter(5),
      Filter_6                               = CU.fApplyFilter(6),
      Filter_7                               = CU.fApplyFilter(7),
      Filter_8                               = CU.fApplyFilter(8),
      Filter_9                               = CU.fApplyFilter(9),
      Filter_10                              = CU.fApplyFilter(10),
      Filter_11                              = CU.fApplyFilter(11),
      Filter_12                              = CU.fApplyFilter(12),
      FullRefresh                            = CU.FullRefresh,
      GraduatedFilter                        = CU.fToggleTool('gradient'),
      IncreaseRating                         = LrSelection.increaseRating,
      IncrementLastDevelopParameter          = function() Ut.execFOM(LrDevelopController.increment,LastParam) end,
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
      LocalPreset1  = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[1]) end,
      LocalPreset2  = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[2]) end,
      LocalPreset3  = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[3]) end,
      LocalPreset4  = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[4]) end,
      LocalPreset5  = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[5]) end,
      LocalPreset6  = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[6]) end,
      LocalPreset7  = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[7]) end,
      LocalPreset8  = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[8]) end,
      LocalPreset9  = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[9]) end,
      LocalPreset10 = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[10]) end,
      LocalPreset11 = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[11]) end,
      LocalPreset12 = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[12]) end,
      LocalPreset13 = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[13]) end,
      LocalPreset14 = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[14]) end,
      LocalPreset15 = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[15]) end,
      LocalPreset16 = function() LocalPresets.ApplyLocalPreset(ProgramPreferences.LocalPresets[16]) end,
      LRCopy                   = CU.SimulateKeys('2c'),
      LRPaste                  = CU.SimulateKeys('2v'),
      LensProfileEnable        = CU.fToggle01Async('LensProfileEnable'),
      Loupe                    = CU.fToggleTool('loupe'),
      Next                     = LrSelection.nextPhoto,
      PasteSelectedSettings    = CU.PasteSelectedSettings,
      PasteSettings            = CU.PasteSettings,
      Pause                    = function() LrTasks.sleep( 0.02 ) end,
      Pick                     = LrSelection.flagAsPick,
      PointCurveLinear         = CU.UpdatePointCurve({ToneCurveName="Linear",ToneCurveName2012="Linear",ToneCurvePV2012={0,0,255,255,}}),
      PointCurveMediumContrast = CU.UpdatePointCurve({ToneCurveName="Medium Contrast",ToneCurveName2012="Medium Contrast",
          ToneCurvePV2012={0,0,32,22,64,56,128,128,192,196,255,255,}}),
      PointCurveStrongContrast = CU.UpdatePointCurve({ToneCurveName="Strong Contrast",ToneCurveName2012="Strong Contrast",
          ToneCurvePV2012={0,0,32,16,64,50,128,128,192,202,255,255,}}),
      PostCropVignetteStyle    = CU.fToggle1ModN('PostCropVignetteStyle', 3),
      PostCropVignetteStyleColorPriority     = Ut.wrapFOM(LrDevelopController.setValue,'PostCropVignetteStyle',2),
      PostCropVignetteStyleHighlightPriority = Ut.wrapFOM(LrDevelopController.setValue,'PostCropVignetteStyle',1),
      PostCropVignetteStylePaintOverlay      = Ut.wrapFOM(LrDevelopController.setValue,'PostCropVignetteStyle',3),
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
      Preset_41                = CU.fApplyPreset(41),
      Preset_42                = CU.fApplyPreset(42),
      Preset_43                = CU.fApplyPreset(43),
      Preset_44                = CU.fApplyPreset(44),
      Preset_45                = CU.fApplyPreset(45),
      Preset_46                = CU.fApplyPreset(46),
      Preset_47                = CU.fApplyPreset(47),
      Preset_48                = CU.fApplyPreset(48),
      Preset_49                = CU.fApplyPreset(49),
      Preset_50                = CU.fApplyPreset(50),
      Preset_51                = CU.fApplyPreset(51),
      Preset_52                = CU.fApplyPreset(52),
      Preset_53                = CU.fApplyPreset(53),
      Preset_54                = CU.fApplyPreset(54),
      Preset_55                = CU.fApplyPreset(55),
      Preset_56                = CU.fApplyPreset(56),
      Preset_57                = CU.fApplyPreset(57),
      Preset_58                = CU.fApplyPreset(58),
      Preset_59                = CU.fApplyPreset(59),
      Preset_60                = CU.fApplyPreset(60),
      Preset_61                = CU.fApplyPreset(61),
      Preset_62                = CU.fApplyPreset(62),
      Preset_63                = CU.fApplyPreset(63),
      Preset_64                = CU.fApplyPreset(64),
      Preset_65                = CU.fApplyPreset(65),
      Preset_66                = CU.fApplyPreset(66),
      Preset_67                = CU.fApplyPreset(67),
      Preset_68                = CU.fApplyPreset(68),
      Preset_69                = CU.fApplyPreset(69),
      Preset_70                = CU.fApplyPreset(70),
      Preset_71                = CU.fApplyPreset(71),
      Preset_72                = CU.fApplyPreset(72),
      Preset_73                = CU.fApplyPreset(73),
      Preset_74                = CU.fApplyPreset(74),
      Preset_75                = CU.fApplyPreset(75),
      Preset_76                = CU.fApplyPreset(76),
      Preset_77                = CU.fApplyPreset(77),
      Preset_78                = CU.fApplyPreset(78),
      Preset_79                = CU.fApplyPreset(79),
      Preset_80                = CU.fApplyPreset(80),
      Prev                     = LrSelection.previousPhoto,
      Profile_Adobe_Standard          = CU.UpdateCameraProfile('Adobe Standard'),
      Profile_Camera_Clear            = CU.UpdateCameraProfile('Camera Clear'),
      Profile_Camera_Darker_Skin_Tone = CU.UpdateCameraProfile('Camera Darker Skin Tone'),
      Profile_Camera_Deep             = CU.UpdateCameraProfile('Camera Deep'),
      Profile_Camera_Faithful         = CU.UpdateCameraProfile('Camera Faithful'),
      Profile_Camera_Flat             = CU.UpdateCameraProfile('Camera Flat'),
      Profile_Camera_Landscape        = CU.UpdateCameraProfile('Camera Landscape'),
      Profile_Camera_Light            = CU.UpdateCameraProfile('Camera Light'),
      Profile_Camera_Lighter_Skin_Tone= CU.UpdateCameraProfile('Camera Lighter Skin Tone'),
      Profile_Camera_Monochrome       = CU.UpdateCameraProfile('Camera Monochrome'),
      Profile_Camera_Monotone         = CU.UpdateCameraProfile('Camera Monotone'),
      Profile_Camera_Muted            = CU.UpdateCameraProfile('Camera Muted'),
      Profile_Camera_Natural          = CU.UpdateCameraProfile('Camera Natural'),
      Profile_Camera_Neutral          = CU.UpdateCameraProfile('Camera Neutral'),
      Profile_Camera_Portrait         = CU.UpdateCameraProfile('Camera Portrait'),
      Profile_Camera_Positive_Film    = CU.UpdateCameraProfile('Camera Positive Film'),
      Profile_Camera_Standard         = CU.UpdateCameraProfile('Camera Standard'),
      Profile_Camera_Vivid            = CU.UpdateCameraProfile('Camera Vivid'),
      Profile_Camera_Vivid_Blue       = CU.UpdateCameraProfile('Camera Vivid Blue'),
      Profile_Camera_Vivid_Green      = CU.UpdateCameraProfile('Camera Vivid Green'),
      Profile_Camera_Vivid_Red        = CU.UpdateCameraProfile('Camera Vivid Red'),
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
      RevealPanelTransform     = CU.fChangePanel('transformPanel'),
      Select1Left              = function() LrSelection.extendSelection('left',1) end,
      Select1Right             = function() LrSelection.extendSelection('right',1) end,
      SetRating0               = function() LrSelection.setRating(0) end,
      SetRating1               = function() LrSelection.setRating(1) end,
      SetRating2               = function() LrSelection.setRating(2) end,
      SetRating3               = function() LrSelection.setRating(3) end,
      SetRating4               = function() LrSelection.setRating(4) end,
      SetRating5               = function() LrSelection.setRating(5) end,
      ShoScndVwcompare         = function() LrApplicationView.showSecondaryView('compare') end,
      ShoScndVwgrid            = function() LrApplicationView.showSecondaryView('grid') end,
      ShoScndVwlive_loupe      = function() LrApplicationView.showSecondaryView('live_loupe') end,
      ShoScndVwlocked_loupe    = function() LrApplicationView.showSecondaryView('locked_loupe') end,
      ShoScndVwloupe           = function() LrApplicationView.showSecondaryView('loupe') end,
      ShoScndVwslideshow       = function() LrApplicationView.showSecondaryView('slideshow') end,
      ShoScndVwsurvey          = function() LrApplicationView.showSecondaryView('survey') end,
      ShoVwRefHoriz            = function() LrApplicationView.showView('develop_reference_horiz') end,
      ShoVwRefVert             = function() LrApplicationView.showView('develop_reference_vert') end,
      ShoVwcompare             = function() LrApplicationView.showView('compare') end,
      ShoVwdevelop_before      = function() LrApplicationView.showView('develop_before') end,
      ShoVwdevelop_before_after_horiz = function() LrApplicationView.showView('develop_before_after_horiz') end,
      ShoVwdevelop_before_after_vert  = function() LrApplicationView.showView('develop_before_after_vert') end,
      ShoVwdevelop_loupe       = function() LrApplicationView.showView('develop_loupe') end,
      ShoVwgrid                = function() LrApplicationView.showView('grid') end,
      ShoVwloupe               = function() LrApplicationView.showView('loupe') end,
      ShoVwpeople              = function() LrApplicationView.showView('people') end,
      ShoVwsurvey              = function() LrApplicationView.showView('survey') end,
      ShowMaskOverlay          = CU.SimulateKeys('0o'),
      SpotRemoval              = CU.fToggleTool('dust'),
      SwToMbook                = CU.fChangeModule('book'),
      SwToMdevelop             = CU.fChangeModule('develop'),
      SwToMlibrary             = CU.fChangeModule('library'),
      SwToMmap                 = CU.fChangeModule('map'),
      SwToMprint               = CU.fChangeModule('print'),
      SwToMslideshow           = CU.fChangeModule('slideshow'),
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
      UprightGuided            = Ut.wrapFOM(LrDevelopController.setValue,'PerspectiveUpright',5),
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
      AppLocale          = function(value) Info.AppLocale = value end,
      AppPath            = function(value) Info.AppPath = value; DebugInfo.write() end,
      AppVersion         = function(value) Info.AppVersion = value end,
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

    local function RunActionSeries(strarg1)
      local strarg = strarg1 -- make argument available to async task
      LrTasks.startAsyncTask(
        function()
          --[[-----------debug section, enable by adding - to beginning this line
          LrMobdebug.on()
          --]]-----------end debug section
          local value -- will need to assign when enable settings functions
          for i in strarg:gmatch("[%w_]+") do
            if(ACTIONS[i]) then -- perform a one time action
              ACTIONS[i]()
            elseif(SETTINGS[i]) then -- do something requiring the transmitted value to be known
              SETTINGS[i](value)
            elseif(Virtual[i]) then -- handle a virtual command
              local lp = Virtual[i](value, UpdateParam)
              if lp then
                LastParam = lp
              end
            elseif(i:find('Reset') == 1) then -- perform a reset other than those explicitly coded in ACTIONS array
              local resetparam = i:sub(6)
              Ut.execFOM(LrDevelopController.resetToDefault,resetparam)
              if ProgramPreferences.ClientShowBezelOnChange then
                local bezelname = ParamList.ParamDisplay[resetparam] or resetparam
                local lrvalue = LrDevelopController.getValue(resetparam)
                LrDialogs.showBezel(bezelname..'  '..LrStringUtils.numberToStringWithSeparators(lrvalue,Ut.precision(lrvalue)))
              end
            else -- otherwise update a develop parameter -- removed recursion guard as it is not in scope here
              UpdateParam(i,tonumber(value))
            end
            LrTasks.sleep(0.01) --pause between actions to allow synchronization with slow LR responses and keystrokes from app
          end
        end 
      )
    end

    --assign after defining RunActionSeries function
    ACTIONS.ActionSeries1 = function() RunActionSeries(ProgramPreferences.ActionSeries[1]) end
    ACTIONS.ActionSeries2 = function() RunActionSeries(ProgramPreferences.ActionSeries[2]) end
    ACTIONS.ActionSeries3 = function() RunActionSeries(ProgramPreferences.ActionSeries[3]) end
    ACTIONS.ActionSeries4 = function() RunActionSeries(ProgramPreferences.ActionSeries[4]) end
    ACTIONS.ActionSeries5 = function() RunActionSeries(ProgramPreferences.ActionSeries[5]) end
    ACTIONS.ActionSeries6 = function() RunActionSeries(ProgramPreferences.ActionSeries[6]) end
    ACTIONS.ActionSeries7 = function() RunActionSeries(ProgramPreferences.ActionSeries[7]) end
    ACTIONS.ActionSeries8 = function() RunActionSeries(ProgramPreferences.ActionSeries[8]) end
    ACTIONS.ActionSeries9 = function() RunActionSeries(ProgramPreferences.ActionSeries[9]) end
    --called within LrRecursionGuard for setting
    function UpdateParamPickup() --closure
      local paramlastmoved = {}
      local lastfullrefresh = 0
      return function(param, midi_value, silent)
        if LrApplication.activeCatalog():getTargetPhoto() == nil then return end--unable to update param
        local value
        if LrApplicationView.getCurrentModuleName() ~= 'develop' then
          LrApplicationView.switchToModule('develop')
        end
        if Limits.Parameters[param] then
          Limits.ClampValue(param)
        end
        if((math.abs(midi_value - CU.LRValueToMIDIValue(param)) <= PICKUP_THRESHOLD) or (paramlastmoved[param] ~= nil and paramlastmoved[param] + 0.5 > os.clock())) then -- pickup succeeded
          paramlastmoved[param] = os.clock()
          value = CU.MIDIValueToLRValue(param, midi_value)
          MIDI2LR.PARAM_OBSERVER[param] = value
          LrDevelopController.setValue(param, value)
          LastParam = param
          if ProgramPreferences.ClientShowBezelOnChange and not silent then
            local bezelname = ParamList.ParamDisplay[param] or param
            LrDialogs.showBezel(bezelname..'  '..LrStringUtils.numberToStringWithSeparators(value,Ut.precision(value)))
          end
          if ParamList.ProfileMap[param] then
            Profiles.changeProfile(ParamList.ProfileMap[param])
          end
        else --failed pickup
          if ProgramPreferences.ClientShowBezelOnChange then -- failed pickup. do I display bezel?
            value = CU.MIDIValueToLRValue(param, midi_value)
            local actualvalue = LrDevelopController.getValue(param)
            local precision = Ut.precision(value)
            local bezelname = ParamList.ParamDisplay[param] or param
            LrDialogs.showBezel(bezelname..'  '..LrStringUtils.numberToStringWithSeparators(value,precision)..'  '..LrStringUtils.numberToStringWithSeparators(actualvalue,precision))
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
    function UpdateParamNoPickup(param, midi_value, silent)
      if LrApplication.activeCatalog():getTargetPhoto() == nil then return end--unable to update param
      local value
      if LrApplicationView.getCurrentModuleName() ~= 'develop' then
        LrApplicationView.switchToModule('develop')
      end
      --Don't need to clamp limited parameters without pickup, as MIDI controls will still work
      --if value is outside limits range
      value = CU.MIDIValueToLRValue(param, midi_value)
      MIDI2LR.PARAM_OBSERVER[param] = value
      LrDevelopController.setValue(param, value)
      LastParam = param
      if ProgramPreferences.ClientShowBezelOnChange and not silent then
        local bezelname = ParamList.ParamDisplay[param] or param
        LrDialogs.showBezel(bezelname..'  '..LrStringUtils.numberToStringWithSeparators(value,Ut.precision(value)))
      end
      if ParamList.ProfileMap[param] then
        Profiles.changeProfile(ParamList.ProfileMap[param])
      end
    end
    UpdateParam = UpdateParamPickup --initial state


    LrFunctionContext.callWithContext(
      'socket_remote',
      function( context )
        --[[-----------debug section, enable by adding - to beginning this line
        LrMobdebug.on()
        --]]-----------end debug section
        local LrRecursionGuard    = import 'LrRecursionGuard'
        local LrShell             = import 'LrShell'
        local LrSocket            = import 'LrSocket'
        local guardreading = LrRecursionGuard('reading')
        local guardsetting = LrRecursionGuard('setting')
        local CurrentObserver
        --call following within guard for reading
        local function AdjustmentChangeObserver()
          local lastrefresh = 0
          return function(observer) -- closure
            if Limits.LimitsCanBeSet() and lastrefresh + 0.1 < os.clock() then
              for _,param in ipairs(ParamList.SendToMidi) do
                local lrvalue = LrDevelopController.getValue(param)
                if observer[param] ~= lrvalue and type(lrvalue) == 'number' and MIDI2LR.SERVER.send then
                  --check send not nil as may be nil in few ms after startup
                  MIDI2LR.SERVER:send(string.format('%s %g\n', param, CU.LRValueToMIDIValue(param)))
                  observer[param] = lrvalue
                  LastParam = param
                end
              end
              lastrefresh = os.clock()
            end
          end
        end
        AdjustmentChangeObserver = AdjustmentChangeObserver() --complete closure
        local function InactiveObserver() end
        CurrentObserver = AdjustmentChangeObserver -- will change when detect loss of MIDI controller

        -- wrapped in function so can be called when connection lost
        local function startServer(context1)
          MIDI2LR.SERVER = LrSocket.bind {
            functionContext = context1,
            plugin = _PLUGIN,
            port = SEND_PORT,
            mode = 'send',
            onError = function( socket )
              if MIDI2LR.RUNNING then --
                socket:reconnect()
              end
            end,
          }
        end

        MIDI2LR.CLIENT = LrSocket.bind {
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
                if(tonumber(value) > BUTTON_ON) then
                  ACTIONS[param]()
                end
              elseif(SETTINGS[param]) then -- do something requiring the transmitted value to be known
                SETTINGS[param](value)
              elseif(Virtual[param]) then -- handle a virtual command
                local lp = Virtual[param](value, UpdateParam)
                if lp then
                  LastParam = lp
                end
              elseif(param:find('Reset') == 1) then -- perform a reset other than those explicitly coded in ACTIONS array
                if(tonumber(value) > BUTTON_ON) then
                  local resetparam = param:sub(6)
                  Ut.execFOM(LrDevelopController.resetToDefault,resetparam)
                  if ProgramPreferences.ClientShowBezelOnChange then
                    local bezelname = ParamList.ParamDisplay[resetparam] or resetparam
                    local lrvalue = LrDevelopController.getValue(resetparam)
                    LrDialogs.showBezel(bezelname..'  '..LrStringUtils.numberToStringWithSeparators(lrvalue,Ut.precision(lrvalue)))
                  end
                end
              else -- otherwise update a develop parameter
                guardsetting:performWithGuard(UpdateParam,param,tonumber(value))
              end
            end
          end,
          onClosed = function( socket )
            if MIDI2LR.RUNNING then
              -- MIDI2LR closed connection, allow for reconnection
              socket:reconnect()
              -- calling SERVER:reconnect causes LR to hang for some reason...
              MIDI2LR.SERVER:close()
              startServer(context)
            end
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
          if ProgramPreferences.RevealAdjustedControls then --may be nil or false
            LrDevelopController.revealAdjustedControls( true ) -- reveal affected parameter in panel track
          end
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
      end
    )
  end
)
