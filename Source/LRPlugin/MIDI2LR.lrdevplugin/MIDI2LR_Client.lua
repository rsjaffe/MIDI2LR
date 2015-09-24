--[[----------------------------------------------------------------------------

MIDI2LR_Client.lua

Receives and processes commands from MIDI2LR
Sends parameters to MIDI2LR
------------------------------------------------------------------------------]]
local LrDevelopController = import 'LrDevelopController'
local LrApplicationView   = import 'LrApplicationView'
local LrSocket            = import 'LrSocket'
local LrTasks             = import 'LrTasks'
local LrFunctionContext   = import 'LrFunctionContext'
local LrSelection         = import 'LrSelection'
local LrShell             = import 'LrShell'
local LrUndo              = import 'LrUndo'

-- Global consts
local RECEIVE_PORT = 58763
local SEND_PORT    = 58764
local PICKUP_THRESHOLD = 4
require 'Develop_Params.lua' -- global table of develop params we need to observe

-- Global vars
local SERVER = {}
local PARAM_OBSERVER = {}
local PICKUP_ENABLED = true
local LAST_PARAM = ''

local function midi_lerp_to_develop(param, midi_value)
    -- map midi range to develop parameter range
    local min,max = LrDevelopController.getRange(param)
    if(param == 'Temperature' and max > 9000) then
        min = 3000
        max = 9000
    end
    
    local result = midi_value/127 * (max-min) + min
    return result
end

local function develop_lerp_to_midi(param)
    -- map develop parameter range to midi range
    local min, max = LrDevelopController.getRange(param)
    if(param == 'Temperature' and max > 9000) then
        min = 3000
        max = 9000
    end
    
    local result = (LrDevelopController.getValue(param)-min)/(max-min) * 127
    return result
end

local function updateParam(param, midi_value)
    -- this function does a 'pickup' type of check
    -- that is, it will ensure the develop parameter is close 
    -- to what the inputted command value is before updating it
    if LrApplicationView.getCurrentModuleName() ~= 'develop' then
            LrApplicationView.switchToModule('develop')
    end
    
    if((not PICKUP_ENABLED) or (math.abs(midi_value - develop_lerp_to_midi(param)) <= PICKUP_THRESHOLD)) then
        PARAM_OBSERVER[param] = midi_lerp_to_develop(param, midi_value)
        LrDevelopController.setValue(param, midi_lerp_to_develop(param, midi_value))
        LAST_PARAM = param
    end
end

local ACTIONS = {
    ['Pick']       = function () LrSelection.flagAsPick() end,
    ['Reject']     = function () LrSelection.flagAsReject() end,
    ['Next']       = function () LrSelection.nextPhoto() end,
    ['Prev']       = function () LrSelection.previousPhoto() end,
    ['RemoveFlag'] = function () LrSelection.removeFlag() end,
    ['IncreaseRating']   = function () LrSelection.increaseRating() end,
    ['DecreaseRating']   = function () LrSelection.decreaseRating() end,
    ['SetRating0']       = function () LrSelection.setRating(0) end,
    ['SetRating1']       = function () LrSelection.setRating(1) end,
    ['SetRating2']       = function () LrSelection.setRating(2) end,
    ['SetRating3']       = function () LrSelection.setRating(3) end,
    ['SetRating4']       = function () LrSelection.setRating(4) end,
    ['SetRating5']       = function () LrSelection.setRating(5) end,
    ['ToggleBlue']       = function () LrSelection.toggleBlueLabel() end,
    ['ToggleGreen']      = function () LrSelection.toggleGreenLabel() end,
    ['ToggleRed']        = function () LrSelection.toggleRedLabel() end,
    ['TogglePurple']     = function () LrSelection.togglePurpleLabel() end,
    ['ToggleYellow']     = function () LrSelection.toggleYellowLabel() end,
    ['ResetAll']         = function () LrDevelopController.resetAllDevelopAdjustments() end,
    ['ResetLast']        = function () LrDevelopController.resetToDefault(LAST_PARAM) end,
    ['Undo']             = function () LrUndo.undo() end,
    ['Redo']             = function () LrUndo.redo() end,
    ['ResetTemperature'] = function () LrDevelopController.resetToDefault('Temperature') end,
    ['ResetTint']        = function () LrDevelopController.resetToDefault('Tint') end,
    ['ResetExposure']    = function () LrDevelopController.resetToDefault('Exposure') end,
    ['ResetHighlights']  = function () LrDevelopController.resetToDefault('Highlights') end,
    ['ResetShadows']     = function () LrDevelopController.resetToDefault('Shadows') end,
    ['ResetContrast']    = function () LrDevelopController.resetToDefault('Contrast') end,
    ['ResetWhites']      = function () LrDevelopController.resetToDefault('Whites') end,
    ['ResetBlacks']      = function () LrDevelopController.resetToDefault('Blacks') end,
    ['ResetClarity']     = function () LrDevelopController.resetToDefault('Clarity') end,
    ['ResetVibrance']    = function () LrDevelopController.resetToDefault('Vibrance') end,
    ['ResetSaturation']  = function () LrDevelopController.resetToDefault('Saturation') end,
    ['ResetParametricDarks']                 = function () LrDevelopController.resetToDefault('ParametricDarks') end,
    ['ResetParametricLights']                = function () LrDevelopController.resetToDefault('ParametricLights') end,
    ['ResetParametricShadows']               = function () LrDevelopController.resetToDefault('ParametricShadows') end,
    ['ResetParametricHighlights']            = function () LrDevelopController.resetToDefault('ParametricHighlights') end,
    ['ResetParametricShadowSplit']           = function () LrDevelopController.resetToDefault('ParametricShadowSplit') end,
    ['ResetParametricMidtoneSplit']          = function () LrDevelopController.resetToDefault('ParametricMidtoneSplit') end,
    ['ResetParametricHighlightSplit']        = function () LrDevelopController.resetToDefault('ParametricHighlightSplit') end,
    ['ResetSaturationAdjustmentRed']         = function () LrDevelopController.resetToDefault('SaturationAdjustmentRed') end,
    ['ResetSaturationAdjustmentOrange']      = function () LrDevelopController.resetToDefault('SaturationAdjustmentOrange') end,
    ['ResetSaturationAdjustmentYellow']      = function () LrDevelopController.resetToDefault('SaturationAdjustmentYellow') end,
    ['ResetSaturationAdjustmentGreen']       = function () LrDevelopController.resetToDefault('SaturationAdjustmentGreen') end,
    ['ResetSaturationAdjustmentAqua']        = function () LrDevelopController.resetToDefault('SaturationAdjustmentAqua') end,
    ['ResetSaturationAdjustmentBlue']        = function () LrDevelopController.resetToDefault('SaturationAdjustmentBlue') end,
    ['ResetSaturationAdjustmentPurple']      = function () LrDevelopController.resetToDefault('SaturationAdjustmentPurple') end,
    ['ResetSaturationAdjustmentMagenta']     = function () LrDevelopController.resetToDefault('SaturationAdjustmentMagenta') end,
    ['ResetHueAdjustmentRed']                = function () LrDevelopController.resetToDefault('HueAdjustmentRed') end,
    ['ResetHueAdjustmentOrange']             = function () LrDevelopController.resetToDefault('HueAdjustmentOrange') end,
    ['ResetHueAdjustmentYellow']             = function () LrDevelopController.resetToDefault('HueAdjustmentYellow') end,
    ['ResetHueAdjustmentGreen']              = function () LrDevelopController.resetToDefault('HueAdjustmentGreen') end,
    ['ResetHueAdjustmentAqua']               = function () LrDevelopController.resetToDefault('HueAdjustmentAqua') end,
    ['ResetHueAdjustmentBlue']               = function () LrDevelopController.resetToDefault('HueAdjustmentBlue') end,
    ['ResetHueAdjustmentPurple']             = function () LrDevelopController.resetToDefault('HueAdjustmentPurple') end,
    ['ResetHueAdjustmentMagenta']            = function () LrDevelopController.resetToDefault('HueAdjustmentMagenta') end,
    ['ResetLuminanceAdjustmentRed']          = function () LrDevelopController.resetToDefault('LuminanceAdjustmentRed') end,
    ['ResetLuminanceAdjustmentOrange']       = function () LrDevelopController.resetToDefault('LuminanceAdjustmentOrange') end,
    ['ResetLuminanceAdjustmentYellow']       = function () LrDevelopController.resetToDefault('LuminanceAdjustmentYellow') end,
    ['ResetLuminanceAdjustmentGreen']        = function () LrDevelopController.resetToDefault('LuminanceAdjustmentGreen') end,
    ['ResetLuminanceAdjustmentAqua']         = function () LrDevelopController.resetToDefault('LuminanceAdjustmentAqua') end,
    ['ResetLuminanceAdjustmentBlue']         = function () LrDevelopController.resetToDefault('LuminanceAdjustmentBlue') end,
    ['ResetLuminanceAdjustmentPurple']       = function () LrDevelopController.resetToDefault('LuminanceAdjustmentPurple') end,
    ['ResetLuminanceAdjustmentMagenta'']     = function () LrDevelopController.resetToDefault('LuminanceAdjustmentMagenta') end,
    ['ResetGrayMixerRed']                    = function () LrDevelopController.resetToDefault('GrayMixerRed') end,
    ['ResetGrayMixerOrange']                 = function () LrDevelopController.resetToDefault('GrayMixerOrange') end,
    ['ResetGrayMixerYellow']                 = function () LrDevelopController.resetToDefault('GrayMixerYellow') end,
    ['ResetGrayMixerGreen']                  = function () LrDevelopController.resetToDefault('GrayMixerGreen') end,
    ['ResetGrayMixerAqua']                   = function () LrDevelopController.resetToDefault('GrayMixerAqua') end,
    ['ResetGrayMixerBlue']                   = function () LrDevelopController.resetToDefault('GrayMixerBlue') end,
    ['ResetGrayMixerPurple']                 = function () LrDevelopController.resetToDefault('GrayMixerPurple') end,
    ['ResetGrayMixerMagenta']                = function () LrDevelopController.resetToDefault('GrayMixerMagenta') end,
    ['ResetSharpness']                       = function () LrDevelopController.resetToDefault('Sharpness') end,
    ['ResetSharpenRadius']                   = function () LrDevelopController.resetToDefault('SharpenRadius') end,
    ['ResetSharpenDetail']                   = function () LrDevelopController.resetToDefault('SharpenDetail') end,
    ['ResetSharpenEdgeMasking']              = function () LrDevelopController.resetToDefault('SharpenEdgeMasking') end,
    ['ResetLuminanceSmoothing']              = function () LrDevelopController.resetToDefault('LuminanceSmoothing') end,
    ['ResetLuminanceNoiseReductionDetail']   = function () LrDevelopController.resetToDefault('LuminanceNoiseReductionDetail') end,
    ['ResetLuminanceNoiseReductionContrast'] = function () LrDevelopController.resetToDefault('LuminanceNoiseReductionContrast') end,
    ['ResetColorNoiseReduction']             = function () LrDevelopController.resetToDefault('ColorNoiseReduction') end,
    ['ResetColorNoiseReductionDetail']       = function () LrDevelopController.resetToDefault('ColorNoiseReductionDetail') end,
    ['ResetColorNoiseReductionSmoothness']   = function () LrDevelopController.resetToDefault('ColorNoiseReductionSmoothness') end,
    ['ResetLensProfileDistortionScale']      = function () LrDevelopController.resetToDefault('LensProfileDistortionScale') end,
    ['ResetLensProfileChromaticAberrationScale']   = function () LrDevelopController.resetToDefault('LensProfileChromaticAberrationScale') end,
    ['ResetLensProfileVignettingScale']      = function () LrDevelopController.resetToDefault('LensProfileVignettingScale') end,
    ['ResetLensManualDistortionAmount']      = function () LrDevelopController.resetToDefault('LensManualDistortionAmount') end,
    ['ResetDefringePurpleAmount']            = function () LrDevelopController.resetToDefault('DefringePurpleAmount') end,
    ['ResetDefringePurpleHueLo']             = function () LrDevelopController.resetToDefault('DefringePurpleHueLo') end,
    ['ResetDefringePurpleHueHi']             = function () LrDevelopController.resetToDefault('DefringePurpleHueHi') end,
    ['ResetDefringeGreenAmount']             = function () LrDevelopController.resetToDefault('DefringeGreenAmount') end,
    ['ResetDefringeGreenHueLo']              = function () LrDevelopController.resetToDefault('DefringeGreenHueLo') end,
    ['ResetDefringeGreenHueHi']              = function () LrDevelopController.resetToDefault('DefringeGreenHueHi') end,
    ['ResetPerspectiveVertical']             = function () LrDevelopController.resetToDefault('PerspectiveVertical') end,
    ['ResetPerspectiveHorizontal']           = function () LrDevelopController.resetToDefault('PerspectiveHorizontal') end,
    ['ResetPerspectiveRotate']               = function () LrDevelopController.resetToDefault('PerspectiveRotate') end,
    ['ResetPerspectiveScale']                = function () LrDevelopController.resetToDefault('PerspectiveScale') end,
    ['ResetPerspectiveAspect']               = function () LrDevelopController.resetToDefault('PerspectiveAspect') end,
    ['ResetPerspectiveUpright']              = function () LrDevelopController.resetToDefault('PerspectiveUpright') end,
    ['ResetDehaze']                          = function () LrDevelopController.resetToDefault('Dehaze') end,
}

local TOOL_ALIASES = {
    ['Loupe']           = 'loupe',
    ['CropOverlay']     = 'crop',
    ['SpotRemoval']     = 'dust',
    ['RedEye']          = 'redeye',
    ['GraduatedFilter'] = 'gradient',
    ['RadialFilter']    = 'circularGradient',
    ['AdjustmentBrush'] = 'localized',
}

local SETTINGS = {
    ['Pickup'] = function(enabled) PICKUP_ENABLED = (enabled == 1) end,
}

-- message processor
local function processMessage(message)
    if type(message) == 'string' then
        -- messages are in the format 'param value'
        local _, _, param, value = string.find( message, '(%S+)%s(%d+)' )
       
        if(ACTIONS[param] ~= nil) then -- perform a one time action
            if(tonumber(value) == 127 or tonumber(value) == 1) then ACTIONS[param]() end
        elseif(TOOL_ALIASES[param] ~= nil) then -- switch to desired tool
            if(tonumber(value) == 127 or tonumber(value) == 1) then 
                if(LrDevelopController.getSelectedTool() == TOOL_ALIASES[param]) then -- toggle between the tool/loupe
                    LrDevelopController.selectTool('loupe')
                else
                    LrDevelopController.selectTool(TOOL_ALIASES[param])
                end
            end
        elseif(SETTINGS[param] ~= nil) then
            SETTINGS[param](tonumber(value))
        else -- otherwise update a develop parameter
            updateParam(param, tonumber(value))
        end
    end
end

-- send changed parameters to MIDI2LR
local function sendChangedParams( observer )
    for _, param in ipairs(DEVELOP_PARAMS) do
        if(observer[param] ~= LrDevelopController.getValue(param)) then
            SERVER:send(string.format('%s %d\n', param, develop_lerp_to_midi(param)))
            observer[param] = LrDevelopController.getValue(param)
            LAST_PARAM = param
        end
    end
end

local function startServer(context)
    SERVER = LrSocket.bind {
          functionContext = context,
          plugin = _PLUGIN,
          port = SEND_PORT,
          mode = 'send',
          onClosed = function( socket ) -- this callback never seems to get called...
            -- MIDI2LR closed connection, allow for reconnection
            -- socket:reconnect()
          end,
          onError = function( socket, err )
            socket:reconnect()
          end,
        }
end

-- Main task
LrTasks.startAsyncTask( function()
    LrFunctionContext.callWithContext( 'socket_remote', function( context )
        LrDevelopController.revealAdjustedControls( true ) -- reveal affected parameter in panel track
        
        -- add an observer for develop param changes
        LrDevelopController.addAdjustmentChangeObserver( context, PARAM_OBSERVER, sendChangedParams )
        
        local client = LrSocket.bind {
            functionContext = context,
            plugin = _PLUGIN,
            port = RECEIVE_PORT,
            mode = 'receive',
            onMessage = function(socket, message)
                processMessage(message)
            end,
            onClosed = function( socket )
                -- MIDI2LR closed connection, allow for reconnection
                socket:reconnect()
                
                -- calling SERVER:reconnect causes LR to hang for some reason...
                SERVER:close()
                startServer(context)
            end,
            onError = function(socket, err)
                if err == 'timeout' then -- reconnect if timed out
                    socket:reconnect()
                end
            end
        }
        
        startServer(context)
        
        while true do
            LrTasks.sleep( 1/2 )
        end
        
        client:close()
        SERVER:close()
    end )
 end )
 
LrTasks.startAsyncTask( function()
    if(WIN_ENV) then
        LrShell.openFilesInApp({_PLUGIN.path..'/Info.lua'}, _PLUGIN.path..'/MIDI2LR.exe')
    else
        LrShell.openFilesInApp({_PLUGIN.path..'/Info.lua'}, _PLUGIN.path..'/MIDI2LR.app') -- On Mac it seems like the files argument has to include an existing file
    end
end)
