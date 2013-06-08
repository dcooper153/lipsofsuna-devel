local Client = require("core/client/client")
local Host = require("core/multiplayer/host")
local Join = require("core/multiplayer/join")

Client:register_init_hook(30, function()
	Client.host = Host()
	Client.join = Join()
end)
