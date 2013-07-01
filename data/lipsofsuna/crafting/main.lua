local Client = require("core/client/client")
local Crafting = require("crafting/crafting")
local CraftingCamera = require("crafting/camera")
local CraftingUtils = require("crafting/crafting-utils")
local DialogManager = require("core/dialog/dialog-manager")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Client:register_init_hook(26, function()
	Client.camera_manager:register_camera("crafting", CraftingCamera())
end)

Client:register_start_hook(0, function()
	if Main.settings.crafting then
		Main.crafting = Crafting()
		Main.dialogs = DialogManager()
		Ui:set_state("crafting")
		return Hooks.STOP
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
		Main.crafting:update()
	end
end)
