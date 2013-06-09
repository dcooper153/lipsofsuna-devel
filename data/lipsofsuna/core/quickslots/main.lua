local Client = require("core/client/client")
local Quickslots = require("core/quickslots/quickslots")

Client:register_init_hook(5, function()
	Quickslots:init()
end)

Client:register_play_hook(5, function()
	Quickslots:reset()
end)
