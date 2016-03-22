local LrStringUtils = import 'LrStringUtils'
local LrView = import 'LrView'

local legalanswers = {
  ['spacebar'] = true,
  ['return'] = true,
  ['escape'] = true,
  ['backspace'] = true,
  ['cursor left'] = true,
  ['cursor right'] = true,
  ['cursor up'] = true,
  ['cursor down'] = true,
  ['page up'] = true,
  ['page down'] = true,
  ['home'] = true,
  ['end'] = true,
  ['delete'] = true,
  ['insert'] = true,
  ['tab'] = true,
  ['play'] = true,
  ['stop'] = true,
  ['fast forward'] = true,
  ['rewind'] = true,
  f1 = true,
  f2 = true,
  f3 = true,
  f4 = true,
  f5 = true,
  f6 = true,
  f7 = true,
  f8 = true,
  f9 = true,
  f10 = true,
  f11 = true,
  f12 = true,
  f13 = true,
  f14 = true,
  f15 = true,
  f16 = true,
}
local maxlength = 0
local completion = {}
for k in pairs(legalanswers) do
  maxlength = math.max(k:len(),maxlength)
  table.insert(completion,k)
end


local function validate(_,value)
  value = LrStringUtils.trimWhitespace(value)
  local _, count = value:gsub( "[^\128-\193]", "") -- UTF-8 characters
  if count == 1 then return true,value end
  value = LrStringUtils.lower(value)
  if legalanswers[value] then return true,value end
  return false, '', 'Value must be single character or spell out an F key (F1-F16) or: spacebar, return, escape, backspace, cursor left, cursor right, cursor up, cursor down, page up, page down, home, end, delete, insert, tab, play, stop, fast forward, rewind.'
end
--startdialog nowhere near ready--do not use yet!
local function StartDialog(obstable,f)
  local row = { 
    f:checkbox{title = 'control', value = LrView.bind('control')},
    f:checkbox{title = 'alt', value = LrView.bind('alt')},
    f:checkbox{title = 'shift', value = LrView.bind('shift')},
    f:edit_field{value = LrView.bind('key'), validate = validate, immediate = false, auto_completion = true, completion = completion, width_in_chars = maxlength} }
  return row
end

local function EndDialog(obstable, status)
  if status == 'ok' then
  end
end

return { --table of exports, setting table member name and module function it points to
  EndDialog   = EndDialog,
  StartDialog = StartDialog,
}