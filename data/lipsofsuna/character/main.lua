local Client = require("core/client/client")
local Chargen = require("character/chargen")
local ChargenCamera = require("character/camera")

Client:register_init_hook(26, function()
	Client.chargen = Chargen()
	Client.camera_manager:register_camera("chargen", ChargenCamera())
end)

Client:register_reset_hook(10, function()
	Client.chargen:reset()
end)
