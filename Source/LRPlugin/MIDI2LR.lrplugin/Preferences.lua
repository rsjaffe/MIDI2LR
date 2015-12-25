--[[ put preferences module here

single table
subtable limits, which has a metatable that adds a parameter to the table when __index. one value in the parameter is {param = NameOfParameter} added by __index function
that parameter has a metatable that adds range-specific limit pair __index. __index uses self.param to identify parameter name

















--]]
LrDevelopController = import 'LrDevelopController'
local metalimit1 = {
  __index = function(table,key)
    table[key] = setmetatable({param = key,},metalimit2)
  end,
}
local metalimit2 = {
  __index = function(table,key)
    table[key] = {LrDevelopController.getLimits(table.param)}
  end,
}

local Preferences = {}

Preferences.Limits = setmetatable({},metalimit1)
