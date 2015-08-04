--[[----------------------------------------------------------------------------

Info.lua

MIDI2LR Plugin properties

------------------------------------------------------------------------------]]

return {
    LrSdkVersion = 3.0,
    LrToolkitIdentifier = 'ca.parth.midi2lr',
    LrPluginName = 'MIDI2LR',
    LrInitPlugin = 'MIDI2LR_Client.lua', -- Main client logic
    LrForceInitPlugin = true,
    LrExportMenuItems = { -- Needs to provide at least a menu item
        title = "About",
        file = "About.lua",
    },
    VERSION = { major=0, minor=1}
}