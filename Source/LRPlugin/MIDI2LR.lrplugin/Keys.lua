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
local completion = {  
  'backspace',
  'cursor down',
  'cursor left',
  'cursor right',
  'cursor up',
  'delete',
  'end',
  'escape',
  'f1',
  'f2',
  'f3',
  'f4',
  'f5',
  'f6',
  'f7',
  'f8',
  'f9',
  'f10',
  'f11',
  'f12',
  'f13',
  'f14',
  'f15',
  'f16',
  'fast forward',
  'home',
  'insert',
  'page down',
  'page up',
  'play',
  'return',
  'rewind',
  'spacebar',
  'stop',
  'tab',}
for k in pairs(legalanswers) do
  maxlength = math.max(k:len(),maxlength)
end

local control, alt
local shift = LOC("$$$/Win/MenuDisplay/KeyboardShortcutElement/Shift=Shift")
if(WIN_ENV) then
  control = LOC("$$$/AgBezels/KeyRemapping/WinControl=Control")
  alt = LOC("$$$/AgBezels/KeyRemapping/WinAlt=Alt")
else
  control = LOC("$$$/AgBezels/KeyRemapping/MacCommand=Command")
  alt = LOC("$$$/AgBezels/KeyRemapping/MacOption=Option")
end

local function validate(_,value)
  if value == nil then return true end
  value = LrStringUtils.trimWhitespace(value)
  local _, count = value:gsub( "[^\128-\193]", "") -- UTF-8 characters
  if count < 2 then return true,value end --one key or empty string
  value = LrStringUtils.lower(value)
  if legalanswers[value] then return true,value end
  return false, '', 'Value must be single character or spell out an F key (F1-F16) or: spacebar, return, escape, backspace, cursor left, cursor right, cursor up, cursor down, page up, page down, home, end, delete, insert, tab, play, stop, fast forward, rewind.'
end


--startdialog nowhere near ready--do not use yet!
local function StartDialog(obstable,f)
  local internalview1 = {}
  for i = 1,20 do
    obstable['Keyscontrol'..i] = ProgramPreferences.Keys[i]['control']
    obstable['Keysalt'..i] = ProgramPreferences.Keys[i]['alt']
    obstable['Keysshift'..i] = ProgramPreferences.Keys[i]['shift']
    obstable['Keyskey'..i] = ProgramPreferences.Keys[i]['key']
    table.insert(internalview1,f:row{
        f:static_text{title = 'Shortcut Key '..i,width = LrView.share('key_name')},
        f:checkbox{title = control, value = LrView.bind('Keyscontrol'..i)},
        f:checkbox{title = alt, value = LrView.bind('Keysalt'..i)},
        f:checkbox{title = shift, value = LrView.bind('Keysshift'..i)},
        f:combo_box{value = LrView.bind('Keyskey'..i), validate = validate, items = completion, width_in_chars = maxlength} } )
  end
  local internalview2 = {}
  for i = 21,40 do
    obstable['Keyscontrol'..i] = ProgramPreferences.Keys[i]['control']
    obstable['Keysalt'..i] = ProgramPreferences.Keys[i]['alt']
    obstable['Keysshift'..i] = ProgramPreferences.Keys[i]['shift']
    obstable['Keyskey'..i] = ProgramPreferences.Keys[i]['key']
    table.insert(internalview2,f:row{
        f:static_text{title = 'Shortcut Key '..i,width = LrView.share('key_name')},
        f:checkbox{title = control, value = LrView.bind('Keyscontrol'..i)},
        f:checkbox{title = alt, value = LrView.bind('Keysalt'..i)},
        f:checkbox{title = shift, value = LrView.bind('Keysshift'..i)},
        f:combo_box{value = LrView.bind('Keyskey'..i), validate = validate, items = completion, width_in_chars = maxlength} } )
  end
  return f:row{ f:column (internalview1), f:column (internalview2) }
end

local function EndDialog(obstable, status)
  if status == 'ok' then
    ProgramPreferences.Keys = {} -- empty out prior settings
    for i = 1,40 do
      ProgramPreferences.Keys[i] = {}
      if obstable['Keyskey'..i] ~= nil and obstable['Keyskey'..i] ~= '' and validate(obstable['Keyskey'..i]) == true then
        ProgramPreferences.Keys[i]['control'] = obstable['Keyscontrol'..i]
        ProgramPreferences.Keys[i]['alt'] = obstable['Keysalt'..i]
        ProgramPreferences.Keys[i]['shift'] = obstable['Keysshift'..i]
        ProgramPreferences.Keys[i]['key'] = obstable['Keyskey'..i]
      else
        ProgramPreferences.Keys[i]['control'] = false
        ProgramPreferences.Keys[i]['alt'] = false
        ProgramPreferences.Keys[i]['shift'] = false
        ProgramPreferences.Keys[i]['key'] = ''
      end
    end
  end
end

local function GetKey(i)
  if i < 1 or i > 40 then return nil end
  local retval = ''
  if(WIN_ENV) then
    if ProgramPreferences.Keys[i]['control'] then
      retval = retval .. 'control + '
    end
    if ProgramPreferences.Keys[i]['alt'] then
      retval = retval .. 'alt + '
    end
    if ProgramPreferences.Keys[i]['shift'] then
      retval = retval .. 'shift + '
    end
    retval = retval .. ProgramPreferences.Keys[i]['key']
    return retval
  else
    if ProgramPreferences.Keys[i]['control'] then
      retval = retval .. 'command + '
    end
    if ProgramPreferences.Keys[i]['alt'] then
      retval = retval .. 'option + '
    end
    if ProgramPreferences.Keys[i]['shift'] then
      retval = retval .. 'shift + '
    end
    retval = retval .. ProgramPreferences.Keys[i]['key']
    return retval
  end
end


return { --table of exports, setting table member name and module function it points to
  EndDialog   = EndDialog,
  StartDialog = StartDialog,
  GetKey = GetKey,
}