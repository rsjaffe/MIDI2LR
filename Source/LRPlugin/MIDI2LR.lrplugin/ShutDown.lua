--[[----------------------------------------------------------------------------

ShutDown.lua
Closes the app
 
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


return {
  LrShutdownFunction = function(doneFunction, progressFunction)  
    local LrShell             = import 'LrShell'	
    local LrTasks             = import 'LrTasks'
    -- Report shutdown
    for i=0,1 do 
      progressFunction (i, LOC("$$$/AgPluginManager/Status/HttpServer/StopServer=Stopping Server"))
      if i == 0 then --main shut down steps
        -- signal main background loop
        math.randomseed(os.time())
        currentLoadVersion = rawget (_G, 'currentLoadVersion') or math.random()  
        currentLoadVersion = currentLoadVersion + 1  --signal halt to main background function
        --shut down app
        if(WIN_ENV) then
          LrShell.openFilesInApp({'--LRSHUTDOWN'}, _PLUGIN.path..'/MIDI2LR.exe')
        else
          LrShell.openFilesInApp({'--LRSHUTDOWN'}, _PLUGIN.path..'/MIDI2LR.app')
        end
      end
      LrTasks.sleep(1) 
    end    
    --tasks completed
    doneFunction()
  end
}
