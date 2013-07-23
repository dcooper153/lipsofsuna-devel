local Building = require("building/building")
local BuildingCamera = require("building/camera")
local BuildingUtils = require("building/building-utils")
local Client = require("core/client/client")
local Game = require("core/server/game")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Main.game_modes:register("Building", function()
	-- Configure messaging.
	Main.messaging:set_transmit_mode(true, true)
	-- FIXME: Initialize the game.
	Main.game = Game("benchmark")
	Main.game.sectors.unload_time = nil
	-- Start the subsystems.
	Main.building = Building()
	Ui:set_state("building")
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

Main.main_start_hooks:register(1000, function()
	if Main.settings.building then
		Main:start_game("Building")
		return Hooks.STOP
	end
end)

Main.update_hooks:register(0, function(secs)
	if Main.building then
		Main.timing:start_action("building")
		Main.building:update(secs)
	end
end)

Client:register_init_hook(26, function()
	Client.camera_manager:register_camera("building", BuildingCamera())
end)
