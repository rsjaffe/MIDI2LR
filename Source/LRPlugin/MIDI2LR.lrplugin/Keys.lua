local LrStringUtils = import 'LrStringUtils'

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

function validate(view,value)
  value = LrStringUtils.trimWhitespace(value)
  local _, count = string.gsub(value, "[^\128-\193]", "") -- UTF-8 characters
  if count == 1 then return true,value end
  value = LrStringUtils.lower(value)
  if legalanswers[value] then return true,value end
  return false, '', 'Value must be single character or spell out an F key (F1-F16) or: spacebar, return, escape, backspace, cursor left, cursor right, cursor up, cursor down, page up, page down, home, end, delete, insert, tab, play, stop, fast forward, rewind.'
end