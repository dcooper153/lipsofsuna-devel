require "system/bitwise"
require "system/config-file"
require "system/core"
require "system/coroutine"
require "system/database"
require "system/eventhandler"
require "system/image"
require "system/file"
require "system/heightmap"
require "system/heightmap-physics"
require "system/network"
require "system/math"
require "system/noise"
require "system/object"
require "system/object-physics"
require "system/physics"
require "system/thread"
require "system/tiles"
require "system/tiles-physics"
require "system/timer"

-- Handle command line arguments.
require "common/settings"
if not Settings:parse_command_line() then
	print(Settings:usage())
	return
end

-- Load mods.
require "common/mod"
require "mods"

-- Initialize the client core.
if not Settings.server then
require "system/graphics"
require "system/input"
Program.window_title = "Lips of Suna"
require "system/camera"
require "system/heightmap-render"
require "system/model-editing"
require "system/object-render"
require "system/render"
require "system/tiles-render"
require "system/reload"
require "system/widgets"
end

Itemspec:validate()

if Settings.quit then Program.quit = true end
if Settings.server then
	require "server/main"
else
	require "client/main"
end
