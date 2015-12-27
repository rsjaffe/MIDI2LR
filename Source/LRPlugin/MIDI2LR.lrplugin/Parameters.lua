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

local Names = {
  Temperature = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Temperature=Temperature"), 1},
  Tint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Tint=Tint"), 2},
  Exposure = {LOC("$$$/AgCameraRawUI/Exposure=Exposure"), 3},
  Highlights = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Highlights=Highlights"), 4},
  Shadows = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Shadows=Shadows"), 5},
  Contrast = {LOC("$$$/AgCameraRawUI/Contrast=Contrast"), 6},
  Whites = {LOC("$$$/AgCameraRawUI/Whites=Whites"), 7},
  Blacks = {LOC("$$$/AgCameraRawUI/Blacks=Blacks"), 8},
  Clarity = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Clarity=Clarity"), 9},
  Vibrance = {LOC("$$$/AgCameraRawUI/Vibrance=Vibrance"), 10},
  Saturation = {LOC("$$$/AgCameraRawUI/Saturation=Saturation"), 10},
  -- Tone
  ParametricDarks = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneDarks=Dunkle Töne"), 12},
  ParametricLights = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneLights=Helle Töne"), 13},
  ParametricShadows = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadows=Tiefen - Töne"), 14},
  ParametricHighlights = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlights=Lichter - Töne"), 15},
  ParametricShadowSplit = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadowSplit=Tiefen - teilen"), 16},
  ParametricMidtoneSplit = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneMidtoneSplit=Mitteltöne - teilen"), 17},
  ParametricHighlightSplit = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlightSplit=Lichter - teilen"), 18},
  -- Mixer
  SaturationAdjustmentRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentRed=Verschiebung Rot-Sättigung"), 19},
  SaturationAdjustmentOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentOrange=Verschiebung Orange-Sättigung"), 20},
  SaturationAdjustmentYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentYellow=Verschiebung Gelb-Sättigung"), 21},
  SaturationAdjustmentGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentGreen=Verschiebung Grün-Sättigung"), 22},
  SaturationAdjustmentAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentAqua=Verschiebung Aquamarin-Sättigung"), 23},
  SaturationAdjustmentBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentBlue=Verschiebung Blau-Sättigung"), 24},
  SaturationAdjustmentPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentPurple=Verschiebung Lila-Sättigung"), 25},
  SaturationAdjustmentMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentMagenta=Verschiebung Magenta-Sättigung"), 26},
  HueAdjustmentRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentRed=Verschiebung Rot-Farbton"), 27},
  HueAdjustmentOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentOrange=Verschiebung Orange-Farbton"), 28},
  HueAdjustmentYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentYellow=Verschiebung Gelb-Farbton"), 29},
  HueAdjustmentGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentGreen=Verschiebung Grün-Farbton"), 30},
  HueAdjustmentAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentAqua=Verschiebung Aquamarin-Farbton"), 31},
  HueAdjustmentBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentBlue=Verschiebung Blau-Farbton"), 32},
  HueAdjustmentPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentPurple=Verschiebung Lila-Farbton"), 33},
  HueAdjustmentMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentMagenta=Verschiebung Magenta-Farbton"), 34},
  LuminanceAdjustmentRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentRed=Verschiebung Rot-Luminanz"), 35},
  LuminanceAdjustmentOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentOrange=Verschiebung Orange-Luminanz"), 36},
  LuminanceAdjustmentYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentYellow=Verschiebung Gelb-Luminanz"), 37},
  LuminanceAdjustmentGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentGreen=Verschiebung Grün-Luminanz"), 38},
  LuminanceAdjustmentAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentAqua=Verschiebung Aquamarin-Luminanz"), 39},
  LuminanceAdjustmentBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentBlue=Verschiebung Blau-Luminanz"), 40},
  LuminanceAdjustmentPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentPurple=Verschiebung Lila-Luminanz"), 41},
  LuminanceAdjustmentMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentMagenta=Verschiebung Magenta-Luminanz"), 42},
  GrayMixerRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerRed=Tonwert Rot/Grau"), 43},
  GrayMixerOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerOrange=Tonwert Orange/Grau"), 44},
  GrayMixerYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerYellow=Tonwert Gelb/Grau"), 45},
  GrayMixerGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerGreen=Tonwert Grün/Grau"), 46},
  GrayMixerAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerAqua=Tonwert Aquamarin/Grau"), 47},
  GrayMixerBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerBlue=Tonwert Blau/Grau"), 48},
  GrayMixerPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerPurple=Tonwert Lila/Grau"), 49},
  GrayMixerMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerMagenta=Tonwert Magenta/Grau"), 50},
  -- Split Toning
  SplitToningShadowHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowHue=Tiefen - Farbton"), 51},
  SplitToningShadowSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowSaturation=Tiefen - Sättigung"), 52},
  SplitToningHighlightHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightHue=Lichter - Farbton"), 53},
  SplitToningHighlightSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightSaturation=Lichter - Sättigung"), 54},
  SplitToningBalance = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningBalance=Abgleich für Teiltonung"), 55},
  -- Detail
  Sharpness = {LOC("$$$/AgDevelop/Localized/Sharpness=Schärfe"), 56},
  SharpenRadius = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenRadius=Scharfzeichnen - Radius"), 57},
  SharpenDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenDetail=Scharfzeichnen - Detail"), 58},
  SharpenEdgeMasking = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenEdgeMasking=Kante - Maskierung"), 59},
  LuminanceSmoothing = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceSmoothing=Luminanzglättung"), 60},
  LuminanceNoiseReductionDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionDetail=Luminanzdetail"), 61},
  LuminanceNoiseReductionContrast = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionContrast=Luminanzkontrast"), 62},
  ColorNoiseReduction = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReduction=Farbrauschen reduzieren"), 63},
  ColorNoiseReductionDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionDetail=Farbdetail"), 64},
  ColorNoiseReductionSmoothness = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionSmoothness=Farbglättung"), 65},
  -- Lens Correction
  LensProfileDistortionScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileDistortionScale=Verzerrung - Skalieren"), 66},
  LensProfileChromaticAberrationScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileChromaticAberrationScale=Chromatische Aberration - Skalieren"), 67},
  LensProfileVignettingScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileVignettingScale=Vignettierung - Skalieren"), 68},
  LensManualDistortionAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensManualDistortionAmount=Verzerrungsbetrag"), 69},
  DefringePurpleAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeAmount=Farbsaum-Intensität Lila"), 70},
  DefringePurpleHueLo = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeHueLo=Farbsaum-Ton Lila")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/Low=Niedrig"), 71},
  DefringePurpleHueHi = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeHueLo=Farbsaum-Ton Lila")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/High=Hoch"), 73},
  DefringeGreenAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeAmount=Farbsaum-Intensität Grün"), 74},
  DefringeGreenHueLo = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeHueLo=Farbsaum-Ton Grün")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/Low=Niedrig"), 75},
  DefringeGreenHueHi = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeHueLo=Farbsaum-Ton Grün")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/High=Hoch"), 76},
  PerspectiveVertical = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveVertical=Vertikale Perspektive"), 77},
  PerspectiveHorizontal = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveHorizontal=Horizontale Perspektive"), 78},
  PerspectiveRotate = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveRotate=Perspektive - Drehen"), 79},
  PerspectiveScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveScale=Perspektive - Skalieren"), 80},
  PerspectiveAspect = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveAspect=Perspektivenaspekt"), 81},
  PerspectiveUpright = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveUpright=Perspektive für „Upright“"), 82},
  -- Effects
  Dehaze = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/DehazeAmount=Dunst entfernen-Stärke"), 83},
  PostCropVignetteAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteAmount=Vignettenbetrag nach Freistellen"), 84},
  PostCropVignetteMidpoint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteMidpoint=Vignettenmittelpunkt nach Freistellen"), 85},
  PostCropVignetteFeather = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteFeather=Weiche Vignettierungskante nach Freistellen"), 86},
  PostCropVignetteRoundness = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteRoundness=Vignettierungsrundheit nach Freistellen"), 87},
  PostCropVignetteStyle = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteStyle=Vignettenstil nach Freistellen"), 88},
  PostCropVignetteHighlightContrast = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteHighlightContrast=Vignettierungslichter nach Freistellen"), 89},
  GrainAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainAmount=Körnungsstärke"), 90},
  GrainSize = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainSize=Körnungsgröße"), 91},
  GrainFrequency = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainRoughness=Körnungsunregelmäßigkeit"), 92},
  -- Calibrate
  ShadowTint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ShadowTintCalibration=Kalibrierung der Schattentönung"), 93},
  RedHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedHueCalibration=Kalibrierung des Rot-Farbtons"), 94},
  RedSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedSaturationCalibration=Kalibrierung der Rot-Sättigung"), 95},
  GreenHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenHueCalibration=Kalibrierung des Grün-Farbtons"), 96},
  GreenSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenSaturationCalibration=Kalibrierung der Grün-Sättigung"), 97},
  BlueHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueHueCalibration=Blue Hue Calibration"), 98},
  BlueSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueSaturationCalibration=Blue Saturation Calibration"), 99},
  -- Experimental, shown in copy settings but not in documentation, will be enabled one-by-one after testing identifies functionality
  AutoLateralCA = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RemoveChromaticAberration=Chromatische Aberration entfernen"), 100}, --0 off 1 on  --noloc
  CameraProfile = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CameraProfile=Kameraprofil"), 101}, --text "Adobe Standard"), 1}, 'Camera Clear', Deep Landscape Light Neutral Protrait Standard Vivid  
  ConvertToGrayscale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ConvertToGrayscale=In Schwarzweiß konvertieren"), 102},  -- enabled in app
  CropAngle = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CropAngle=Freistellen - Winkel"), 103}, -- enabled in app
  CropBottom = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Freistellen")..' - '.. LOC("$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Bottom=Unten"), 104}, -- enabled in app
  CropConstrainToWarp = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CropConstrainToWarp=Zuschnitt beschränken"), 105}, --0
  CropLeft = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Freistellen")..' - '..LOC("$$$/AgWatermarking/Alignment/Left=Links"), 106}, -- enabled in app
  CropRight = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Freistellen")..' - '..LOC("$$$/AgWatermarking/Alignment/Right=Rechts"), 107}, -- enabled in app
  CropTop = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Freistellen")..' - '..LOC("$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Top=Oben"), 108}, -- enabled in app  
  EnableCalibration = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableCalibration=Kalibrierung aktivieren"), 109},  -- enabled in app
  EnableCircularGradientBasedCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableRadialFilter=Radial-Filter aktivieren"), 110},  -- enabled in app
  EnableColorAdjustments = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableColorAdjustments=Farbkorrekturen aktivieren"), 111}, -- enabled in app
  EnableDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableDetail=Details aktivieren"), 112}, -- enabled in app
  EnableEffects = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableEffects=Effekte aktivieren"), 113}, -- enabled in app
  EnableGradientBasedCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableGraduatedFilter=Verlaufsfilter aktivieren"), 114}, -- enabled in app
  EnableGrayscaleMix = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableGrayscaleMix=Schwarzweißmischung aktivieren"), 115}, -- enabled in app
  EnableLensCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableLensCorrections=Objektivkorrekturen aktivieren"), 116}, -- enabled in app
  EnablePaintBasedCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableBrushAdjustments=Pinselkorrekturen aktivieren"), 117},
 -- enabled in app
  EnableRedEye = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableRedEye=Rote-Augen-Korrektur aktivieren"), 118}, -- enabled in app
  EnableRetouch = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableSpotRemoval=Bereichsreparatur aktivieren"), 119}, -- enabled in app
  EnableSplitToning = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableSplitToning=Teiltonung aktivieren"), 120}, -- enabled in app
  LensProfileEnable = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileEnable=Objektivprofil aktivieren"), 121},--1 on 0 off
  VignetteAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteAmount=Vignettierungsbetrag"), 122}, -- 0 --this affects lens vignetting in manual tab of lens corrections
  VignetteMidpoint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteMidpoint=Vignettierungsmittelpunkt"), 123}, -- 50 --this affects lens vignetting in manual tab of lens corrections  
  WhiteBalance = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/WhiteBalance=Weißabgleich"), 124}, -- enabled in app
  --don't know how to control the following or whether control is even possible
  AutoBrightness = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoBrightness=Automatische Helligkeit"), 125}, --true/false
  AutoContrast = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoContrast=Automatischer Kontrast"), 126}, --true/false --read only?
  AutoExposure = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoExposure=Automatische Belichtung"), 127}, --true/false --read only?
  AutoShadows = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoShadows=Automatische Schatten"), 128}, --true/false
  Brightness = {LOC("$$$/AgCameraRawUI/Brightness=Helligkeit"), 129}, --50

  ChromaticAberrationB = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ChromaticAberrationB=Blaue chromatische Aberration"), 130}, --0
  ChromaticAberrationR = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ChromaticAberrationR=Rote chromatische Aberration"), 131}, --0
  Defringe = {LOC("$$$/MIDI2LR/Parameters/Defringe="), 132}, --0
  LensProfileSetup = {LOC("$$$/MIDI2LR/Parameters/LensProfileSetup="), 133}, -- "LensDefaults"), 1}, Auto = {LOC("$$$/MIDI2LR/Parameters/Auto="), 1}, Custom = {LOC("$$$/MIDI2LR/Parameters/Custom=" -- works, but who would want this control?
  orientation = {LOC("$$$/MIDI2LR/Parameters/orientation="), 134}, --"AB"
  ProcessVersion = {LOC("$$$/MIDI2LR/Parameters/ProcessVersion="), 135}, --"6.7"
  RedEyeInfo = {LOC("$$$/MIDI2LR/Parameters/RedEyeInfo="), 136}, -- {}
  RetouchInfo = {LOC("$$$/MIDI2LR/Parameters/RetouchInfo="), 137}, -- {}
  ToneCurve = {LOC("$$$/MIDI2LR/Parameters/ToneCurve="), 138}, -- {0,0,32,22,64,56,128,128,192,196,255,255} --looks to be read only?
  ToneCurveName = {LOC("$$$/MIDI2LR/Parameters/ToneCurveName="), 139}, -- "Medium Contrast"), 1}, "Linear"), 1}, "Strong Contrast" 
  ToneCurveName2012 = {LOC("$$$/MIDI2LR/Parameters/ToneCurveName2012="), 140},  --"Linear"
  ToneCurvePV2012 = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012="), 141}, -- {0,0,255,255}
  ToneCurvePV2012Blue = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012Blue="), 142}, -- {0,0,255,255}
  ToneCurvePV2012Green = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012Green="), 143}, -- {0,0,255,255}
  ToneCurvePV2012Red = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012Red="), 144}, -- {0,0,255,255}
}

return {
  Names = Names,
}
