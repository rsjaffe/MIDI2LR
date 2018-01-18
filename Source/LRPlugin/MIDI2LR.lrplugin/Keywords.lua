--[[------------------------------------------------------------------------------------

Keywords.lua

Keywords by Ernst Bokkelkamp and Rory Jaffe

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

local KeywordList
local ListReady = false
local DialogStartedSuccessfully = false
local NotifyFinished = false

local function GetKeywordChildren(KeywordList, Keyword, Name)
  local KeywordChildren = Keyword:getChildren()
  for _,v in ipairs(KeywordChildren) do
    table.insert(KeywordList, { title=Name..'\226\134\146' .. string.lower(v:getName()), value=v.localIdentifier } )
    GetKeywordChildren(KeywordList, v, Name .. '\226\134\146' .. string.lower(v:getName()))
    LrTasks.yield()
  end
end

local function GetKeywords()
  LrTasks.startAsyncTask(
    function()
      ListReady = false
      NotifyFinished = false
      KeywordList = {}
      table.insert(KeywordList, { title=' ', value=0 } )
      local LrCat = LrApplication.activeCatalog()
      local Keywords = LrCat:getKeywords()
      for _,v in ipairs(Keywords) do
        table.insert(KeywordList, { title=string.lower(v:getName()), value=v.localIdentifier } )
        GetKeywordChildren(KeywordList, v, string.lower(v:getName()))
        LrTasks.yield()
      end
      ListReady = true
      if NotifyFinished then
        LrDialogs.showBezel(LOC("$$$/CRaw/QueueStatus/Finished=Finished").. ' ' ..
          LOC("$$$/AgLibrary/KeywordsPanel/KeywordCountTooltip=Keyword count: ^1",#KeywordList),3)
      end
    end
  )
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

if KeywordList == nil then --memoize this task
  GetKeywords()
end

local function StartDialog(obstable,f)
  if ListReady then --need to add dialog box for when list isn't ready yet
    DialogStartedSuccessfully = true
    LrDialogs.showBezel(LOC("$$$/AgDevelop/Toolbar/SortTool=Sorting")..' '.. #KeywordList ..' '..LOC("$$$/AgLibrary/Filter/BrowserCriteria/Keywords=Keywords"),2)
    for i = 1,numseries do
      obstable['Keyword'..i] = ProgramPreferences.Keywords[i]
    end
    local dlgrows = {}
    for i=1, numseries do
      dlgrows[i] = f:row{
        bind_to_object = obstable, -- default bound table
        f:static_text{title = LOC("$$$/AgLibrary/Filter/BrowserCriteria/Keyword=Keyword") .. " " .. i,
          width = LrView.share('KeywordTitle')},
        f:popup_menu{
          items = KeywordList,
          value = LrView.bind('Keyword'..i)
        }
      }
    end
    return f:column(dlgrows)
  else
    NotifyFinished = true
    return f:column(f:static_text{title = LOC("$$$/CRaw/QueueStatus/Processing=Processing") .. '\n' .. 
        LOC("$$$/AgLibrary/KeywordsPanel/KeywordCountTooltip=Keyword count: ^1",#KeywordList)})
  end
end


local function EndDialog(obstable, status)
  if DialogStartedSuccessfully then
    DialogStartedSuccessfully = false -- for next run
    if status == 'ok' then
      ProgramPreferences.Keywords = {} -- empty out prior settings
      for i = 1,numseries do
        ProgramPreferences.Keywords[i] = obstable['Keyword'..i]
      end
    end
  end
end

return {
  ApplyKeyword       = ApplyKeyword,
  EndDialog          = EndDialog,
  GetKeywords        = GetKeywords,
  StartDialog        = StartDialog,
}
