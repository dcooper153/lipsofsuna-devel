local Client = require("core/client/client")
local Chargen = require("character/chargen")

Client.chargen = Chargen()
Client.operators.chargen = Client.chargen

Client:register_reset_hook(10, function()
	Client.chargen:reset()
end)
