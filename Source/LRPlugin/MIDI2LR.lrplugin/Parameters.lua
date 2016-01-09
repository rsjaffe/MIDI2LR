--[[----------------------------------------------------------------------------

Parameters.lua
List of develop params

Parameters.Names table. Key = short name. Value = {[1]=Friendly name,[2]=rank order,[3]panel used}
Parameters.Order table(array). Key = rank order. Value = short name.
 
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

--[[ debugging section. reduce --- to -- to comment out
local function LOC(a) 
  local idx = a:find("=",1,true)
  return a:sub(idx+1) 
end
-- current status: module tested independently and passed
--]] -- http://notebook.kulchenko.com/zerobrane/debugging-lightroom-plugins-zerobrane-studio-ide

local Names = {
  Temperature = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Temperature=Temperature"), 10, 'adjustPanel'},
  Tint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Tint=Tint"), 20, 'adjustPanel'},
  Exposure = {LOC("$$$/AgCameraRawUI/Exposure=Exposure"), 30, 'adjustPanel'},
--  Exposure2012 = {LOC("$$$/AgCameraRawUI/Exposure=Exposure")..' 2012', 35, 'adjustPanel'},
  Highlights = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Highlights=Highlights"), 40, 'adjustPanel'},
--  Highlights2012 = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Highlights=Highlights")..' 2012', 45, 'adjustPanel'},
  Shadows = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Shadows=Shadows"), 50, 'adjustPanel'},
--  Shadows2012 = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Shadows=Shadows")..' 2012', 55, 'adjustPanel'},
  Contrast = {LOC("$$$/AgCameraRawUI/Contrast=Contrast"), 60, 'adjustPanel'},
--  Contrast2012 = {LOC("$$$/AgCameraRawUI/Contrast=Contrast")..' 2012', 65, 'adjustPanel'},
  Whites = {LOC("$$$/AgCameraRawUI/Whites=Whites"), 70, 'adjustPanel'},
--  Whites2012 = {LOC("$$$/AgCameraRawUI/Whites=Whites")..'2012', 75, 'adjustPanel'},
  Blacks = {LOC("$$$/AgCameraRawUI/Blacks=Blacks"), 80, 'adjustPanel'},
--  Blacks2012 = {LOC("$$$/AgCameraRawUI/Blacks=Blacks")..' 2012', 85, 'adjustPanel'},
  Clarity = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Clarity=Clarity"), 90, 'adjustPanel'},
--  Clarity2012 = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Clarity=Clarity")..'2012', 95, 'adjustPanel'},
  Vibrance = {LOC("$$$/AgCameraRawUI/Vibrance=Vibrance"), 100, 'adjustPanel'},
  Saturation = {LOC("$$$/AgCameraRawUI/Saturation=Saturation"), 110, 'adjustPanel'},
  -- Tone
  ParametricDarks = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneDarks=Parametric Darks"), 120, 'tonePanel'},
  ParametricLights = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneLights=Parametric Lights"), 130, 'tonePanel'},
  ParametricShadows = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadows=Parametric Shadows"), 140, 'tonePanel'},
  ParametricHighlights = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlights=Parametric Highlights"), 150, 'tonePanel'},
  ParametricShadowSplit = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadowSplit=Parametric Shadow Split"), 160, 'tonePanel'},
  ParametricMidtoneSplit = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneMidtoneSplit=Parametric Midtone Split"), 170, 'tonePanel'},
  ParametricHighlightSplit = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlightSplit=Parametric Highlight Split"), 180, 'tonePanel'},
  -- Mixer
  SaturationAdjustmentRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentRed=Saturation Adjustment Red"), 190, 'mixerPanel'},
  SaturationAdjustmentOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentOrange=Saturation Adjustment Orange"), 200, 'mixerPanel'},
  SaturationAdjustmentYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentYellow=Saturation Adjustment Yellow"), 210, 'mixerPanel'},
  SaturationAdjustmentGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentGreen=Saturation Adjustment Green"), 220, 'mixerPanel'},
  SaturationAdjustmentAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentAqua=Saturation Adjustment Aqua"), 230, 'mixerPanel'},
  SaturationAdjustmentBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentBlue=Saturation Adjustment Blue"), 240, 'mixerPanel'},
  SaturationAdjustmentPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentPurple=Saturation Adjustment Purple"), 250, 'mixerPanel'},
  SaturationAdjustmentMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentMagenta=Saturation Adjustment Magenta"), 260, 'mixerPanel'},
  HueAdjustmentRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentRed=Hue Adjustment Red"), 270, 'mixerPanel'},
  HueAdjustmentOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentOrange=Hue Adjustment Orange"), 280, 'mixerPanel'},
  HueAdjustmentYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentYellow=Hue Adjustment Yellow"), 290, 'mixerPanel'},
  HueAdjustmentGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentGreen=Hue Adjustment Green"), 300, 'mixerPanel'},
  HueAdjustmentAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentAqua=Hue Adjustment Aqua"), 310, 'mixerPanel'},
  HueAdjustmentBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentBlue=Hue Adjustment Blue"), 320, 'mixerPanel'},
  HueAdjustmentPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentPurple=Hue Adjustment Purple"), 330, 'mixerPanel'},
  HueAdjustmentMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentMagenta=Hue Adjustment Magenta"), 340, 'mixerPanel'},
  LuminanceAdjustmentRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentRed=Luminance Adjustment Red"), 350, 'mixerPanel'},
  LuminanceAdjustmentOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentOrange=Luminance Adjustment Orange"), 360, 'mixerPanel'},
  LuminanceAdjustmentYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentYellow=Luminance Adjustment Yellow"), 370, 'mixerPanel'},
  LuminanceAdjustmentGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentGreen=Luminance Adjustment Green"), 380, 'mixerPanel'},
  LuminanceAdjustmentAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentAqua=Luminance Adjustment Aqua"), 390, 'mixerPanel'},
  LuminanceAdjustmentBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentBlue=Luminance Adjustment Blue"), 400, 'mixerPanel'},
  LuminanceAdjustmentPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentPurple=Luminance Adjustment Purple"), 410, 'mixerPanel'},
  LuminanceAdjustmentMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentMagenta=Luminance Adjustment Magenta"), 420, 'mixerPanel'},
  GrayMixerRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerRed=Gray Mixer Red"), 430, 'mixerPanel'},
  GrayMixerOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerOrange=Gray Mixer Orange"), 440, 'mixerPanel'},
  GrayMixerYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerYellow=Gray Mixer Yellow"), 450, 'mixerPanel'},
  GrayMixerGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerGreen=Gray Mixer Green"), 460, 'mixerPanel'},
  GrayMixerAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerAqua=Gray Mixer Aqua"), 470, 'mixerPanel'},
  GrayMixerBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerBlue=Gray Mixer Blue"), 480, 'mixerPanel'},
  GrayMixerPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerPurple=Gray Mixer Purple"), 490, 'mixerPanel'},
  GrayMixerMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerMagenta=Gray Mixer Magenta"), 500, 'mixerPanel'},
  -- Split Toning
  SplitToningShadowHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowHue=Split Toning Shadow Hue"), 510, 'splitToningPanel'},
  SplitToningShadowSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowSaturation=Split Toning Shadow Saturation"), 520, 'splitToningPanel'},
  SplitToningHighlightHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightHue=Split Toning Highlight Hue"), 530, 'splitToningPanel'},
  SplitToningHighlightSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightSaturation=Split Toning Highlight Saturation"), 540, 'splitToningPanel'},
  SplitToningBalance = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningBalance=Split Toning Balance"), 550, 'splitToningPanel'},
  -- Detail
  Sharpness = {LOC("$$$/AgDevelop/Localized/Sharpness=Sharpness"), 560, 'detailPanel'},
  SharpenRadius = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenRadius=Sharpen Radius"), 570, 'detailPanel'},
  SharpenDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenDetail=Sharpen Detail"), 580, 'detailPanel'},
  SharpenEdgeMasking = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenEdgeMasking=Sharpen Edge Masking"), 590, 'detailPanel'},
  LuminanceSmoothing = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceSmoothing=Luminance Smoothing"), 600, 'detailPanel'},
  LuminanceNoiseReductionDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionDetail=Luminance Noise Reduction Detail"), 610, 'detailPanel'},
  LuminanceNoiseReductionContrast = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionContrast=Luminance Noise Reduction Contrast"), 620, 'detailPanel'},
  ColorNoiseReduction = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReduction=Color Noise Reduction"), 630, 'detailPanel'},
  ColorNoiseReductionDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionDetail=Color Noise Reduction Detail"), 640, 'detailPanel'},
  ColorNoiseReductionSmoothness = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionSmoothness=Color Noise Reduction Smoothness"), 650, 'detailPanel'},
  -- Lens Correction
  LensProfileDistortionScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileDistortionScale=Lens Profile Distortion Scale"), 660, 'lensCorrectionsPanel'},
  LensProfileChromaticAberrationScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileChromaticAberrationScale=Lens Profile Chromatic Aberration Scale"), 670, 'lensCorrectionsPanel'},
  LensProfileVignettingScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileVignettingScale=Lens Profile Vignetting Scale"), 680, 'lensCorrectionsPanel'},
  LensManualDistortionAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensManualDistortionAmount=Lens Manual Distortion Amount"), 690, 'lensCorrectionsPanel'},
  DefringePurpleAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeAmount=Defringe Purple Amount"), 700, 'lensCorrectionsPanel'},
  DefringePurpleHueLo = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeHueLo=Defringe Purple Hue")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/Low=Low"), 710, 'lensCorrectionsPanel'},
  DefringePurpleHueHi = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeHueLo=Defringe Purple Hue")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/High=High"), 730, 'lensCorrectionsPanel'},
  DefringeGreenAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeAmount=Defringe Green Amount"), 740, 'lensCorrectionsPanel'},
  DefringeGreenHueLo = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeHueLo=Defringe Green Hue")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/Low=Low"), 750, 'lensCorrectionsPanel'},
  DefringeGreenHueHi = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeHueLo=Defringe Green Hue")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/High=High"), 760, 'lensCorrectionsPanel'},
  PerspectiveVertical = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveVertical=Perspective Vertical"), 770, 'lensCorrectionsPanel'},
  PerspectiveHorizontal = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveHorizontal=Perspective Horizontal"), 780, 'lensCorrectionsPanel'},
  PerspectiveRotate = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveRotate=Perspective Rotate"), 790, 'lensCorrectionsPanel'},
  PerspectiveScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveScale=Perspective Scale"), 800, 'lensCorrectionsPanel'},
  PerspectiveAspect = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveAspect=Perspective Aspect"), 810, 'lensCorrectionsPanel'},
  PerspectiveUpright = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveUpright=Perspective Upright"), 820, 'lensCorrectionsPanel'},
  -- Effects
  Dehaze = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/DehazeAmount=Dehaze Amount"), 830, 'effectsPanel'},
  PostCropVignetteAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteAmount=Post Crop Vignette Amount"), 840, 'effectsPanel'},
  PostCropVignetteMidpoint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteMidpoint=Post Crop Vignette Midpoint"), 850, 'effectsPanel'},
  PostCropVignetteFeather = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteFeather=Post Crop Vignette Feather"), 860, 'effectsPanel'},
  PostCropVignetteRoundness = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteRoundness=Post Crop Vignette Roundness"), 870, 'effectsPanel'},
  PostCropVignetteStyle = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteStyle=Post Crop Vignette Style"), 880, 'effectsPanel'},
  PostCropVignetteHighlightContrast = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteHighlightContrast=Post Crop Vignette Highlight Contrast"), 890, 'effectsPanel'},
  GrainAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainAmount=Grain Amount"), 900, 'effectsPanel'},
  GrainSize = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainSize=Grain Size"), 910, 'effectsPanel'},
  GrainFrequency = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainRoughness=Grain Roughness"), 920, 'effectsPanel'},
  -- Calibrate
  ShadowTint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ShadowTintCalibration=Shadow Tint Calibration"), 930, 'calibratePanel'},
  RedHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedHueCalibration=Red Hue Calibration"), 940, 'calibratePanel'},
  RedSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedSaturationCalibration=Red Saturation Calibration"), 950, 'calibratePanel'},
  GreenHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenHueCalibration=Green Hue Calibration"), 960, 'calibratePanel'},
  GreenSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenSaturationCalibration=Green Saturation Calibration"), 970, 'calibratePanel'},
  BlueHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueHueCalibration=Blue Hue Calibration"), 980, 'calibratePanel'},
  BlueSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueSaturationCalibration=Blue Saturation Calibration"), 990, 'calibratePanel'},
  -- Experimental, shown in copy settings but not in documentation; enabled one-by-one after testing identifies functionality
  AutoLateralCA = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RemoveChromaticAberration=Remove Chromatic Aberration"), 1000, 'lensCorrectionsPanel'}, --0 off 1 on  --noloc
  CameraProfile = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CameraProfile=Camera Profile"), 1010, 'calibratePanel'}, --text "Adobe Standard"), 10, ''}, 'Camera Clear', Deep Landscape Light Neutral Protrait Standard Vivid  
  ConvertToGrayscale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ConvertToGrayscale=Convert to Grayscale"), 1020, 'mixerPanel'},  -- enabled in app
  CropAngle = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CropAngle=Crop Angle"), 1030, 'crop'}, -- enabled in app
  CropBottom = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Crop")..' - '.. LOC("$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Bottom=Bottom"), 1040, 'crop'}, -- enabled in app
  CropConstrainToWarp = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CropConstrainToWarp=Constrain to Warp"), 1050, 'crop'}, --0
  CropLeft = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Crop")..' - '..LOC("$$$/AgWatermarking/Alignment/Left=Left"), 1060, 'crop'}, -- enabled in app
  CropRight = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Crop")..' - '..LOC("$$$/AgWatermarking/Alignment/Right=Right"), 1070, 'crop'}, -- enabled in app
  CropTop = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Crop")..' - '..LOC("$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Top=Top"), 1080, 'crop'}, -- enabled in app  
  EnableCalibration = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableCalibration=Enable Calibration"), 1090, 'calibratePanel'},  -- enabled in app
  EnableCircularGradientBasedCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableRadialFilter=Enable Radial Filter"), 1100, 'circularGradient'},  -- enabled in app
  EnableColorAdjustments = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableColorAdjustments=Enable Color Adjustments"), 1110, 'mixerPanel'}, -- enabled in app
  EnableDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableDetail=Enable Detail"), 1120, 'detailPanel'}, -- enabled in app
  EnableEffects = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableEffects=Enable Effects"), 1130, 'effectsPanel'}, -- enabled in app
  EnableGradientBasedCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableGraduatedFilter=Enable Graduated Filter"), 1140, 'gradient'}, -- enabled in app
  EnableGrayscaleMix = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableGrayscaleMix=Enable Grayscale Mix"), 1150, 'mixerPanel'}, -- enabled in app
  EnableLensCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableLensCorrections=Enable Lens Corrections"), 1160, 'lensCorrectionsPanel'}, -- enabled in app
  EnablePaintBasedCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableBrushAdjustments=Enable Brush Adjustments"), 1170, 'localized'},
  EnableRedEye = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableRedEye=Enable Red-Eye"), 1180, 'redeye'}, -- enabled in app
  EnableRetouch = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableSpotRemoval=Enable Spot Removal"), 1190, 'dust'}, -- enabled in app
  EnableSplitToning = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableSplitToning=Enable Split Toning"), 1200, 'splitToningPanel'}, -- enabled in app
  LensProfileEnable = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileEnable=Enable Lens Corrections"), 1210, 'lensCorrectionsPanel'},--1 on 0 off
  VignetteAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteAmount=Vignette Amount"), 1220, 'lensCorrectionsPanel'}, -- 0 --this affects lens vignetting in manual tab of lens corrections
  VignetteMidpoint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteMidpoint=Vignette Midpoint"), 1230, 'lensCorrectionsPanel'}, -- 50 --this affects lens vignetting in manual tab of lens corrections  
  WhiteBalance = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/WhiteBalance=White Balance"), 1240, 'adjustPanel'}, -- enabled in app
  --don't know how to control the following or whether control is even possible
  AutoBrightness = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoBrightness=Automatic Brightness"), 1250, 'adjustPanel'}, --true/false
  AutoContrast = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoContrast=Automatic Contrast"), 1260, 'adjustPanel'}, --true/false --read only?
  AutoExposure = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoExposure=Automatic Exposure"), 1270, 'adjustPanel'}, --true/false --read only?
  AutoShadows = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoShadows=Automatic Shadows"), 1280, 'adjustPanel'}, --true/false
  Brightness = {LOC("$$$/AgCameraRawUI/Brightness=Brightness"), 1290, 'adjustPanel'}, --50
  ChromaticAberrationB = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ChromaticAberrationB=Blue Chromatic Aberration"), 1300, 'lensCorrectionsPanel'}, --0
  ChromaticAberrationR = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ChromaticAberrationR=Red Chromatic Aberration"), 1310, 'lensCorrectionsPanel'}, --0
  Defringe = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Defringe=Defringe"), 1320, 'lensCorrectionsPanel'}, --0
  LensProfileSetup = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileSetup=Lens Profile Setup"), 1330, 'lensCorrectionsPanel'}, -- "LensDefaults"), 10, ''}, Auto = {LOC("$$$/MIDI2LR/Parameters/Auto="), 10, ''}, Custom = {LOC("$$$/MIDI2LR/Parameters/Custom=" -- works, but who would want this control?
  orientation = {LOC("$$$/MIDI2LR/Parameters/orientation=orientation"), 1340, ''}, --"AB"
  ProcessVersion = {LOC("$$$/AgDevelop/Menu/ProcessVersion=Process Version"), 1350, 'adjustPanel'}, --"6.7"
  RedEyeInfo = {LOC("$$$/MIDI2LR/Parameters/RedEyeInfo=Red-Eye Information"), 1360, 'redeye'}, -- {}
  RetouchInfo = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/BrushAdjustments=Brush Adjustments"), 1370, 'localized'}, -- {}
  ToneCurve = {LOC("$$$/AgDevelop/Panel/ToneCurve=Tone Curve"), 1380, 'tonePanel'}, -- {0,0,32,22,64,56,128,128,192,196,255,255} --looks to be read only?
  ToneCurveName = {LOC("$$$/AgDevelop/Menu/Settings/Target/ToneCurve=Tone Curve Name"), 1390, 'tonePanel'}, -- "Medium Contrast"), 10, ''}, "Linear"), 10, ''}, "Strong Contrast" 
--  ToneCurveName2012 = {LOC("$$$/MIDI2LR/Parameters/ToneCurveName2012=Tone Curve Name 2012"), 1400, 'tonePanel'},  --"Linear"
--  ToneCurvePV2012 = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012=Tone Curve PV 2012"), 1410, 'tonePanel'}, -- {0,0,255,255}
  ToneCurvePV2012Blue = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012Blue=Tone Curve PV 2012 Blue"), 1420, 'tonePanel'}, -- {0,0,255,255}
  ToneCurvePV2012Green = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012Green=Tone Curve PV 2012 Green"), 1430, 'tonePanel'}, -- {0,0,255,255}
  ToneCurvePV2012Red = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012Red=Tone Curve PV 2012 Red"), 1440, 'tonePanel'}, -- {0,0,255,255}
}


local Order = {}
for k,_ in pairs(Names) do
  table.insert(Order,k)
end
table.sort(Order, function(a,b) return Names[a][2] < Names[b][2] end)

setmetatable(Names, 
  {__index = 
    function(t,k) 
      if type(k)=='number' then 
        return t[Order[k]]
      end
      return nil
    end,
  }
)

return {
  Names = Names,
  Order = Order
}
