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

function sleep(s)
  local ntime = os.time() + s
  repeat until os.time() > ntime
end

return {
		
		LrShutdownFunction = function(doneFunction, progressFunction)  			
						
			local LrShell             = import 'LrShell'			
			 if(WIN_ENV) then
				LrShell.openFilesInApp({"--LRSHUTDOWN"}, _PLUGIN.path..'/MIDI2LR.exe')
			else
				LrShell.openFilesInApp({"--LRSHUTDOWN"}, _PLUGIN.path..'/MIDI2LR.app') -- On Mac it seems like the files argument has to include an existing file
			end
			
			-- Do some work shutting down the plugin and then report progress
			for i=0,1,1 
			do 
			   progressFunction (i, "Thank you for using MIDI2LR")
			   sleep(1)
			end
			
			doneFunction ()
		
		end
		}
