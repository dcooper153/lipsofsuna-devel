if not Settings then return end
if Settings.server then return end

-- Initialize the default video mode.
local Options = require(Mod.path .. "options")
local options = Options()
__initial_videomode = {options.window_width, options.window_height, options.fullscreen, options.vsync, options.multisamples}
__initial_pointer_grab = false

require "system/animation"
require "system/camera"
require "system/graphics"
require "system/heightmap-render"
require "system/input"
require "system/keysym"
require "system/model-editing"
require "system/model-merger"
require "system/object-render"
require "system/reload"
require "system/render"
require "system/render-model"
require "system/sound"
require "system/string"
require "system/tiles-render"
require "system/watchdog"
require "system/widgets"

Program:set_window_title("Lips of Suna")

require(Mod.path .. "bindings")
require(Mod.path .. "client")
