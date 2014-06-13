local Building = require("building/building")
local BuildingCamera = require("building/camera")
local BuildingUtils = require("building/building-utils")
local Client = require("core/client/client")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Main.game_modes:register("Building", function()
	Main.building = Building()
	Ui:set_state("building")
end)

Main.game_end_hooks:register(0, function(secs)
	if Main.building then
		Main.building:close()
		Main.building = nil
	end
end)

Main.main_start_hooks:register(0, function(secs)
	Main.building_utils = BuildingUtils()
	Main.objects.object_created_hooks:register(15, function(object)
		--FIXME
		if Main.building then
			object.has_server_data = function() return true end
		end
	end)
end)

Main.update_hooks:register(0, function(secs)
	if Main.building then
		Main.timing:start_action("building")
		Main.building:update(secs)
	end
end)

Main.main_start_hooks:register(130, function(secs)
	Client.camera_manager:register_camera("building", BuildingCamera())
end)
