local Arena = require("arena/arena")
local ArenaCamera = require("arena/camera")
local Client = require("core/client/client")
local CombatUtils = require("arena/combat-utils")
local DialogManager = require("core/dialog/dialog-manager")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Main.game_modes:register("Arena", function()
	-- Configure messaging.
	Main.messaging:set_transmit_mode(true, true)
	-- FIXME: Initialize the game.
	Main.game = Game
	Main.game:init("benchmark")
	Main.game.sectors.unload_time = nil
	-- Start the subsystems.
	Main.arena = Arena()
	Main.dialogs = DialogManager()
	Ui:set_state("arena")
end)

Main.main_start_hooks:register(0, function(secs)
	Main.combat_utils = CombatUtils()
	Main.objects.object_created_hooks:register(15, function(object)
		--FIXME
		if Main.arena then
			object.has_server_data = function() return true end
		end
	end)
end)

Main.main_start_hooks:register(1000, function()
	if Main.settings.arena then
		Main:start_game("Arena")
		return Hooks.STOP
	end
end)

Main.update_hooks:register(0, function(secs)
	if Main.arena then
		Main.timing:start_action("arena")
		Main.arena:update(secs)
	end
end)

Client:register_init_hook(26, function()
	Client.camera_manager:register_camera("arena", ArenaCamera())
end)
