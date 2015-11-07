--[[----------------------------------------------------------------------------

Info.lua

MIDI2LR Plugin properties

------------------------------------------------------------------------------]]

return {
    LrSdkVersion = 6.0,
    LrToolkitIdentifier = 'com.rsjaffe.midi2lr',
    LrPluginName = 'MIDI2LR',
    LrInitPlugin = 'MIDI2LR_Client.lua', -- Main client logic
    LrForceInitPlugin = true,
    LrExportMenuItems = { -- Needs to provide at least a menu item
        {
            title = "About",
            file = "About.lua",
        },
        {
            title = "Start MIDI2LR",
            file = "LaunchAppFromMenu.lua"
        },
    },
    VERSION = { major=0, minor=5, revision=0}
}
