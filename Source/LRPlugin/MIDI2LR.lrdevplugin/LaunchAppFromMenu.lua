--[[----------------------------------------------------------------------------

LaunchAppFromMenu.lua

Launches the app

------------------------------------------------------------------------------]]
local LrShell = import 'LrShell'
local LrTasks = import 'LrTasks'

LrTasks.startAsyncTask( function()
    if(WIN_ENV) then
        LrShell.openFilesInApp({""}, _PLUGIN.path..'/MIDI2LR.exe')
    else
        LrShell.openFilesInApp({""}, _PLUGIN.path..'/MIDI2LR.app')
    end
end)