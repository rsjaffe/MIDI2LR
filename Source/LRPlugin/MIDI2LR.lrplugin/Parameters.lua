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
local function LOC(a) return a end
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
  ParametricDarks = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneDarks=Dunkle Töne"), 120},
  ParametricLights = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneLights=Helle Töne"), 130},
  ParametricShadows = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadows=Tiefen - Töne"), 140},
  ParametricHighlights = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlights=Lichter - Töne"), 150},
  ParametricShadowSplit = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneShadowSplit=Tiefen - teilen"), 160},
  ParametricMidtoneSplit = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneMidtoneSplit=Mitteltöne - teilen"), 170},
  ParametricHighlightSplit = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ToneHighlightSplit=Lichter - teilen"), 180},
  -- Mixer
  SaturationAdjustmentRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentRed=Verschiebung Rot-Sättigung"), 190},
  SaturationAdjustmentOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentOrange=Verschiebung Orange-Sättigung"), 200},
  SaturationAdjustmentYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentYellow=Verschiebung Gelb-Sättigung"), 210},
  SaturationAdjustmentGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentGreen=Verschiebung Grün-Sättigung"), 220},
  SaturationAdjustmentAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentAqua=Verschiebung Aquamarin-Sättigung"), 230},
  SaturationAdjustmentBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentBlue=Verschiebung Blau-Sättigung"), 240},
  SaturationAdjustmentPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentPurple=Verschiebung Lila-Sättigung"), 250},
  SaturationAdjustmentMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SaturationAdjustmentMagenta=Verschiebung Magenta-Sättigung"), 260},
  HueAdjustmentRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentRed=Verschiebung Rot-Farbton"), 270},
  HueAdjustmentOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentOrange=Verschiebung Orange-Farbton"), 280},
  HueAdjustmentYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentYellow=Verschiebung Gelb-Farbton"), 290},
  HueAdjustmentGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentGreen=Verschiebung Grün-Farbton"), 300},
  HueAdjustmentAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentAqua=Verschiebung Aquamarin-Farbton"), 310},
  HueAdjustmentBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentBlue=Verschiebung Blau-Farbton"), 320},
  HueAdjustmentPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentPurple=Verschiebung Lila-Farbton"), 330},
  HueAdjustmentMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/HueAdjustmentMagenta=Verschiebung Magenta-Farbton"), 340},
  LuminanceAdjustmentRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentRed=Verschiebung Rot-Luminanz"), 350},
  LuminanceAdjustmentOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentOrange=Verschiebung Orange-Luminanz"), 360},
  LuminanceAdjustmentYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentYellow=Verschiebung Gelb-Luminanz"), 370},
  LuminanceAdjustmentGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentGreen=Verschiebung Grün-Luminanz"), 380},
  LuminanceAdjustmentAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentAqua=Verschiebung Aquamarin-Luminanz"), 390},
  LuminanceAdjustmentBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentBlue=Verschiebung Blau-Luminanz"), 400},
  LuminanceAdjustmentPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentPurple=Verschiebung Lila-Luminanz"), 410},
  LuminanceAdjustmentMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceAdjustmentMagenta=Verschiebung Magenta-Luminanz"), 420},
  GrayMixerRed = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerRed=Tonwert Rot/Grau"), 430},
  GrayMixerOrange = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerOrange=Tonwert Orange/Grau"), 440},
  GrayMixerYellow = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerYellow=Tonwert Gelb/Grau"), 450},
  GrayMixerGreen = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerGreen=Tonwert Grün/Grau"), 460},
  GrayMixerAqua = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerAqua=Tonwert Aquamarin/Grau"), 470},
  GrayMixerBlue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerBlue=Tonwert Blau/Grau"), 480},
  GrayMixerPurple = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerPurple=Tonwert Lila/Grau"), 490},
  GrayMixerMagenta = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrayMixerMagenta=Tonwert Magenta/Grau"), 500},
  -- Split Toning
  SplitToningShadowHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowHue=Tiefen - Farbton"), 510},
  SplitToningShadowSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningShadowSaturation=Tiefen - Sättigung"), 520},
  SplitToningHighlightHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightHue=Lichter - Farbton"), 530},
  SplitToningHighlightSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningHighlightSaturation=Lichter - Sättigung"), 540},
  SplitToningBalance = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SplitToningBalance=Abgleich für Teiltonung"), 550},
  -- Detail
  Sharpness = {LOC("$$$/AgDevelop/Localized/Sharpness=Schärfe"), 560},
  SharpenRadius = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenRadius=Scharfzeichnen - Radius"), 570},
  SharpenDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenDetail=Scharfzeichnen - Detail"), 580},
  SharpenEdgeMasking = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SharpenEdgeMasking=Kante - Maskierung"), 590},
  LuminanceSmoothing = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceSmoothing=Luminanzglättung"), 600},
  LuminanceNoiseReductionDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionDetail=Luminanzdetail"), 610},
  LuminanceNoiseReductionContrast = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LuminanceNoiseReductionContrast=Luminanzkontrast"), 620},
  ColorNoiseReduction = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReduction=Farbrauschen reduzieren"), 630},
  ColorNoiseReductionDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionDetail=Farbdetail"), 640},
  ColorNoiseReductionSmoothness = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ColorNoiseReductionSmoothness=Farbglättung"), 650},
  -- Lens Correction
  LensProfileDistortionScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileDistortionScale=Verzerrung - Skalieren"), 660},
  LensProfileChromaticAberrationScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileChromaticAberrationScale=Chromatische Aberration - Skalieren"), 670},
  LensProfileVignettingScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileVignettingScale=Vignettierung - Skalieren"), 680},
  LensManualDistortionAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensManualDistortionAmount=Verzerrungsbetrag"), 690},
  DefringePurpleAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeAmount=Farbsaum-Intensität Lila"), 700},
  DefringePurpleHueLo = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeHueLo=Farbsaum-Ton Lila")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/Low=Niedrig"), 710},
  DefringePurpleHueHi = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PurpleDefringeHueLo=Farbsaum-Ton Lila")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/High=Hoch"), 730},
  DefringeGreenAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeAmount=Farbsaum-Intensität Grün"), 740},
  DefringeGreenHueLo = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeHueLo=Farbsaum-Ton Grün")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/Low=Niedrig"), 750},
  DefringeGreenHueHi = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenDefringeHueLo=Farbsaum-Ton Grün")..' - '..LOC("$$$/Slideshow/Panels/PanAndZoom/High=Hoch"), 760},
  PerspectiveVertical = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveVertical=Vertikale Perspektive"), 770},
  PerspectiveHorizontal = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveHorizontal=Horizontale Perspektive"), 780},
  PerspectiveRotate = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveRotate=Perspektive - Drehen"), 790},
  PerspectiveScale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveScale=Perspektive - Skalieren"), 800},
  PerspectiveAspect = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveAspect=Perspektivenaspekt"), 810},
  PerspectiveUpright = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PerspectiveUpright=Perspektive für „Upright“"), 820},
  -- Effects
  Dehaze = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/DehazeAmount=Dunst entfernen-Stärke"), 830},
  PostCropVignetteAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteAmount=Vignettenbetrag nach Freistellen"), 840},
  PostCropVignetteMidpoint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteMidpoint=Vignettenmittelpunkt nach Freistellen"), 850},
  PostCropVignetteFeather = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteFeather=Weiche Vignettierungskante nach Freistellen"), 860},
  PostCropVignetteRoundness = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteRoundness=Vignettierungsrundheit nach Freistellen"), 870},
  PostCropVignetteStyle = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteStyle=Vignettenstil nach Freistellen"), 880},
  PostCropVignetteHighlightContrast = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/PostCropVignetteHighlightContrast=Vignettierungslichter nach Freistellen"), 890},
  GrainAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainAmount=Körnungsstärke"), 900},
  GrainSize = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainSize=Körnungsgröße"), 910},
  GrainFrequency = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GrainRoughness=Körnungsunregelmäßigkeit"), 920},
  -- Calibrate
  ShadowTint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ShadowTintCalibration=Kalibrierung der Schattentönung"), 930},
  RedHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedHueCalibration=Kalibrierung des Rot-Farbtons"), 940},
  RedSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RedSaturationCalibration=Kalibrierung der Rot-Sättigung"), 950},
  GreenHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenHueCalibration=Kalibrierung des Grün-Farbtons"), 960},
  GreenSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/GreenSaturationCalibration=Kalibrierung der Grün-Sättigung"), 970},
  BlueHue = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueHueCalibration=Blue Hue Calibration"), 980},
  BlueSaturation = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/BlueSaturationCalibration=Blue Saturation Calibration"), 990},
  -- Experimental, shown in copy settings but not in documentation, will be enabled one-by-one after testing identifies functionality
  AutoLateralCA = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/RemoveChromaticAberration=Chromatische Aberration entfernen"), 1000}, --0 off 1 on  --noloc
  CameraProfile = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CameraProfile=Kameraprofil"), 1010}, --text "Adobe Standard"), 10}, 'Camera Clear', Deep Landscape Light Neutral Protrait Standard Vivid  
  ConvertToGrayscale = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ConvertToGrayscale=In Schwarzweiß konvertieren"), 1020},  -- enabled in app
  CropAngle = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CropAngle=Freistellen - Winkel"), 1030}, -- enabled in app
  CropBottom = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Freistellen")..' - '.. LOC("$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Bottom=Unten"), 1040}, -- enabled in app
  CropConstrainToWarp = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/CropConstrainToWarp=Zuschnitt beschränken"), 1050}, --0
  CropLeft = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Freistellen")..' - '..LOC("$$$/AgWatermarking/Alignment/Left=Links"), 1060}, -- enabled in app
  CropRight = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Freistellen")..' - '..LOC("$$$/AgWatermarking/Alignment/Right=Rechts"), 1070}, -- enabled in app
  CropTop = {LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Crop=Freistellen")..' - '..LOC("$$$/Layout/Panel/Panel/OutputFormat/PageNumber/Top=Oben"), 1080}, -- enabled in app  
  EnableCalibration = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableCalibration=Kalibrierung aktivieren"), 1090},  -- enabled in app
  EnableCircularGradientBasedCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableRadialFilter=Radial-Filter aktivieren"), 1100},  -- enabled in app
  EnableColorAdjustments = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableColorAdjustments=Farbkorrekturen aktivieren"), 1110}, -- enabled in app
  EnableDetail = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableDetail=Details aktivieren"), 1120}, -- enabled in app
  EnableEffects = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableEffects=Effekte aktivieren"), 1130}, -- enabled in app
  EnableGradientBasedCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableGraduatedFilter=Verlaufsfilter aktivieren"), 1140}, -- enabled in app
  EnableGrayscaleMix = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableGrayscaleMix=Schwarzweißmischung aktivieren"), 1150}, -- enabled in app
  EnableLensCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableLensCorrections=Objektivkorrekturen aktivieren"), 1160}, -- enabled in app
  EnablePaintBasedCorrections = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableBrushAdjustments=Pinselkorrekturen aktivieren"), 1170},
  -- enabled in app
  EnableRedEye = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableRedEye=Rote-Augen-Korrektur aktivieren"), 1180}, -- enabled in app
  EnableRetouch = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableSpotRemoval=Bereichsreparatur aktivieren"), 1190}, -- enabled in app
  EnableSplitToning = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/EnableSplitToning=Teiltonung aktivieren"), 1200}, -- enabled in app
  LensProfileEnable = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/LensProfileEnable=Objektivprofil aktivieren"), 1210},--1 on 0 off
  VignetteAmount = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteAmount=Vignettierungsbetrag"), 1220}, -- 0 --this affects lens vignetting in manual tab of lens corrections
  VignetteMidpoint = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/VignetteMidpoint=Vignettierungsmittelpunkt"), 1230}, -- 50 --this affects lens vignetting in manual tab of lens corrections  
  WhiteBalance = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/WhiteBalance=Weißabgleich"), 1240}, -- enabled in app
  --don't know how to control the following or whether control is even possible
  AutoBrightness = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoBrightness=Automatische Helligkeit"), 1250}, --true/false
  AutoContrast = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoContrast=Automatischer Kontrast"), 1260}, --true/false --read only?
  AutoExposure = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoExposure=Automatische Belichtung"), 1270}, --true/false --read only?
  AutoShadows = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/AutoShadows=Automatische Schatten"), 1280}, --true/false
  Brightness = {LOC("$$$/AgCameraRawUI/Brightness=Helligkeit"), 1290}, --50

  ChromaticAberrationB = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ChromaticAberrationB=Blaue chromatische Aberration"), 1300}, --0
  ChromaticAberrationR = {LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/ChromaticAberrationR=Rote chromatische Aberration"), 1310}, --0
  Defringe = {LOC("$$$/MIDI2LR/Parameters/Defringe="), 1320}, --0
  LensProfileSetup = {LOC("$$$/MIDI2LR/Parameters/LensProfileSetup="), 1330}, -- "LensDefaults"), 10}, Auto = {LOC("$$$/MIDI2LR/Parameters/Auto="), 10}, Custom = {LOC("$$$/MIDI2LR/Parameters/Custom=" -- works, but who would want this control?
  orientation = {LOC("$$$/MIDI2LR/Parameters/orientation="), 1340}, --"AB"
  ProcessVersion = {LOC("$$$/MIDI2LR/Parameters/ProcessVersion="), 1350}, --"6.7"
  RedEyeInfo = {LOC("$$$/MIDI2LR/Parameters/RedEyeInfo="), 1360}, -- {}
  RetouchInfo = {LOC("$$$/MIDI2LR/Parameters/RetouchInfo="), 1370}, -- {}
  ToneCurve = {LOC("$$$/MIDI2LR/Parameters/ToneCurve="), 1380}, -- {0,0,32,22,64,56,128,128,192,196,255,255} --looks to be read only?
  ToneCurveName = {LOC("$$$/MIDI2LR/Parameters/ToneCurveName="), 1390}, -- "Medium Contrast"), 10}, "Linear"), 10}, "Strong Contrast" 
  ToneCurveName2012 = {LOC("$$$/MIDI2LR/Parameters/ToneCurveName2012="), 1400},  --"Linear"
  ToneCurvePV2012 = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012="), 1410}, -- {0,0,255,255}
  ToneCurvePV2012Blue = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012Blue="), 1420}, -- {0,0,255,255}
  ToneCurvePV2012Green = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012Green="), 1430}, -- {0,0,255,255}
  ToneCurvePV2012Red = {LOC("$$$/MIDI2LR/Parameters/ToneCurvePV2012Red="), 1440}, -- {0,0,255,255}
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
