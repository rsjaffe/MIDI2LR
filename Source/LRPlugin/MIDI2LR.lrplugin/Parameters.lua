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

--[[ debugging section. reduce --- to -- to comment out
local function LOC(a) 
  local idx = a:find("=",1,true)
  return a:sub(idx+1) 
end
--]] -- http://notebook.kulchenko.com/zerobrane/debugging-lightroom-plugins-zerobrane-studio-ide

local Names = {
  Temperature = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Temperature=Temperature"), 10},
  Tint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Tint=Tint"), 20},
  Exposure = {LOC("$$$/AgCameraRawUI/Exposure=Exposure"), 30},
  Highlights = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Highlights=Highlights"), 40},
  Shadows = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Shadows=Shadows"), 50},
  Contrast = {LOC("$$$/AgCameraRawUI/Contrast=Contrast"), 60},
  Whites = {LOC("$$$/AgCameraRawUI/Whites=Whites"), 70},
  Blacks = {LOC("$$$/AgCameraRawUI/Blacks=Blacks"), 80},
  Clarity = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Clarity=Clarity"), 90},
  Vibrance = {LOC("$$$/AgCameraRawUI/Vibrance=Vibrance"), 100},
  Saturation = {LOC("$$$/AgCameraRawUI/Saturation=Saturation"), 110},
  -- Tone
  ParametricDarks = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneDarks=Parametric Darks"), 120},
  ParametricLights = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneLights=Parametric Lights"), 130},
  ParametricShadows = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadows=Parametric Shadows"), 140},
  ParametricHighlights = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlights=Parametric Highlights"), 150},
  ParametricShadowSplit = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadowSplit=Parametric Shadow Split"), 160},
  ParametricMidtoneSplit = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneMidtoneSplit=Parametric Midtone Split"), 170},
  ParametricHighlightSplit = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlightSplit=Parametric Highlight Split"), 180},
  -- Mixer
  SaturationAdjustmentRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentRed=Saturation Adjustment Red"), 190},
  SaturationAdjustmentOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentOrange=Saturation Adjustment Orange"), 200},
  SaturationAdjustmentYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentYellow=Saturation Adjustment Yellow"), 210},
  SaturationAdjustmentGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentGreen=Saturation Adjustment Green"), 220},
  SaturationAdjustmentAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentAqua=Saturation Adjustment Aqua"), 230},
  SaturationAdjustmentBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentBlue=Saturation Adjustment Blue"), 240},
  SaturationAdjustmentPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentPurple=Saturation Adjustment Purple"), 250},
  SaturationAdjustmentMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentMagenta=Saturation Adjustment Magenta"), 260},
  HueAdjustmentRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentRed=Hue Adjustment Red"), 270},
  HueAdjustmentOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentOrange=Hue Adjustment Orange"), 280},
  HueAdjustmentYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentYellow=Hue Adjustment Yellow"), 290},
  HueAdjustmentGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentGreen=Hue Adjustment Green"), 300},
  HueAdjustmentAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentAqua=Hue Adjustment Aqua"), 310},
  HueAdjustmentBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentBlue=Hue Adjustment Blue"), 320},
  HueAdjustmentPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentPurple=Hue Adjustment Purple"), 330},
  HueAdjustmentMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentMagenta=Hue Adjustment Magenta"), 340},
  LuminanceAdjustmentRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentRed=Luminance Adjustment Red"), 350},
  LuminanceAdjustmentOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentOrange=Luminance Adjustment Orange"), 360},
  LuminanceAdjustmentYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentYellow=Luminance Adjustment Yellow"), 370},
  LuminanceAdjustmentGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentGreen=Luminance Adjustment Green"), 380},
  LuminanceAdjustmentAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentAqua=Luminance Adjustment Aqua"), 390},
  LuminanceAdjustmentBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentBlue=Luminance Adjustment Blue"), 400},
  LuminanceAdjustmentPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentPurple=Luminance Adjustment Purple"), 410},
  LuminanceAdjustmentMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentMagenta=Luminance Adjustment Magenta"), 420},
  GrayMixerRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerRed=Gray Mixer Red"), 430},
  GrayMixerOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerOrange=Gray Mixer Orange"), 440},
  GrayMixerYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerYellow=Gray Mixer Yellow"), 450},
  GrayMixerGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerGreen=Gray Mixer Green"), 460},
  GrayMixerAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerAqua=Gray Mixer Aqua"), 470},
  GrayMixerBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerBlue=Gray Mixer Blue"), 480},
  GrayMixerPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerPurple=Gray Mixer Purple"), 490},
  GrayMixerMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerMagenta=Gray Mixer Magenta"), 500},
  -- Split Toning
  SplitToningShadowHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowHue=Split Toning Shadow Hue"), 510},
  SplitToningShadowSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowSaturation=Split Toning Shadow Saturation"), 520},
  SplitToningHighlightHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightHue=Split Toning Highlight Hue"), 530},
  SplitToningHighlightSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightSaturation=Split Toning Highlight Saturation"), 540},
  SplitToningBalance = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningBalance=Split Toning Balance"), 550},
  -- Detail
  Sharpness = {LOC("$$$/AgDevelop/Localized/Sharpness=Sharpness"), 560},
  SharpenRadius = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenRadius=Sharpen Radius"), 570},
  SharpenDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenDetail=Sharpen Detail"), 580},
  SharpenEdgeMasking = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenEdgeMasking=Sharpen Edge Masking"), 590},
  LuminanceSmoothing = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceSmoothing=Luminance Smoothing"), 600},
  LuminanceNoiseReductionDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionDetail=Luminance Noise Reduction Detail"), 610},
  LuminanceNoiseReductionContrast = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionContrast=Luminance Noise Reduction Contrast"), 620},
  ColorNoiseReduction = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReduction=Color Noise Reduction"), 630},
  ColorNoiseReductionDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionDetail=Color Noise Reduction Detail"), 640},
  ColorNoiseReductionSmoothness = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionSmoothness=Color Noise Reduction Smoothness"), 650},
  -- Lens Correction
  LensProfileDistortionScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileDistortionScale=Lens Profile Distortion Scale"), 660},
  LensProfileChromaticAberrationScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileChromaticAberrationScale=Lens Profile Chromatic Aberration Scale"), 670},
  LensProfileVignettingScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileVignettingScale=Lens Profile Vignetting Scale"), 680},
  LensManualDistortionAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensManualDistortionAmount=Lens Manual Distortion Amount"), 690},
  DefringePurpleAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeAmount=Defringe Purple Amount"), 700},
  DefringePurpleHueLo = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeHueLo=Defringe Purple Hue")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/Low=Low"), 710},
  DefringePurpleHueHi = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeHueLo=Defringe Purple Hue")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/High=High"), 730},
  DefringeGreenAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeAmount=Defringe Green Amount"), 740},
  DefringeGreenHueLo = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeHueLo=Defringe Green Hue")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/Low=Low"), 750},
  DefringeGreenHueHi = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeHueLo=Defringe Green Hue")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/High=High"), 760},
  PerspectiveVertical = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveVertical=Perspective Vertical"), 770},
  PerspectiveHorizontal = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveHorizontal=Perspective Horizontal"), 780},
  PerspectiveRotate = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveRotate=Perspective Rotate"), 790},
  PerspectiveScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveScale=Perspective Scale"), 800},
  PerspectiveAspect = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveAspect=Perspective Aspect"), 810},
  PerspectiveUpright = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveUpright=Perspective Upright"), 820},
  -- Effects
  Dehaze = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/DehazeAmount=Dehaze Amount"), 830},
  PostCropVignetteAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteAmount=Post Crop Vignette Amount"), 840},
  PostCropVignetteMidpoint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteMidpoint=Post Crop Vignette Midpoint"), 850},
  PostCropVignetteFeather = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteFeather=Post Crop Vignette Feather"), 860},
  PostCropVignetteRoundness = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteRoundness=Post Crop Vignette Roundness"), 870},
  PostCropVignetteStyle = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteStyle=Post Crop Vignette Style"), 880},
  PostCropVignetteHighlightContrast = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteHighlightContrast=Post Crop Vignette Highlight Contrast"), 890},
  GrainAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainAmount=Grain Amount"), 900},
  GrainSize = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainSize=Grain Size"), 910},
  GrainFrequency = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainRoughness=Grain Roughness"), 920},
  -- Calibrate
  ShadowTint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ShadowTintCalibration=Shadow Tint Calibration"), 930},
  RedHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedHueCalibration=Red Hue Calibration"), 940},
  RedSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedSaturationCalibration=Red Saturation Calibration"), 950},
  GreenHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenHueCalibration=Green Hue Calibration"), 960},
  GreenSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenSaturationCalibration=Green Saturation Calibration"), 970},
  BlueHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueHueCalibration=Blue Hue Calibration"), 980},
  BlueSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueSaturationCalibration=Blue Saturation Calibration"), 990},
  -- Experimental, shown in copy settings but not in documentation, will be enabled one-by-one after testing identifies functionality
  AutoLateralCA = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RemoveChromaticAberration=Remove Chromatic Aberration"), 1000}, --0 off 1 on  --noloc
  CameraProfile = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CameraProfile=Camera Profile"), 1010}, --text "Adobe Standard"), 10}, 'Camera Clear', Deep Landscape Light Neutral Protrait Standard Vivid  
  ConvertToGrayscale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ConvertToGrayscale=Convert to Grayscale"), 1020},  -- enabled in app
  CropAngle = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CropAngle=Crop Angle"), 1030}, -- enabled in app
  CropBottom = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Crop")..' - '.. LOC("$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Bottom=Bottom"), 1040}, -- enabled in app
  CropConstrainToWarp = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CropConstrainToWarp=Constrain to Warp"), 1050}, --0
  CropLeft = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Crop")..' - '..LOC("$$$/AgWatermarking/Alignment/Left=Left"), 1060}, -- enabled in app
  CropRight = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Crop")..' - '..LOC("$$$/AgWatermarking/Alignment/Right=Right"), 1070}, -- enabled in app
  CropTop = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Crop")..' - '..LOC("$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Top=Top"), 1080}, -- enabled in app  
  EnableCalibration = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableCalibration=Enable Calibration"), 1090},  -- enabled in app
  EnableCircularGradientBasedCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableRadialFilter=Enable Radial Filter"), 1100},  -- enabled in app
  EnableColorAdjustments = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableColorAdjustments=Enable Color Adjustments"), 1110}, -- enabled in app
  EnableDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableDetail=Enable Detail"), 1120}, -- enabled in app
  EnableEffects = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableEffects=Enable Effects"), 1130}, -- enabled in app
  EnableGradientBasedCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableGraduatedFilter=Enable Graduated Filter"), 1140}, -- enabled in app
  EnableGrayscaleMix = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableGrayscaleMix=Enable Grayscale Mix"), 1150}, -- enabled in app
  EnableLensCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableLensCorrections=Enable Lens Corrections"), 1160}, -- enabled in app
  EnablePaintBasedCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableBrushAdjustments=Enable Brush Adjustments"), 1170},
  EnableRedEye = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableRedEye=Enable Red-Eye"), 1180}, -- enabled in app
  EnableRetouch = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableSpotRemoval=Enable Spot Removal"), 1190}, -- enabled in app
  EnableSplitToning = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableSplitToning=Enable Split Toning"), 1200}, -- enabled in app
  LensProfileEnable = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileEnable=Enable Lens Corrections"), 1210},--1 on 0 off
  VignetteAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteAmount=Vignette Amount"), 1220}, -- 0 --this affects lens vignetting in manual tab of lens corrections
  VignetteMidpoint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteMidpoint=Vignette Midpoint"), 1230}, -- 50 --this affects lens vignetting in manual tab of lens corrections  
  WhiteBalance = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/WhiteBalance=White Balance"), 1240}, -- enabled in app
  --don't know how to control the following or whether control is even possible
  AutoBrightness = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoBrightness=Automatic Brightness"), 1250}, --true/false
  AutoContrast = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoContrast=Automatic Contrast"), 1260}, --true/false --read only?
  AutoExposure = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoExposure=Automatic Exposure"), 1270}, --true/false --read only?
  AutoShadows = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoShadows=Automatic Shadows"), 1280}, --true/false
  Brightness = {LOC("$$$/AgCameraRawUI/Brightness=Brightness"), 1290}, --50
  ChromaticAberrationB = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ChromaticAberrationB=Blue Chromatic Aberration"), 1300}, --0
  ChromaticAberrationR = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ChromaticAberrationR=Red Chromatic Aberration"), 1310}, --0
  Defringe = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Defringe=Defringe"), 1320}, --0
  LensProfileSetup = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileSetup=Lens Profile Setup"), 1330}, -- "LensDefaults"), 10}, Auto = {LOC("$$$/MIDI2LR/Parameters/Auto="), 10}, Custom = {LOC("$$$/MIDI2LR/Parameters/Custom=" -- works, but who would want this control?
  orientation = {LOC("$$$/MIDI2LR/Parameters/orientation=orientation"), 1340}, --"AB"
  ProcessVersion = {LOC("$$$/AgDevelop/Menu/ProcessVersion=Process Version"), 1350}, --"6.7"
  RedEyeInfo = {LOC("$$$/MIDI2LR/Parameters/RedEyeInfo=Red-Eye Information"), 1360}, -- {}
  RetouchInfo = {LOC("$$$/MIDI2LR/Parameters/RetouchInfo=Retouch Information"), 1370}, -- {}
  ToneCurve = {LOC("$$$/MIDI2LR/Parameters/ToneCurve=Tone Curve"), 1380}, -- {0,0,32,22,64,56,128,128,192,196,255,255} --looks to be read only?
  ToneCurveName = {LOC("$$$/AgDevelop/Menu/Settings/Target/ToneCurve=Tone Curve Name"), 1390}, -- "Medium Contrast"), 10}, "Linear"), 10}, "Strong Contrast" 
  ToneCurveName2012 = {LOC("$$$/MIDI2LR/Parameters/ToneCurveName2012=Tone Curve Name 2012"), 1400},  --"Linear"
  ToneCurvePV2012 = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012=Tone Curve PV 2012"), 1410}, -- {0,0,255,255}
  ToneCurvePV2012Blue = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012Blue=Tone Curve PV 2012 Blue"), 1420}, -- {0,0,255,255}
  ToneCurvePV2012Green = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012Green=Tone Curve PV 2012 Green"), 1430}, -- {0,0,255,255}
  ToneCurvePV2012Red = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012Red=Tone Curve PV 2012 Red"), 1440}, -- {0,0,255,255}
}

local Order = {}
for k,_ in pairs(Names) do
  table.insert(Order,k)
end
local function sorter(a,b) 
  return Names[a][2] < Names[b][2] 
end
table.sort(Order, sorter)

return {
  Names = Names,
  Order = Order
}
