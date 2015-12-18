--[[----------------------------------------------------------------------------

Develop_Params.lua
List of develop params
 
This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe, derived from code
by Parth.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>. 
------------------------------------------------------------------------------]]

DEVELOP_PARAMS = {
  'Temperature',
  'Tint',
  'Exposure',
  'Highlights',
  'Shadows',
  'Contrast',
  'Whites',
  'Blacks',
  'Clarity',
  'Vibrance',
  'Saturation',
  -- Tone
  'ParametricDarks',
  'ParametricLights',
  'ParametricShadows',
  'ParametricHighlights',
  'ParametricShadowSplit',
  'ParametricMidtoneSplit',
  'ParametricHighlightSplit',
  -- Mixer
  'SaturationAdjustmentRed',
  'SaturationAdjustmentOrange',
  'SaturationAdjustmentYellow',
  'SaturationAdjustmentGreen',
  'SaturationAdjustmentAqua',
  'SaturationAdjustmentBlue',
  'SaturationAdjustmentPurple',
  'SaturationAdjustmentMagenta',
  'HueAdjustmentRed',
  'HueAdjustmentOrange',
  'HueAdjustmentYellow',
  'HueAdjustmentGreen',
  'HueAdjustmentAqua',
  'HueAdjustmentBlue',
  'HueAdjustmentPurple',
  'HueAdjustmentMagenta',
  'LuminanceAdjustmentRed',
  'LuminanceAdjustmentOrange',
  'LuminanceAdjustmentYellow',
  'LuminanceAdjustmentGreen',
  'LuminanceAdjustmentAqua',
  'LuminanceAdjustmentBlue',
  'LuminanceAdjustmentPurple',
  'LuminanceAdjustmentMagenta',
  'GrayMixerRed',
  'GrayMixerOrange',
  'GrayMixerYellow',
  'GrayMixerGreen',
  'GrayMixerAqua',
  'GrayMixerBlue',
  'GrayMixerPurple',
  'GrayMixerMagenta',
  -- Split Toning
  'SplitToningShadowHue',
  'SplitToningShadowSaturation',
  'SplitToningHighlightHue',
  'SplitToningHighlightSaturation',
  'SplitToningBalance',
  -- Detail
  'Sharpness',
  'SharpenRadius',
  'SharpenDetail',
  'SharpenEdgeMasking',
  'LuminanceSmoothing',
  'LuminanceNoiseReductionDetail',
  'LuminanceNoiseReductionContrast',
  'ColorNoiseReduction',
  'ColorNoiseReductionDetail',
  'ColorNoiseReductionSmoothness',
  -- Lens Correction
  'LensProfileDistortionScale',
  'LensProfileChromaticAberrationScale',
  'LensProfileVignettingScale',
  'LensManualDistortionAmount',
  'DefringePurpleAmount',
  'DefringePurpleHueLo',
  'DefringePurpleHueHi',
  'DefringeGreenAmount',
  'DefringeGreenHueLo',
  'DefringeGreenHueHi',
  'PerspectiveVertical',
  'PerspectiveHorizontal',
  'PerspectiveRotate',
  'PerspectiveScale',
  'PerspectiveAspect',
  'PerspectiveUpright',
  -- Effects
  'Dehaze',
  'PostCropVignetteAmount',
  'PostCropVignetteMidpoint',
  'PostCropVignetteFeather',
  'PostCropVignetteRoundness',
  'PostCropVignetteStyle',
  'PostCropVignetteHighlightContrast',
  'GrainAmount',
  'GrainSize',
  'GrainFrequency',
  -- Calibrate
  'ShadowTint',
  'RedHue',
  'RedSaturation',
  'GreenHue',
  'GreenSaturation',
  'BlueHue',
  'BlueSaturation',
  -- Experimental, shown in copy settings not in documentation
  'AutoBrightness', --true/false
  'AutoContrast', --true/false --read only?
  'AutoExposure', --true/false --read only?
  'AutoLateralCA', --0
  'AutoShadows', --true/false
  'Brightness', --50
  'CameraProfile', --text "Adobe Standard"
  'ChromaticAberrationB', --0
  'ChromaticAberrationR', --0
  'ConvertToGrayscale',  --true/false --works
  'CropAngle', --0 --works
  'CropBottom', --1 --works
  'CropConstrainToWarp', --0
  'CropLeft', --0 --works
  'CropRight', --1 --works
  'CropTop', --0 --works
  'Defringe', --0
  'EnableCalibration',  --true/false
  'EnableCircularGradientBasedCorrections',  --true/false
  'EnableColorAdjustments', --true/false
  'EnableDetail', --true/false
  'EnableEffects', --true/false
  'EnableGradientBasedCorrections', --true/false
  'EnableGrayscaleMix', --true/false
  'EnableLensCorrections', --true/false
  'EnablePaintBasedCorrections', --true/false
  'EnableRedEye', --true/false
  'EnableRetouch', --true/false
  'EnableSplitToning', --true/false
  'LensProfileEnable',--1
  'LensProfileSetup', -- "LensDefaults"
  'orientation', --"AB"
  'ProcessVersion', --"6.7"
  'RedEyeInfo', -- {}
  'RetouchInfo', -- {}
  'ToneCurve', -- {0,0,32,22,64,56,128,128,192,196,255,255} --looks to be read only?
  'ToneCurveName', -- "Medium Contrast", "Linear", "Strong Contrast" 
  'ToneCurveName2012',  --"Linear"
  'ToneCurvePV2012', -- {0,0,255,255}
  'ToneCurvePV2012Blue', -- {0,0,255,255}
  'ToneCurvePV2012Green', -- {0,0,255,255}
  'ToneCurvePV2012Red', -- {0,0,255,255}
  'VignetteAmount', -- 0 --this affects lens vignetting in manual tab of lens corrections
  'VignetteMidpoint', -- 50 --this affects lens vignetting in manual tab of lens corrections
  'WhiteBalance', -- "Custom" -- works Auto Daylight Cloudy Shade Tungsten Fluorescent Flash

}