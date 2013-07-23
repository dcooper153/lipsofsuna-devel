local Client = require("core/client/client")
local Crafting = require("crafting/crafting")
local CraftingCamera = require("crafting/camera")
local CraftingUtils = require("crafting/crafting-utils")
local Game = require("core/server/game")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Main.game_modes:register("Crafting", function()
	-- Configure messaging.
	Main.messaging:set_transmit_mode(true, true)
	-- FIXME: Initialize the game.
	Main.game = Game("benchmark")
	Main.game.sectors.unload_time = nil
	-- Start the subsystems.
	Main.crafting = Crafting()
	Ui:set_state("crafting")
end)

Main.game_end_hooks:register(0, function(secs)
	if Main.crafting then
		Main.crafting:close()
		Main.crafting = nil
	end
end)

Main.main_start_hooks:register(0, function(secs)
	Main.crafting_utils = CraftingUtils()
	Main.objects.object_created_hooks:register(15, function(object)
		--FIXME
		if Main.crafting then
			object.has_server_data = function() return true end
		end
	end)
end)

Main.update_hooks:register(0, function(secs)
	if Main.crafting then
		Main.timing:start_action("crafting")
		Main.crafting:update(secs)
	end
end)

Client:register_init_hook(26, function()
	Client.camera_manager:register_camera("crafting", CraftingCamera())
end)
