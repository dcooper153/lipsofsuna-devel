if not Settings then return end
if Settings.server then return end

-- Initialize the default video mode.
local Options = require(Mod.path .. "options")
local options = Options()
__initial_videomode = {options.window_width, options.window_height, options.fullscreen, options.vsync, options.multisamples}
__initial_pointer_grab = false

require "system/graphics"
require "system/model-editing"
require "system/widgets"

Program:set_window_title("Lips of Suna")

require(Mod.path .. "bindings")
require(Mod.path .. "client")
