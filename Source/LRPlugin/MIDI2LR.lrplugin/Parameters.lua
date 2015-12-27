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
  Temperature = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Temperature=Temperatur"), 1},
  Tint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Tint=Tönung"), 1},
  Exposure = {LOC("$$$/AgCameraRawUI/Exposure=Belichtung"), 1},
  Highlights = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Highlights=Lichter"), 1},
  Shadows = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Shadows=Tiefen"), 1},
  Contrast = {LOC("$$$/AgCameraRawUI/Contrast=Kontrast"), 1},
  Whites = {LOC("$$$/AgCameraRawUI/Whites=Weiß"), 1},
  Blacks = {LOC("$$$/AgCameraRawUI/Blacks=Schwarz"), 1},
  Clarity = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/Clarity=Klarheit"), 1},
  Vibrance = {LOC("$$$/AgCameraRawUI/Vibrance=Dynamik"), 1},
  Saturation = {LOC("$$$/AgCameraRawUI/Saturation=Sättigung"), 1},
  -- Tone
  ParametricDarks = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneDarks=Dunkle Töne"), 1},
  ParametricLights = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneLights=Helle Töne"), 1},
  ParametricShadows = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadows=Tiefen - Töne"), 1},
  ParametricHighlights = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlights=Lichter - Töne"), 1},
  ParametricShadowSplit = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadowSplit=Tiefen - teilen"), 1},
  ParametricMidtoneSplit = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneMidtoneSplit=Mitteltöne - teilen"), 1},
  ParametricHighlightSplit = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlightSplit=Lichter - teilen"), 1},
  -- Mixer
  SaturationAdjustmentRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentRed=Verschiebung Rot-Sättigung"), 1},
  SaturationAdjustmentOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentOrange=Verschiebung Orange-Sättigung"), 1},
  SaturationAdjustmentYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentYellow=Verschiebung Gelb-Sättigung"), 1},
  SaturationAdjustmentGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentGreen=Verschiebung Grün-Sättigung"), 1},
  SaturationAdjustmentAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentAqua=Verschiebung Aquamarin-Sättigung"), 1},
  SaturationAdjustmentBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentBlue=Verschiebung Blau-Sättigung"), 1},
  SaturationAdjustmentPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentPurple=Verschiebung Lila-Sättigung"), 1},
  SaturationAdjustmentMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentMagenta=Verschiebung Magenta-Sättigung"), 1},
  HueAdjustmentRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentRed=Verschiebung Rot-Farbton"), 1},
  HueAdjustmentOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentOrange=Verschiebung Orange-Farbton"), 1},
  HueAdjustmentYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentYellow=Verschiebung Gelb-Farbton"), 1},
  HueAdjustmentGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentGreen=Verschiebung Grün-Farbton"), 1},
  HueAdjustmentAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentAqua=Verschiebung Aquamarin-Farbton"), 1},
  HueAdjustmentBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentBlue=Verschiebung Blau-Farbton"), 1},
  HueAdjustmentPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentPurple=Verschiebung Lila-Farbton"), 1},
  HueAdjustmentMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentMagenta=Verschiebung Magenta-Farbton"), 1},
  LuminanceAdjustmentRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentRed=Verschiebung Rot-Luminanz"), 1},
  LuminanceAdjustmentOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentOrange=Verschiebung Orange-Luminanz"), 1},
  LuminanceAdjustmentYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentYellow=Verschiebung Gelb-Luminanz"), 1},
  LuminanceAdjustmentGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentGreen=Verschiebung Grün-Luminanz"), 1},
  LuminanceAdjustmentAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentAqua=Verschiebung Aquamarin-Luminanz"), 1},
  LuminanceAdjustmentBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentBlue=Verschiebung Blau-Luminanz"), 1},
  LuminanceAdjustmentPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentPurple=Verschiebung Lila-Luminanz"), 1},
  LuminanceAdjustmentMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentMagenta=Verschiebung Magenta-Luminanz"), 1},
  GrayMixerRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerRed=Tonwert Rot/Grau"), 1},
  GrayMixerOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerOrange=Tonwert Orange/Grau"), 1},
  GrayMixerYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerYellow=Tonwert Gelb/Grau"), 1},
  GrayMixerGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerGreen=Tonwert Grün/Grau"), 1},
  GrayMixerAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerAqua=Tonwert Aquamarin/Grau"), 1},
  GrayMixerBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerBlue=Tonwert Blau/Grau"), 1},
  GrayMixerPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerPurple=Tonwert Lila/Grau"), 1},
  GrayMixerMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerMagenta=Tonwert Magenta/Grau"), 1},
  -- Split Toning
  SplitToningShadowHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowHue=Tiefen - Farbton"), 1},
  SplitToningShadowSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowSaturation=Tiefen - Sättigung"), 1},
  SplitToningHighlightHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightHue=Lichter - Farbton"), 1},
  SplitToningHighlightSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightSaturation=Lichter - Sättigung"), 1},
  SplitToningBalance = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningBalance=Abgleich für Teiltonung"), 1},
  -- Detail
  Sharpness = {LOC("$$$/AgDevelop/Localized/Sharpness=Schärfe"), 1},
  SharpenRadius = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenRadius=Scharfzeichnen - Radius"), 1},
  SharpenDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenDetail=Scharfzeichnen - Detail"), 1},
  SharpenEdgeMasking = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenEdgeMasking=Kante - Maskierung"), 1},
  LuminanceSmoothing = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceSmoothing=Luminanzglättung"), 1},
  LuminanceNoiseReductionDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionDetail=Luminanzdetail"), 1},
  LuminanceNoiseReductionContrast = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionContrast=Luminanzkontrast"), 1},
  ColorNoiseReduction = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReduction=Farbrauschen reduzieren"), 1},
  ColorNoiseReductionDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionDetail=Farbdetail"), 1},
  ColorNoiseReductionSmoothness = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionSmoothness=Farbglättung"), 1},
  -- Lens Correction
  LensProfileDistortionScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileDistortionScale=Verzerrung - Skalieren"), 1},
  LensProfileChromaticAberrationScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileChromaticAberrationScale=Chromatische Aberration - Skalieren"), 1},
  LensProfileVignettingScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileVignettingScale=Vignettierung - Skalieren"), 1},
  LensManualDistortionAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensManualDistortionAmount=Verzerrungsbetrag"), 1},
  DefringePurpleAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeAmount=Farbsaum-Intensität Lila"), 1},
  DefringePurpleHueLo = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeHueLo=Farbsaum-Ton Lila"..' '.."$$$/PrefsWindow/PreviewQuality/Low=Niedrig"), 1},
  DefringePurpleHueHi = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeHueLo=Farbsaum-Ton Lila"..' '.."$$$/PrefsWindow/PreviewQuality/High=Hoch"), 1},
  DefringeGreenAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeAmount=Farbsaum-Intensität Grün"), 1},
  DefringeGreenHueLo = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeHueLo=Farbsaum-Ton Grün")..' '..LOC("$$$/PrefsWindow/PreviewQuality/Low=Niedrig"), 1},
  DefringeGreenHueHi = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeHueLo=Farbsaum-Ton Grün")..' '..LOC("$$$/PrefsWindow/PreviewQuality/High=Hoch"), 1},
  PerspectiveVertical = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveVertical=Vertikale Perspektive"), 1},
  PerspectiveHorizontal = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveHorizontal=Horizontale Perspektive"), 1},
  PerspectiveRotate = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveRotate=Perspektive - Drehen"), 1},
  PerspectiveScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveScale=Perspektive - Skalieren"), 1},
  PerspectiveAspect = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveAspect=Perspektivenaspekt"), 1},
  PerspectiveUpright = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveUpright=Perspektive für „Upright“"), 1},
  -- Effects
  Dehaze = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/DehazeAmount=Dunst entfernen-Stärke"), 1},
  PostCropVignetteAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteAmount=Vignettenbetrag nach Freistellen"), 1},
  PostCropVignetteMidpoint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteMidpoint=Vignettenmittelpunkt nach Freistellen"), 1},
  PostCropVignetteFeather = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteFeather=Weiche Vignettierungskante nach Freistellen"), 1},
  PostCropVignetteRoundness = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteRoundness=Vignettierungsrundheit nach Freistellen"), 1},
  PostCropVignetteStyle = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteStyle=Vignettenstil nach Freistellen"), 1},
  PostCropVignetteHighlightContrast = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteHighlightContrast=Vignettierungslichter nach Freistellen"), 1},
  GrainAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainAmount=Körnungsstärke"), 1},
  GrainSize = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainSize=Körnungsgröße"), 1},
  GrainFrequency = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainRoughness=Körnungsunregelmäßigkeit"), 1},
  -- Calibrate
  ShadowTint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ShadowTintCalibration=Kalibrierung der Schattentönung"), 1},
  RedHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedHueCalibration=Kalibrierung des Rot-Farbtons"), 1},
  RedSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedSaturationCalibration=Kalibrierung der Rot-Sättigung"), 1},
  GreenHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenHueCalibration=Kalibrierung des Grün-Farbtons"), 1},
  GreenSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenSaturationCalibration=Kalibrierung der Grün-Sättigung"), 1},
  BlueHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueHueCalibration=Blue Hue Calibration"), 1},
  BlueSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueSaturationCalibration=Blue Saturation Calibration"), 1},
  -- Experimental, shown in copy settings but not in documentation, will be enabled one-by-one after testing identifies functionality
  AutoLateralCA = {LOC("$$$/MIDI2LR/Parameters/AutoLateralCA="), 1}, --0 off 1 on  --noloc
  CameraProfile = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CameraProfile=Kameraprofil"), 1}, --text "Adobe Standard"), 1}, 'Camera Clear', Deep Landscape Light Neutral Protrait Standard Vivid  
  ConvertToGrayscale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ConvertToGrayscale=In Schwarzweiß konvertieren"), 1},  -- enabled in app
  CropAngle = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CropAngle=Freistellen - Winkel"), 1}, -- enabled in app
  CropBottom = {LOC("$$$/MIDI2LR/Parameters/CropBottom="), 1}, -- enabled in app
  CropConstrainToWarp = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CropConstrainToWarp=Zuschnitt beschränken"), 1}, --0
  CropLeft = {LOC("$$$/MIDI2LR/Parameters/CropLeft="), 1}, -- enabled in app
  CropRight = {LOC("$$$/MIDI2LR/Parameters/CropRight="), 1}, -- enabled in app
  CropTop = {LOC("$$$/MIDI2LR/Parameters/CropTop="), 1}, -- enabled in app  
  EnableCalibration = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableCalibration=Kalibrierung aktivieren"), 1},  -- enabled in app
  EnableCircularGradientBasedCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableRadialFilter=Radial-Filter aktivieren"), 1},  -- enabled in app
  EnableColorAdjustments = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableColorAdjustments=Farbkorrekturen aktivieren"), 1}, -- enabled in app
  EnableDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableDetail=Details aktivieren"), 1}, -- enabled in app
  EnableEffects = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableEffects=Effekte aktivieren"), 1}, -- enabled in app
  EnableGradientBasedCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableGraduatedFilter=Verlaufsfilter aktivieren"), 1}, -- enabled in app
  EnableGrayscaleMix = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableGrayscaleMix=Schwarzweißmischung aktivieren"), 1}, -- enabled in app
  EnableLensCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableLensCorrections=Objektivkorrekturen aktivieren"), 1}, -- enabled in app
  EnablePaintBasedCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableBrushAdjustments=Pinselkorrekturen aktivieren"), 1},
 -- enabled in app
  EnableRedEye = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableRedEye=Rote-Augen-Korrektur aktivieren"), 1}, -- enabled in app
  EnableRetouch = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableSpotRemoval=Bereichsreparatur aktivieren"), 1}, -- enabled in app
  EnableSplitToning = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableSplitToning=Teiltonung aktivieren"), 1}, -- enabled in app
  LensProfileEnable = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileEnable=Objektivprofil aktivieren"), 1},--1 on 0 off
  VignetteAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteAmount=Vignettierungsbetrag"), 1}, -- 0 --this affects lens vignetting in manual tab of lens corrections
  VignetteMidpoint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteMidpoint=Vignettierungsmittelpunkt"), 1}, -- 50 --this affects lens vignetting in manual tab of lens corrections  
  WhiteBalance = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/WhiteBalance=Weißabgleich"), 1}, -- enabled in app
  --don't know how to control the following or whether control is even possible
  AutoBrightness = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoBrightness=Automatische Helligkeit"), 1}, --true/false
  AutoContrast = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoContrast=Automatischer Kontrast"), 1}, --true/false --read only?
  AutoExposure = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoExposure=Automatische Belichtung"), 1}, --true/false --read only?
  AutoShadows = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoShadows=Automatische Schatten"), 1}, --true/false
  Brightness = {LOC("$$$/AgCameraRawUI/Brightness=Helligkeit"), 1}, --50

  ChromaticAberrationB = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ChromaticAberrationB=Blaue chromatische Aberration"), 1}, --0
  ChromaticAberrationR = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ChromaticAberrationR=Rote chromatische Aberration"), 1}, --0
  Defringe = {LOC("$$$/MIDI2LR/Parameters/Defringe="), 1}, --0
  LensProfileSetup = {LOC("$$$/MIDI2LR/Parameters/LensProfileSetup="), 1}, -- "LensDefaults"), 1}, Auto = {LOC("$$$/MIDI2LR/Parameters/Auto="), 1}, Custom = {LOC("$$$/MIDI2LR/Parameters/Custom=" -- works, but who would want this control?
  orientation = {LOC("$$$/MIDI2LR/Parameters/orientation="), 1}, --"AB"
  ProcessVersion = {LOC("$$$/MIDI2LR/Parameters/ProcessVersion="), 1}, --"6.7"
  RedEyeInfo = {LOC("$$$/MIDI2LR/Parameters/RedEyeInfo="), 1}, -- {}
  RetouchInfo = {LOC("$$$/MIDI2LR/Parameters/RetouchInfo="), 1}, -- {}
  ToneCurve = {LOC("$$$/MIDI2LR/Parameters/ToneCurve="), 1}, -- {0,0,32,22,64,56,128,128,192,196,255,255} --looks to be read only?
  ToneCurveName = {LOC("$$$/MIDI2LR/Parameters/ToneCurveName="), 1}, -- "Medium Contrast"), 1}, "Linear"), 1}, "Strong Contrast" 
  ToneCurveName2012 = {LOC("$$$/MIDI2LR/Parameters/ToneCurveName2012="), 1},  --"Linear"
  ToneCurvePV2012 = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012="), 1}, -- {0,0,255,255}
  ToneCurvePV2012Blue = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012Blue="), 1}, -- {0,0,255,255}
  ToneCurvePV2012Green = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012Green="), 1}, -- {0,0,255,255}
  ToneCurvePV2012Red = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012Red="), 1}, -- {0,0,255,255}
}

return {
  Names = Names,
}
