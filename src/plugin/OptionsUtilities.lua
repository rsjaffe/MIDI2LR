--[[----------------------------------------------------------------------------

OptionsUtilities.lua

Utilities for options dialog.
 
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

local LrView              = import 'LrView'

local function slider(f,obstable,title,widthshare,binding,low,high,default)
  return
  f:row { 
    f:static_text {
      title = title,
      width = LrView.share(widthshare..'_label'),
    }, -- static_text
    f:slider {
      value = LrView.bind(binding),
      min = low, 
      max = high,
      width = LrView.share(widthshare..'_slider'),
      integral = high - 10 > low
    }, -- slider
    f:static_text {
      title = LrView.bind(binding),
      alignment = 'right',
      width_in_digits = 4,
      width = LrView.share(widthshare..'_reading'),  
    }, -- static_text
    f:push_button {
      title = LOC("$$$/AgLibrary/CameraRawView/PresetMenu/DefaultSettings=Default settings"),
      action = function ()
        obstable[binding] = default
      end,
    }, -- push_button
  }
end

return {
  slider = slider,
}
