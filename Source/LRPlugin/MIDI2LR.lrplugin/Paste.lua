--[[----------------------------------------------------------------------------

Paste.lua

Manages paste settings for plugin
 
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
local ParamList  = require 'ParamList'
local LrView     = import 'LrView'

local Duplicates = { -- will set each to its duplicate in EndDialog
  Exposure2012 = 'Exposure',
  Highlights2012 = 'Highlights',
  Shadows2012 = 'Shadows',
  Contrast2012 = 'Contrast',
  Whites2012 = 'Whites',
  Blacks2012 = 'Blacks',
  Clarity2012 = 'Clarity',
  ToneCurveName2012 = 'ToneCurveName',
  ToneCurvePV2012 = 'ToneCurve',
}

local function StartDialog(obstable,f)
  
  local function set_reset(parmarray) --closure factory for toggle buttons
    local set = true
    return function()
      for _,p in ipairs(parmarray) do
        obstable['Paste'..p] = set
      end
      set = not set
    end
  end
  
  for k,v in pairs(ProgramPreferences.PasteList) do
    obstable['Paste'..k] = v 
  end 
  obstable.PastePopup = ProgramPreferences.PastePopup
  local selectivepastecol = {}
  do 
    local numberofcolumns = 4
    local breakpoint = math.floor(#ParamList.SelectivePasteMenu / numberofcolumns)
    for col = 1,numberofcolumns do
      selectivepastecol[col] = {}
      for i = ((col-1)*breakpoint+1),(breakpoint*col) do
        table.insert(
          selectivepastecol[col], 
          f:checkbox { title = ParamList.SelectivePasteMenu[i][2], value = LrView.bind ('Paste'..ParamList.SelectivePasteMenu[i][1]) } 
        )
      end
      selectivepastecol[col] = f:column (selectivepastecol[col]) -- prepare for use in f:row below
    end -- for col
  end
  return 
  f:row(selectivepastecol),
  f:row{ 
    f:row{
      f:push_button {
        title = LOC("$$$/AgCameraRawNamedSettings/NamedSettingsControls/CheckNone=Check none"),
        action = function ()
          for _,v in ipairs(ParamList.SelectivePasteMenu) do
            obstable['Paste'..v[1]] = false
          end 
        end,
      }, -- push_button
      f:push_button {
        title = LOC("$$$/AgCameraRawNamedSettings/NamedSettingsControls/CheckAll=Check all"
        ),
        action = function ()
          for _,v in ipairs(ParamList.SelectivePasteMenu) do
            obstable['Paste'..v[1]] = true
          end 
        end,
      } ,-- push_button 
      f:push_button {
        title = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/BasicTone=Basic Tone"),
        action = set_reset {'Exposure','Highlights','Shadows','Contrast','Whites','Blacks'},
      }, -- push button
      f:push_button {
        title = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Color=Color"),
        action = set_reset {'Vibrance','Saturation','SaturationAdjustmentRed','SaturationAdjustmentOrange','SaturationAdjustmentYellow',
          'SaturationAdjustmentGreen','SaturationAdjustmentAqua','SaturationAdjustmentBlue','SaturationAdjustmentPurple',
          'SaturationAdjustmentMagenta','HueAdjustmentRed','HueAdjustmentOrange','HueAdjustmentYellow','HueAdjustmentGreen',
          'HueAdjustmentAqua','HueAdjustmentBlue','HueAdjustmentPurple','HueAdjustmentMagenta','LuminanceAdjustmentRed',
          'LuminanceAdjustmentOrange','LuminanceAdjustmentYellow','LuminanceAdjustmentGreen','LuminanceAdjustmentAqua','LuminanceAdjustmentBlue',
          'LuminanceAdjustmentPurple','LuminanceAdjustmentMagenta'},  
      }, --push button
      f:push_button {
        title = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/NoiseReduction=Noise Reduction"),
        action = set_reset {'LuminanceSmoothing','LuminanceNoiseReductionDetail','LuminanceNoiseReductionContrast','ColorNoiseReduction',
          'ColorNoiseReductionDetail','ColorNoiseReductionSmoothness'},  
      }, --push button
      f:push_button {
        title = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/LensCorrections=Lens Corrections"),
        action = set_reset {'LensProfileDistortionScale','LensProfileChromaticAberrationScale','LensProfileVignettingScale',
          'LensManualDistortionAmount','DefringePurpleAmount','DefringePurpleHueLo','DefringePurpleHueHi','DefringeGreenAmount',
          'DefringeGreenHueLo','DefringeGreenHueHi','PerspectiveVertical','PerspectiveHorizontal','PerspectiveRotate',
          'PerspectiveScale','PerspectiveAspect','PerspectiveUpright','VignetteAmount','VignetteMidpoint','Defringe'},  
      }, --push button      
      f:push_button {
        title = LOC("$$$/AgCameraRawNamedSettings/SaveNamedDialog/Effects=Effects"),
        action = set_reset {'Dehaze','PostCropVignetteAmount','PostCropVignetteMidpoint','PostCropVignetteFeather','PostCropVignetteRoundness',
          'PostCropVignetteStyle','PostCropVignetteHighlightContrast','GrainAmount','GrainSize','GrainFrequency'},  
      }, --push button
    },-- set of pushbuttons
    f:checkbox {title = 'Ask each time', value = LrView.bind('PastePopup')}
  } --row with pushbuttons and checkbox
end

local function EndDialog(obstable, status)
  if status == 'ok' then
    --assign PasteList
    ProgramPreferences.PasteList = {} -- empty out prior settings
    for _,v in ipairs(ParamList.SelectivePasteMenu) do
      ProgramPreferences.PasteList[v[1]] = obstable['Paste'..v[1]]
    end 
    for k,v in pairs(ParamList.SelectivePasteHidden) do
      ProgramPreferences.PasteList[k] = ProgramPreferences.PasteList[v]
    end
    ProgramPreferences.PastePopup = obstable.PastePopup
  end
end

return {
  StartDialog = StartDialog,
  EndDialog = EndDialog,
  Duplicates = Duplicates,
}