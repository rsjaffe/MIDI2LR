--[[----------------------------------------------------------------------------

MIDI2LR_Client.lua

Receives and processes commands from MIDI2LR
Sends parameters to MIDI2LR
------------------------------------------------------------------------------]]
require 'strict.lua' -- catch some incorrect variable names
require 'Develop_Params.lua' -- global table of develop params we need to observe
local LrApplication       = import 'LrApplication'
local LrApplicationView   = import 'LrApplicationView'
local LrDevelopController = import 'LrDevelopController'
local LrFunctionContext   = import 'LrFunctionContext'
local LrSelection         = import 'LrSelection'
local LrShell             = import 'LrShell'
local LrSocket            = import 'LrSocket'
local LrTasks             = import 'LrTasks'
local LrUndo              = import 'LrUndo'

MIDI2LR = {RECEIVE_PORT = 58763, SEND_PORT = 58764, PICKUP_THRESHOLD = 4, CONTROL_MAX = 127, BUTTON_ON = 127; --constants
	LAST_PARAM = '', PARAM_OBSERVER = {}, PICKUP_ENABLED = true, SERVER = {} } --non-local but in MIDI2LR namespace

--File local function declarations (advance declared to allow it to be in scope for all calls. 
--When defining function, DO NOT USE local KEYWORD, as it will define yet another local function.
--These declaration are intended to get around some Lua gotcha's.

local develop_lerp_to_midi
local midi_lerp_to_develop
local pasteSettings
local processMessage
local sendChangedParams
local sendPresets
local startServer
local updateParam

local ACTIONS = {
    ['Pick']             = LrSelection.flagAsPick,
    ['Reject']           = LrSelection.flagAsReject,
    ['Next']             = LrSelection.nextPhoto,
    ['Prev']             = LrSelection.previousPhoto,
    ['RemoveFlag']       = LrSelection.removeFlag,
    ['IncreaseRating']   = LrSelection.increaseRating,
    ['DecreaseRating']   = LrSelection.decreaseRating,
    ['SetRating0']       = function () LrSelection.setRating(0) end,
    ['SetRating1']       = function () LrSelection.setRating(1) end,
    ['SetRating2']       = function () LrSelection.setRating(2) end,
    ['SetRating3']       = function () LrSelection.setRating(3) end,
    ['SetRating4']       = function () LrSelection.setRating(4) end,
    ['SetRating5']       = function () LrSelection.setRating(5) end,
    ['ToggleBlue']       = LrSelection.toggleBlueLabel,
    ['ToggleGreen']      = LrSelection.toggleGreenLabel,
    ['ToggleRed']        = LrSelection.toggleRedLabel,
    ['TogglePurple']     = LrSelection.togglePurpleLabel,
    ['ToggleYellow']     = LrSelection.toggleYellowLabel,
    ['ResetAll']         = LrDevelopController.resetAllDevelopAdjustments,
    ['ResetLast']        = function () LrDevelopController.resetToDefault(MIDI2LR.LAST_PARAM) end,
    ['Undo']             = LrUndo.undo,
    ['Redo']             = LrUndo.redo,
    ['ZoomInLargeStep']  = LrApplicationView.zoomIn,
    ['ZoomInSmallStep']  = LrApplicationView.zoomInSome,
    ['ZoomOutSmallStep'] = LrApplicationView.zoomOutSome,
    ['ZoomOutLargeStep'] = LrApplicationView.zoomOut,
    ['ToggleZoomOffOn']  = LrApplicationView.toggleZoom,
    ['IncrementLastDevelopParameter'] = function () LrDevelopController.increment(MIDI2LR.LAST_PARAM) end,
    ['DecrementLastDevelopParameter'] = function () LrDevelopController.decrement(MIDI2LR.LAST_PARAM) end,
    ['VirtualCopy']      = function () LrApplication.activeCatalog():createVirtualCopies() end,
    ['ToggleScreenTwo']  = LrApplicationView.toggleSecondaryDisplay,
    ['CopySettings']     = function () MIDI2LR.Copied_Settings = LrApplication.activeCatalog():getTargetPhoto():getDevelopSettings() end,
    ['PasteSettings']    = function () pasteSettings(MIDI2LR.Copied_Settings) end,
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
    ['Pickup'] = function(enabled) MIDI2LR.PICKUP_ENABLED = (enabled == 1) end,
}

function midi_lerp_to_develop(param, midi_value)
    -- map midi range to develop parameter range
    local min,max = LrDevelopController.getRange(param)
--    if(param == 'Temperature') then
--        min = 3000
--        max = 9000
--    end
    
    local result = midi_value/MIDI2LR.CONTROL_MAX * (max-min) + min
    return result
end

function develop_lerp_to_midi(param)
    -- map develop parameter range to midi range
    local min, max = LrDevelopController.getRange(param)
--    if(param == 'Temperature') then
--        min = 3000
--        max = 9000
--    end
    
    local result = (LrDevelopController.getValue(param)-min)/(max-min) * MIDI2LR.CONTROL_MAX
    return result
end

function updateParam(param, midi_value)
    -- this function does a 'pickup' type of check
    -- that is, it will ensure the develop parameter is close 
    -- to what the inputted command value is before updating it
    if LrApplicationView.getCurrentModuleName() ~= 'develop' then
            LrApplicationView.switchToModule('develop')
    end
    
    if((not MIDI2LR.PICKUP_ENABLED) or (math.abs(midi_value - develop_lerp_to_midi(param)) <= MIDI2LR.PICKUP_THRESHOLD)) then
        MIDI2LR.PARAM_OBSERVER[param] = midi_lerp_to_develop(param, midi_value)
        LrDevelopController.setValue(param, midi_lerp_to_develop(param, midi_value))
        MIDI2LR.LAST_PARAM = param
    end
end

function pasteSettings(set) --still experimental
    if LrApplicationView.getCurrentModuleName() ~= 'develop' then
            LrApplicationView.switchToModule('develop')
    end
    sendChangedParams(set) --send parameters to midi controller
    for x,v in pairs(set) do --send parameters to LR
        MIDI2LR.PARAM_OBSERVER[x] = v
        LrDevelopController.setValue(x,v)
    end
end

-- message processor
function processMessage(message)
    if type(message) == 'string' then
        -- messages are in the format 'param value'
        local _, _, param, value = string.find( message, '(%S+)%s(%S+)' )
       
        if(ACTIONS[param]) then -- perform a one time action
            if(tonumber(value) == MIDI2LR.BUTTON_ON) then ACTIONS[param]() end
        elseif(param:find('Reset') == 1) then -- perform a reset other than those explicitly coded in ACTIONS array
           if(tonumber(value) == MIDI2LR.BUTTON_ON) then LrDevelopController.resetToDefault(param:sub(6)) end
        elseif(param:find('SwToM') == 1) then -- perform a switch to module
            if(tonumber(value) == MIDI2LR.BUTTON_ON) then LrApplicationView.switchToModule(param:sub(6)) end
        elseif(param:find('ShoVw') == 1) then -- change application's view mode
            if(tonumber(value) == MIDI2LR.BUTTON_ON) then LrApplicationView.showView(param:sub(6)) end
        elseif(param:find('ShoScndVw') == 1) then -- change application's view mode
            if(tonumber(value) == MIDI2LR.BUTTON_ON) then LrApplicationView.showSecondaryView(param:sub(10)) end
        elseif(parm:find('preSet') == 1) then -- apply preset
             local TBD = 1 --replace with call to apply preset for param:sub(6)
        elseif(TOOL_ALIASES[param]) then -- switch to desired tool
            if(tonumber(value) == MIDI2LR.BUTTON_ON) then 
                if(LrDevelopController.getSelectedTool() == TOOL_ALIASES[param]) then -- toggle between the tool/loupe
                    LrDevelopController.selectTool('loupe')
                else
                    LrDevelopController.selectTool(TOOL_ALIASES[param])
                end
            end
        elseif(SETTINGS[param]) then
            SETTINGS[param](tonumber(value))
        else -- otherwise update a develop parameter
            updateParam(param, tonumber(value))
        end
    end
end

-- send changed parameters to MIDI2LR
function sendChangedParams( observer )
    for _, param in ipairs(DEVELOP_PARAMS) do
        if(observer[param] ~= LrDevelopController.getValue(param)) then
            MIDI2LR.SERVER:send(string.format('%s %g\n', param, develop_lerp_to_midi(param)))
            observer[param] = LrDevelopController.getValue(param)
            MIDI2LR.LAST_PARAM = param
        end
    end
end

-- send presets to MIDI2LR
function sendPresets()
	MIDI2LR.SERVER:send('ClearPresets')
	for _,fold in pairs(LrApplication.developPresetFolders()) do
		local foldname = fold:getName()
		for _,pst in pairs(fold:getDevelopPresets()) do
			MIDI2LR.SERVER:send('AddPreset preSet'..pst:getUuid()..'\t'..pst:getName())
		end
	end
end

function startServer(context)
    MIDI2LR.SERVER = LrSocket.bind {
          functionContext = context,
          plugin = _PLUGIN,
          port = MIDI2LR.SEND_PORT,
          mode = 'send',
          onConnected = sendPresets,
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
        LrDevelopController.addAdjustmentChangeObserver( context, MIDI2LR.PARAM_OBSERVER, sendChangedParams )
        
        local client = LrSocket.bind {
            functionContext = context,
            plugin = _PLUGIN,
            port = MIDI2LR.RECEIVE_PORT,
            mode = 'receive',
            onMessage = function(socket, message)
                processMessage(message)
            end,
            onClosed = function( socket )
                -- MIDI2LR closed connection, allow for reconnection
                socket:reconnect()
                
                -- calling SERVER:reconnect causes LR to hang for some reason...
                MIDI2LR.SERVER:close()
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
        MIDI2LR.SERVER:close()
    end )
 end )
 
LrTasks.startAsyncTask( function()
    if(WIN_ENV) then
        LrShell.openFilesInApp({_PLUGIN.path..'/Info.lua'}, _PLUGIN.path..'/MIDI2LR.exe')
    else
        LrShell.openFilesInApp({_PLUGIN.path..'/Info.lua'}, _PLUGIN.path..'/MIDI2LR.app') -- On Mac it seems like the files argument has to include an existing file
    end
end)
