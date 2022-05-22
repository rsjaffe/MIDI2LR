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
local LrDialogs             = import 'LrDialogs'
local LrTasks               = import 'LrTasks'
local LrView                = import 'LrView'

local DialogStartedSuccessfully = false
local KeywordList
local ListReady = false
local NotifyFinished = false
local numseries = 64 -- number of keywords allowed

local function GetKeywordChildren(Keyword, Name)
  local KeywordChildren = Keyword:getChildren()
  for _,v in ipairs(KeywordChildren) do
    table.insert(KeywordList, {title=Name..'\226\134\146'..string.lower(v:getName()), value=v.localIdentifier } )
    GetKeywordChildren(v, Name..'\226\134\146'..string.lower(v:getName()))
    LrTasks.yield()
  end
end

local function GetKeywords()
  LrTasks.startAsyncTask(
    function()
          --[[-----------debug section, enable by adding - to beginning this line
    LrMobdebug.on()
    --]]-----------end debug section
      ListReady = false
      NotifyFinished = false
      KeywordList = {}
      table.insert(KeywordList, { title=' ', value=0 } )
      local LrCat = LrApplication.activeCatalog()
      local Keywords = LrCat:getKeywords()
      for _,v in ipairs(Keywords) do
        table.insert(KeywordList, { title=string.lower(v:getName()), value=v.localIdentifier } )
        GetKeywordChildren(v, string.lower(v:getName()))
        LrTasks.yield()
      end
      table.sort(KeywordList,function(k1, k2) return k1.title < k2.title end)
      ListReady = true
      if NotifyFinished then
        LrDialogs.showBezel(LOC("$$$/AgDialogs/Done=Done")..' '..
          LOC("$$$/AgLibrary/KeywordsPanel/KeywordCountTooltip=Keyword count: ^1",#KeywordList),3)
      end
    end
  )
end

local function ApplyKeyword(Keyword)
  LrTasks.startAsyncTask( function(context)
          --[[-----------debug section, enable by adding - to beginning this line
    LrMobdebug.on()
    --]]-----------end debug section
      local LrCat = LrApplication.activeCatalog()
      local LrKeyword = LrCat:getKeywordsByLocalId({Keyword})[1]
      local TargetPhoto  = LrCat:getTargetPhoto()
      if TargetPhoto and LrKeyword then
        local TargetPhotos = LrCat:getTargetPhotos()
        if ProgramPreferences.ClientShowBezelOnChange then
          LrDialogs.showBezel(LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SettingsString/ConstructionWithColon=^1: ^2",LOC("$$$/AgLibrary/AddKeyword=Add Keyword"),LrKeyword:getName()))
        end
        for _,v in ipairs(TargetPhotos) do
          LrCat:withWriteAccessDo( 'addKeyword',function( context ) v:addKeyword(LrKeyword) end, { timeout = 2 } )
        end
      end
    end
  )
end

local function ToggleKeyword(Keyword)
  LrTasks.startAsyncTask( function(context)
          --[[-----------debug section, enable by adding - to beginning this line
    LrMobdebug.on()
    --]]-----------end debug section
      local LrCat = LrApplication.activeCatalog()
      local LrKeyword = LrCat:getKeywordsByLocalId({Keyword})[1]
      local TargetPhoto  = LrCat:getTargetPhoto()
      if TargetPhoto and LrKeyword then
        local TargetPhotos = LrCat:getTargetPhotos()
        if ProgramPreferences.ClientShowBezelOnChange then
          LrDialogs.showBezel(LOC("$$$/AgCameraRawNamedSettings/CameraRawSettingMapping/SettingsString/ConstructionWithColon=^1: ^2",LOC("$$$/MIDI2LR/Keyword/Toggle=Toggle Keyword"),LrKeyword:getName()))
        end
        for _,v in ipairs(TargetPhotos) do
          local keyword_enabled = false
          for _,k in ipairs(v:getRawMetadata('keywords')) do
            if k.localIdentifier == Keyword then
              keyword_enabled = true
              break
            end
          end
          if keyword_enabled then
            LrCat:withWriteAccessDo( 'removeKeyword',function( context ) v:removeKeyword(LrKeyword) end, { timeout = 2 } )
          else
            LrCat:withWriteAccessDo( 'addKeyword',function( context ) v:addKeyword(LrKeyword) end, { timeout = 2 } )
          end
        end
      end
    end
  )
end

local function StartDialog(obstable,f)
  local group_rows, group_cols = 4,16-- row X col must equal numseries
  if ListReady then
    DialogStartedSuccessfully = true
    for i = 1,numseries do
      obstable['Keyword'..i] = {}
      obstable['Keyword'..i][1] = ProgramPreferences.Keywords[i]
    end
    local groupkeywords = {}
    -- set up each group of simple_list s
    for i = 1, group_cols do
      groupkeywords[i] = {}
      for j = 1, group_rows do
        local k = group_rows * (i - 1) + j
        groupkeywords[i][#groupkeywords[i]+1] = f:row{
          f:static_text{title = LOC("$$$/AgLibrary/Filter/BrowserCriteria/Keyword=Keyword").." "..k,
            width = LrView.share('KeywordTitle')},
          f:simple_list {items = KeywordList,
            allows_multiple_selection = false,
            width = 900,
            value = LrView.bind('Keyword'..k) }
        }
        LrTasks.yield()
      end
    end
    -- set up tabs
    local tabs = {}
    for i = 1,group_cols do
      local label = (i-1)*group_rows+1 ..'-'..i*group_rows --must have space after 1 before ..
      tabs[i] = f:tab_view_item {
        title = label,
        identifier = 'keywords-'..label,
        f:row{
          f:column(groupkeywords[i]),
        } -- row
      } -- tabviewitem
      LrTasks.yield()
    end
    return f:tab_view(tabs)
  else -- keyword enumeration not completed
    NotifyFinished = true
    return f:column(f:static_text{title = "MIDI2LR "..
        LOC("$$$/AgKeywords/ImportDialog/Title=Importing keywords")..'\n'..
        LOC("$$$/AgCatalogOptimize/ProgressDialog/Message=This process can take several minutes")..'\n'..
        LOC("$$$/AgLibrary/KeywordsPanel/KeywordCountTooltip=Keyword count: ^1",#KeywordList)})
  end
end

local function EndDialog(obstable, status)
  if DialogStartedSuccessfully then
    DialogStartedSuccessfully = false -- for next run
    if status == 'ok' then
      ProgramPreferences.Keywords = {} -- empty out prior settings
      for i = 1,numseries do
        if type(obstable['Keyword'..i])=='table' and obstable['Keyword'..i][1] ~= 0 then -- simple_list should return a table
          --obstable == 0 excluded as that is blank--trying to erase keyword
          ProgramPreferences.Keywords[i] = obstable['Keyword'..i][1]
        end
      end
    end
  end
end

if KeywordList == nil then --memoize KeywordList
  GetKeywords()
end

return {
  ApplyKeyword       = ApplyKeyword,
  EndDialog          = EndDialog,
  GetKeywords        = GetKeywords,
  StartDialog        = StartDialog,
  ToggleKeyword      = ToggleKeyword,
}
