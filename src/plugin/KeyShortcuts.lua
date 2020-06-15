--[[----------------------------------------------------------------------------

KeyShortcuts.lua

Returns MIDI2LR key code for a given shortcut. Uses locale-specific shortcuts.
Use the returned table, e.g., KeyCode.BrushDecreaseKey is "[" in English.
 
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
local LrLocalization = import 'LrLocalization'
--constants for translation work
local Modifiers = {
  Alt     = 0x1,
  Cmd     = 0x8,
  Control = 0x2,
  Option  = 0x1,
  Shift   = 0x4,
}
--keycodes using Cmd in LR use 0x8 in OSX and 0x2 in Windows for modifier

--note that functions using KeyCode must get it right--errors in spelling can cause crashes (e.g., BrushIncrease instead of BrushIncreaseKey)
local KeyCode = { 
  CycleAdjustmentBrushOverlayKey = "4 o",
  SliderDecreaseKey       = "0 cursor left",
  SliderIncreaseKey       = "0 cursor right",
  ShowAdjustmentBrushOverlayKey = "0 o",
}

if(WIN_ENV) then -- shortcuts that differ between Mac and PC
  KeyCode.CopyKey  = "6 c"
  KeyCode.PasteKey = "6 v"
  KeyCode.RedoKey  = "2 y"
  KeyCode.UndoKey  = "2 z"
else
  KeyCode.CopyKey  = "12 c"
  KeyCode.PasteKey = "12 v"
  KeyCode.RedoKey  = "12 z"
  KeyCode.UndoKey  = "8 z"
end

local langcode = LrLocalization.currentLanguage() 
if langcode == 'de' then
  KeyCode.BrushDecreaseKey   = '0 ,'
  KeyCode.FeatherDecreaseKey = '0 ;'
  KeyCode.BrushIncreaseKey   = '0 .'
  KeyCode.FeatherIncreaseKey = '0 :'
elseif langcode == 'es' or langcode == 'it' then
  KeyCode.BrushDecreaseKey   = '0 <'
  KeyCode.FeatherDecreaseKey = '0 >'
  KeyCode.BrushIncreaseKey   = '0 +'
  KeyCode.FeatherIncreaseKey = '0 *'
elseif langcode == 'fr' then
  KeyCode.BrushDecreaseKey   = '0 ,'
  KeyCode.FeatherDecreaseKey = '0 ?'
  KeyCode.BrushIncreaseKey   = '0 ;'
  KeyCode.FeatherIncreaseKey = '0 .'
elseif langcode == 'sv' then 
  KeyCode.BrushDecreaseKey   = '0 <'
  KeyCode.FeatherDecreaseKey = '0 >'
  KeyCode.BrushIncreaseKey   = '0 -'
  KeyCode.FeatherIncreaseKey = '0 _'
else
  KeyCode.BrushDecreaseKey   = '0 ['
  KeyCode.FeatherDecreaseKey = '0 {'
  KeyCode.BrushIncreaseKey   = '0 ]'
  KeyCode.FeatherIncreaseKey = '0 }'
end

return {
  KeyCode = KeyCode,
}



