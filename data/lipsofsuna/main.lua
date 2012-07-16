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
require "system/time"
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
require "system/model-merge"
require "system/object-render"
require "system/render"
require "system/render-model"
require "system/tiles-render"
require "system/reload"
require "system/string"
require "system/widgets"
end

Actorspec:validate_all()
Actorpresetspec:validate_all()
Animationspec:validate_all()
Dialogspec:validate_all()
Factionspec:validate_all()
Feattypespec:validate_all()
Feateffectspec:validate_all()
Iconspec:validate_all()
Itemspec:validate_all()
Obstaclespec:validate_all()
Patternspec:validate_all()
Questspec:validate_all()
Skillspec:validate_all()
Spellspec:validate_all()
Staticspec:validate_all()

if Settings.quit then Program.quit = true end
if Settings.server then
	require "server/main"
else
	require "client/main"
end
