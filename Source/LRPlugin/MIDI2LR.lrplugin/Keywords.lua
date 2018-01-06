--[[------------------------------------------------------------------------------------

Keywords.lua

Keywords by Ernst Bokkelkamp

This file is part of MIDI2LR. Copyright 2015 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>. 

---------------------------------------------------------------------------------------]]



local LrApplication         = import 'LrApplication'
local LrTasks               = import 'LrTasks'
local LrDialogs             = import 'LrDialogs'
local LrView                = import 'LrView'

local KeywordCount = 0

local function GetKeywordChildren(KeywordList, Keyword, Name)
  local KeywordChildren = Keyword:getChildren()
  for _,v in ipairs(KeywordChildren) do
    KeywordCount = KeywordCount + 1
    LrDialogs.showBezel(LOC("$$$/AgLibrary/KeywordsPanel/KeywordCountTooltip=Keyword count: ^1",KeywordCount))
    table.insert(KeywordList, { title=Name..">" .. string.lower(v:getName()), value=v.localIdentifier } )
    GetKeywordChildren(KeywordList, v, Name .. ">" .. string.lower(v:getName()))
  end
end

local function GetKeywords()
  local KeywordList = {}
  table.insert(KeywordList, { title=' ', value=0 } )
  local LrCat = LrApplication.activeCatalog()
  local Keywords = LrCat:getKeywords()
  KeywordCount = 0
  for _,v in ipairs(Keywords) do
    KeywordCount = KeywordCount + 1
    LrDialogs.showBezel(LOC("$$$/AgLibrary/KeywordsPanel/KeywordCountTooltip=Keyword count: ^1",KeywordCount))
    table.insert(KeywordList, { title=string.lower(v:getName()), value=v.localIdentifier } )
    GetKeywordChildren(KeywordList, v, string.lower(v:getName()))
  end
  LrDialogs.showBezel(LOC("$$$/AgDevelop/Toolbar/SortTool=Sorting")..' '..tonumber(KeywordCount)..' '..LOC("$$$/AgLibrary/Filter/BrowserCriteria/Keywords=Keywords"),2)
  return KeywordList
end


local function ApplyKeyword(Keyword)
  LrTasks.startAsyncTask( function(context)
      local LrCat = LrApplication.activeCatalog()
      local TargetPhoto = LrCat:getTargetPhoto()
      if TargetPhoto then
        local LrKeyword = LrCat:getKeywordsByLocalId({Keyword})[1]
        if LrKeyword then
          local LrKeywordName = LrKeyword:getName()
          LrCat:withWriteAccessDo( 'addKeyword',function( context ) TargetPhoto:addKeyword(LrKeyword) end, { timeout = 2 } )
          LrDialogs.showBezel(LOC("$$$/AgLibrary/AddKeyword=Add Keyword") .. ': ' .. LrKeywordName)
        end 
      end
    end
  )
end

local numseries = 16 -- number of keywords allowed

local function StartDialog(obstable,f)
  local KeywordList = GetKeywords()

  for i = 1,numseries do
    obstable['Keyword'..i] = ProgramPreferences.Keywords[i]
  end

  local dlgrows = {}
  for i=1, numseries do
    dlgrows[i] = f:row{
      bind_to_object = obstable, -- default bound table
      f:static_text{title = LOC("$$$/AgLibrary/Filter/BrowserCriteria/Keyword=Keyword") .. " " .. i },
      -- f:static_text{title = string.format('%2d',i), width_in_char=2 },
      f:popup_menu{
        items = KeywordList,
        value = LrView.bind('Keyword'..i)
      }
    }
  end
  return f:column(dlgrows)
end


local function EndDialog(obstable, status)
  if status == 'ok' then
    ProgramPreferences.Keywords = {} -- empty out prior settings
    for i = 1,numseries do
      ProgramPreferences.Keywords[i] = obstable['Keyword'..i]
    end
  end
end

return {
  ApplyKeyword       = ApplyKeyword,
  EndDialog          = EndDialog,
  GetKeywords        = GetKeywords,
  StartDialog        = StartDialog,
}
