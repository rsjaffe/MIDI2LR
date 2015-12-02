--[[----------------------------------------------------------------------------

ShutDownApp.lua
Launches the app
 
This file is part of MIDI2LR. Copyright 2015 by Jeffrey Westgeest.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>. 
------------------------------------------------------------------------------]]
local LrShell = import 'LrShell'
local LrTasks = import 'LrTasks'

--LrTasks.startAsyncTask( function()
--    if(WIN_ENV) then
--        LrShell.openFilesInCommandLineProcess({_PLUGIN.path..'/Info.lua'}, _PLUGIN.path..'/MIDI2LR.exe',  '--LRSHUTDOWN')
--    else
--        LrShell.openFilesInCommandLineProcess({_PLUGIN.path..'/Info.lua'}, _PLUGIN.path..'/MIDI2LR.app', '--LRSHUTDOWN') -- On Mac it seems like the files argument has to include an existing file
--    end
--end)


return {

    LrShutdownFunction = function(doneFunction, progressFunction) 

        doneFunction = function()

            -- Uitvoeren wanneer ik klaar ben?
			
			LrTasks.startAsyncTask( function()
				if(WIN_ENV) then
					LrShell.openFilesInCommandLineProcess({_PLUGIN.path..'/Info.lua'}, _PLUGIN.path..'/MIDI2LR.exe',  '--LRSHUTDOWN')
				else
					LrShell.openFilesInCommandLineProcess({_PLUGIN.path..'/Info.lua'}, _PLUGIN.path..'/MIDI2LR.app', '--LRSHUTDOWN') -- On Mac it seems like the files argument has to include an existing file
				end
			end)
				
		end

        

        progressFunction = function(percent, description)
			
			--percent = 1
			
			--description = 'jewe testing'
            -- Uitvoeren tijdens progressie?    
			return false
        end

    end

}