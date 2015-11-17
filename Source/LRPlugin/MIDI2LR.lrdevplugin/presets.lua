--[[----------------------------------------------------------------------------
presets.lua
Enumerates presets in Lightroom for MIDI2LR
Saves selected presets for mapping to MIDI2LR commands
------------------------------------------------------------------------------]]

local LrApplication = import 'LrApplication'
local LrBinding = import 'LrBinding'
local LrDialogs = import 'LrDialogs'
local LrFunctionContext = import 'LrFunctionContext'
local LrView = import 'LrView'
local psList = {}

local function assignPresets()
  LrFunctionContext.callWithContext( "assignPresets", function( context )

      for _,fold in pairs(LrApplication.developPresetFolders()) do
        local foldname = fold:getName()
        for _,pst in pairs(fold:getDevelopPresets()) do
          table.insert(psList,{title = foldname..'->'..pst:getName(), value = pst:getUuid()})
        end
      end

      local props = LrBinding.makePropertyTable( context )
--      props.myObservedString = "This is a string"

      local f = LrView.osFactory()
      local font = "<system/small>"
      LrDialogs.presentModalDialog {title = "Assign Presets", resizable = true,
        contents = f:column {
          bind_to_object = props,
          f:static_text {title = "Available Presets", font = font},
          f:simple_list {font = font, items = psList},
        }
      }
    end )
end

assignPresets() --execute
