local Host = require("core/multiplayer/host")
local Join = require("core/multiplayer/join")

Main.main_start_hooks:register(5, function()
	Main.host = Host()
	Main.join = Join()
end)
