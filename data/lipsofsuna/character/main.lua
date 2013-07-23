local Client = require("core/client/client")
local Chargen = require("character/chargen")
local Game = require("core/server/game")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Main.game_modes:register("Character", function()
	-- Configure messaging.
	Main.messaging:set_transmit_mode(true, true)
	-- FIXME: Initialize the game.
	Main.game = Game("benchmark")
	Main.game.sectors.unload_time = nil
	-- Start the subsystems.
	Client.chargen:init(true)
	Ui:set_state("chargen")
end)

Main.main_start_hooks:register(1000, function()
	if Main.settings.character then
		Main:start_game("Character")
		return Hooks.STOP
	end
end)

Client:register_init_hook(26, function()
	Client.chargen = Chargen()
	Client.camera_manager:register_camera("chargen", Client.chargen.camera)
end)

Client:register_reset_hook(10, function()
	Client.chargen:reset()
end)
