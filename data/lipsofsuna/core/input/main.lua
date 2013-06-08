local Client = require("core/client/client")
local Input = require("core/input/input")

Client:register_init_hook(0, function()
	Client.input = Input()
	Client.bindings = Client.input.bindings --FIXME
end)

Client:register_init_hook(1000, function()
	Client.input:load()
end)
