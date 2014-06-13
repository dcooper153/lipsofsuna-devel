local CameraManager = require("core/camera/camera-manager")
local Client = require("core/client/client")

Main.main_start_hooks:register(120, function()
	Client.camera_manager = CameraManager()
end)
Main.main_start_hooks:register(150, function()
	Client.camera_manager:reset()
end)

Client:register_player_hook(25, function()
	Client.camera_manager:reset()
end)

Client:register_reset_hook(25, function()
	Client.camera_manager:reset()
end)

Client:register_update_hook(25, function(secs)
	Main.timing:start_action("camera")
	Client.camera_manager:update(secs)
end)
