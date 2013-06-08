require "system/bitwise"
require "system/config-file"
require "system/core"
require "system/coroutine"
require "system/database"
require "system/eventhandler"
require "system/image"
require "system/file"
require "system/network"
require "system/math"
require "system/noise"
require "system/object"
require "system/physics"
require "system/thread"
require "system/time"
require "system/timer"

-- Handle command line arguments.
require "common/settings"
if not Settings:parse_command_line() then
	print(Settings:usage())
	return
end

-- Load mods.
Mod = require("common/mod") --FIXME: global
Mod:load_list("mods.json")
Main()
