local Building = require("building/building")
local BuildingCamera = require("building/camera")
local Client = require("core/client/client")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Client:register_init_hook(26, function()
	Client.camera_manager:register_camera("building", BuildingCamera())
end)

Client:register_start_hook(0, function()
	if Main.settings.building then
		Main.building = Building()
		Ui:set_state("building")
		return Hooks.STOP
	end
end)

Main.update_hooks:register(0, function(secs)
	if Main.building then
		Main.timing:start_action("building")
		Main.building:update()
	end
end)
