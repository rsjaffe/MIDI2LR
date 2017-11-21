--[[------------------------------------------------------------------------------------

LocalPresets.lua

Procedures used by Client.lua 
 
This file is part of MIDI2LR. This file has been originally coded by Clifton Saulnier

---------------------------------------------------------------------------------------]]

	--Values from a local preset with all sliders set to maximum values
	--[[   
		blacks2012 = 1,
		clarity = 0,
		clarity2012 = 1,
		contrast = 0,
		contrast2012 = 1,
		defringe = 1,
		dehaze = 1,
		exposure = 0,
		exposure2012 = 1,
		highlights2012 = 1,
		luminanceNoise = 1,
		moire = 1,
		saturation = 1,
		shadows2012 = 1,
		sharpness = 1,
		temperature = 1,
		tint = 1,
		toningHue = 240,
		toningLuminance = 0,
		toningSaturation = 0,
		whites2012 = 1,
	--]]

	
local LrDevelopController = import 'LrDevelopController'	
local LrPathUtils       = import 'LrPathUtils'
local LrFileUtils		= import 'LrFileUtils'
local LrDialogs           = import 'LrDialogs'

local LocalAdjustmentPresetsPath = LrPathUtils.child(LrPathUtils.parent(LrPathUtils.getStandardFilePath ('appPrefs')) , 'Local Adjustment Presets')

local LocalPresets = {}  --Store presets in table upon reqested by user : key = filename, value = preset values table

local localPresetMap = {
		blacks2012 = "local_Blacks2012",
		clarity = "",
		clarity2012 = "local_Clarity",
		contrast = "",
		contrast2012 = "local_Contrast",
		defringe = "local_Defringe",
		dehaze = "local_Dehaze",
		exposure = "",
		exposure2012 = "local_Exposure",
		highlights2012 = "local_Highlights",
		luminanceNoise = "local_LuminanceNoise",
		moire = "local_Moire",
		saturation = "local_Saturation",
		shadows2012 = "local_Shadows",
		sharpness = "local_Sharpness",
		temperature = "local_Temperature",
		tint = "local_Tint",
		toningHue = "", --"local_ToningHue" doesn't exsist
		toningSaturation = "", --"local_ToningSaturation" doesn't exsist
		toningLuminance = "local_ToningLuminance",
		whites2012 = "local_Whites2012"
}

local function GetPresetFilenames() 
	local filenames = {}
	--Extract filename only from full paths
	for afile in LrFileUtils.files ( LocalAdjustmentPresetsPath ) do
		table.insert (filenames, LrPathUtils.leafName(afile))
		myLogger:trace (afile)
	end	
	return filenames
end

local function ApplyLocalPreset(LocalPresetName)
	local LRLocalPresetFileName = LrPathUtils.child(LocalAdjustmentPresetsPath,LocalPresetName)
	--myLogger:trace("Local Preset: " .. LRLocalPresetFileName)
	
	--Check to see if preset is already loaded by checking table... if so do not reload file.
	--Reloading template file on each request would however allow the use to update and save local preset settings in lightroom.    
	if LocalPresets[tostring(LocalPresetName)] == nil then 
		local f = io.open(LRLocalPresetFileName)
		local strPreset = f:read("*a")
		f:close()
		
		--I had to remove 'ZSTR' from the built-in .lrtemplate preset files as it would not load/execute file
		LRLocalPresetFile = loadstring(string.gsub(strPreset,'ZSTR',''))  --Loads into a function which is laeter called
		LRLocalPresetFile() --Execute the loaded file string as lua code.  This will give access to the variable 's'
	
		LocalPresets[tostring(LocalPresetName)] = s['value']	--Add the currently selected preset to the table of presets
	end

	LrDialogs.showBezel (LrPathUtils.removeExtension(LocalPresetName))
	
	--Apply preset to LR
	for param, MappedParam in pairs(localPresetMap) do	
		local value = LocalPresets[LocalPresetName][param]			
		if value == nil then value=0; end	
		if MappedParam == 'local_Exposure' then 
			value = value * 4
		else
			value = value * 100
		end
		MIDI2LR.PARAM_OBSERVER[MappedParam] = value
		LrDevelopController.setValue(MappedParam, value)
	end
end	

return {
	ApplyLocalPreset = ApplyLocalPreset,
	GetPresetFilenames = GetPresetFilenames,
}