--[[----------------------------------------------------------------------------

Develop_Params.lua
List of develop params
 
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

DEVELOP_PARAMS = {
  'Temperature',
  'Tint',
  'Exposure',
  'Exposure2012',
  'Highlights',
  'Highlights2012',
  'Shadows',
  'Shadows2012',
  'Contrast',
  'Contrast2012',
  'Whites',
  'Whites2012',
  'Blacks',
  'Blacks2012',
  'Clarity',
  'Clarity2012',
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
  -- Experimental, shown in copy settings but not in documentation, will be enabled one-by-one after testing identifies functionality
  'AutoLateralCA', --0 off 1 on  
  'CameraProfile', --text "Adobe Standard", 'Camera Clear', Deep Landscape Light Neutral Protrait Standard Vivid  
  'ConvertToGrayscale',  -- enabled in app
  'CropAngle', -- enabled in app
  'CropBottom', -- enabled in app
  'CropConstrainToWarp', --0
  'CropLeft', -- enabled in app
  'CropRight', -- enabled in app
  'CropTop', -- enabled in app  
  'EnableCalibration',  -- enabled in app
  'EnableCircularGradientBasedCorrections',  -- enabled in app
  'EnableColorAdjustments', -- enabled in app
  'EnableDetail', -- enabled in app
  'EnableEffects', -- enabled in app
  'EnableGradientBasedCorrections', -- enabled in app
  'EnableGrayscaleMix', -- enabled in app
  'EnableLensCorrections', -- enabled in app
  'EnablePaintBasedCorrections', -- enabled in app
  'EnableRedEye', -- enabled in app
  'EnableRetouch', -- enabled in app
  'EnableSplitToning', -- enabled in app
  'LensProfileEnable',--1 on 0 off
  'VignetteAmount', -- 0 --this affects lens vignetting in manual tab of lens corrections
  'VignetteMidpoint', -- 50 --this affects lens vignetting in manual tab of lens corrections  
  'WhiteBalance', -- enabled in app
  --don't know how to control the following or whether control is even possible
  'AutoBrightness', --true/false
  'AutoContrast', --true/false --read only?
  'AutoExposure', --true/false --read only?
  'AutoShadows', --true/false
  'Brightness', --50

  'ChromaticAberrationB', --0
  'ChromaticAberrationR', --0
  'Defringe', --0
  'LensProfileSetup', -- "LensDefaults", 'Auto', 'Custom' -- works, but who would want this control?
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
}