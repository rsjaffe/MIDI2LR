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


local Modifiers = {
  Alt     = 0x1,
  Cmd     = 0x2,
  Control = 0x2,
  Option  = 0x1,
  Shift   = 0x4,
}
Modifiers[LOC("$$$/Win/MenuDisplay/KeyboardShortcutElement/Shift=Shift")] = 0x4
Modifiers[LOC("$$$/AgBezels/KeyRemapping/MacCommand=Cmd")]                = 0x2
Modifiers[LOC("$$$/AgBezels/KeyRemapping/MacOption=Option")]              = 0x1
Modifiers[LOC("$$$/AgBezels/KeyRemapping/WinAlt=Alt")]                    = 0x1
Modifiers[LOC("$$$/AgBezels/KeyRemapping/WinControl=Control")]            = 0x2

local MIDI2LRkeyNames = {
  Backspace = 'backspace',
  Down      = 'cursor down',
  Left      = 'cursor left',
  Right     = 'cursor right',
  Space     = 'space',
  Tab       = 'tab',
  Up        = 'cursor up',
}
MIDI2LRkeyNames[LOC("$$$/Win/MenuDisplay/KeyboardShortcutElement/Backspace=Backspace")] = 'backspace'
MIDI2LRkeyNames[LOC("$$$/Win/MenuDisplay/KeyboardShortcutElement/Down=Down")]           = 'cursor down'
MIDI2LRkeyNames[LOC("$$$/Win/MenuDisplay/KeyboardShortcutElement/Left=Left")]           = 'cursor left'
MIDI2LRkeyNames[LOC("$$$/Win/MenuDisplay/KeyboardShortcutElement/Right=Right")]         = 'cursor right'
MIDI2LRkeyNames[LOC("$$$/Win/MenuDisplay/KeyboardShortcutElement/Space=Space")]         = 'space'
MIDI2LRkeyNames[LOC("$$$/Win/MenuDisplay/KeyboardShortcutElement/Tab=Tab")]             = 'tab'
MIDI2LRkeyNames[LOC("$$$/Win/MenuDisplay/KeyboardShortcutElement/Up=Up")]               = 'cursor up'


local function LRkeytoCode(arg1)
  local mystring = LOC(arg1)
  local KeyValue --default nil
  local ModifierValue = 0
  for i in mystring:gmatch("[^+ ]+") do
    if Modifiers[i] then
      ModifierValue = ModifierValue + Modifiers[i]
    else
      KeyValue = i
    end
  end
  if KeyValue == nil then
    KeyValue = '+' -- ignored in gmatch
  end
  if MIDI2LRkeyNames[KeyValue] then --KeyValue now guaranteed non-nil
    KeyValue = MIDI2LRkeyNames[KeyValue]
  end
  return ModifierValue..KeyValue
end


local KeyCode = { -- format from translation list is Cmd+Shift+Option+r , where 'r' is the key
  BrushDecreaseKey        = LRkeytoCode("$$$/AgDevelop/Localized/BrushDecreaseKey=["),
  BrushDecreaseKeyShifted = LRkeytoCode("$$$/AgDevelop/Localized/BrushDecreaseKeyShifted={"),
  BrushIncreaseKey        = LRkeytoCode("$$$/AgDevelop/Localized/BrushIncreaseKey=]"),
  BrushIncreaseKeyShifted = LRkeytoCode("$$$/AgDevelop/Localized/BrushIncreaseKeyShifted=}"),
  CopyKey                 = LRkeytoCode("$$$/AgLayout/Menu/Edit/Copy/Key=Cmd+c"),
  CycleAdjustmentBrushOverlayKey = LRkeytoCode("$$$/AgDevelop/Menu/View/CycleAdjustmentBrushOverlay/Key=Shift+o"),
  PasteKey                = LRkeytoCode("$$$/AgLayout/Menu/Edit/Paste/Key=Cmd+v"),
  SliderDecreaseKey       = "0cursor left",
  SliderIncreaseKey       = "0cursor right",
  ShowAdjustmentBrushOverlayKey = LRkeytoCode("$$$/AgDevelop/Menu/View/ShowHideAdjustmentBrushOverlay/Key=o"),
  UndoKey                 = LRkeytoCode("$$$/Application/Menu/Edit/Undo/Key=Cmd+z"),
}

if(WIN_ENV) then -- shortcuts that differ between Mac and PC
  KeyCode.RedoKey = LRkeytoCode("$$$/Application/Menu/Edit/RedoWin/Key=Cmd+y")
else
  KeyCode.RedoKey = LRkeytoCode("$$$/Application/Menu/Edit/Redo/Key=Cmd+Shift+z")
end

return {
  KeyCode = KeyCode,
}



