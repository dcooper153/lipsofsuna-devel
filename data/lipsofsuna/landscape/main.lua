local Client = require("core/client/client")
local Hooks = require("system/hooks")
local Landscape = require("landscape/landscape")
local LandscapeCamera = require("landscape/camera")
local Ui = require("ui/ui")

Client:register_init_hook(26, function()
	Client.camera_manager:register_camera("landscape", LandscapeCamera())
end)

Client:register_start_hook(0, function()
	if Main.settings.landscape then
		Main.landscape = Landscape()
		Ui:set_state("landscape")
		return Hooks.STOP
	end
end)

Main.update_hooks:register(0, function(secs)
	if Main.landscape then
		Main.timing:start_action("landscape")
		Main.landscape:update()
	end
end)
