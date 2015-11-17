local LrApplication = import 'LrApplication'
local LrDialogs = import 'LrDialogs'
local LrView = import 'LrView'
local psList = {}

for _,fold in pairs(LrApplication.developPresetFolders()) do
  local foldname = fold:getName()
  for _,pst in pairs(fold:getDevelopPresets()) do
    table.insert(psList,{title = foldname..'->'..pst:getName(), value = pst:getUuid()})
  end
end

local f = LrView.osFactory()
local font = "<system/small>"
LrDialogs.presentModalDialog {title = "Test", resizable = true,
  contents = f:column {
    f:static_text {title = "Hello world", font = font},
    f:simple_list {font = font,
      items = psList
    }
  }
}
