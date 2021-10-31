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

--[[----------------------------------------------------------------------------
--constants for translation work
local Modifiers = {
  Alt     = 0x1,
  Cmd     = 0x8,
  Control = 0x2,
  Option  = 0x1,
  Shift   = 0x4,
}
--keycodes using Cmd in LR use 0x8 in OSX and 0x2 in Windows for modifier
------------------------------------------------------------------------------]]

--functions using KeyCode must get it right
--errors in spelling can cause crashes (e.g., BrushIncrease instead of BrushIncreaseKey)
local KeyCode = {
  BrushDecreaseKey               = '0 '..LOC("$$$/AgDevelop/Localized/BrushDecreaseKey=["),
  BrushIncreaseKey               = '0 '..LOC("$$$/AgDevelop/Localized/BrushIncreaseKey=]"),
  CycleAdjustmentBrushOverlayKey = "4 o",
  FeatherDecreaseKey             = '0 '..LOC("$$$/AgDevelop/Localized/BrushDecreaseKeyShifted={"),
  FeatherIncreaseKey             = '0 '..LOC("$$$/AgDevelop/Localized/BrushIncreaseKeyShifted=}"),
  SliderDecreaseKey              = '0 '..LOC("$$$/AgDevelop/KBSC/decrementTargetAdjustmentValue=-"),
  SliderIncreaseKey              = '0 '..LOC("$$$/AgDevelop/KBSC/incrementTargetAdjustmentValue=="),
  ToggleFlagKey                  = '0 '..LOC("$$$/AgLibrary/Menu/ToggleFlag/Key=`"),
}

if WIN_ENV then -- shortcuts that differ between Mac and PC
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

return {
  KeyCode = KeyCode,
}



