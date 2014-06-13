local Client = require("core/client/client")
local Crafting = require("crafting/crafting")
local CraftingCamera = require("crafting/camera")

Main.game_modes:register("Crafting", function()
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

Main.main_start_hooks:register(130, function()
	Client.camera_manager:register_camera("crafting", CraftingCamera())
end)
