--[[----------------------------------------------------------------------------

ShutDownPlugin.lua
Closes the app
 
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

-- check if MIDI2LR is set because if plugin fails to load in LR, reloading mechanism will fail because MIDI2LR will be unset
if MIDI2LR and MIDI2LR.RUNNING then
  MIDI2LR.RUNNING = false
  if MIDI2LR.SERVER then
    MIDI2LR.SERVER:send('TerminateApplication 1\n')
    MIDI2LR.SERVER:close()
  end
  if MIDI2LR.CLIENT then
    MIDI2LR.CLIENT:close()
  end
end
