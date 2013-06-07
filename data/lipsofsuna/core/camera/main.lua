local CameraManager = require("core/camera/camera-manager")
local Client = require("core/client/client")

Client:register_init_hook(25, function()
	Client.camera_manager = CameraManager(Client.options)
end)

Client:register_player_hook(25, function()
	Client.camera_manager:reset()
end)

Client:register_reset_hook(25, function()
	Client.camera_manager:reset()
end)

Client:register_update_hook(25, function(secs)
	Client.camera_manager:update(secs)
end)
