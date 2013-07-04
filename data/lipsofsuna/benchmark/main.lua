local Benchmark = require("benchmark/benchmark")
local BenchmarkCamera = require("benchmark/camera")
local Client = require("core/client/client")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Main.game_modes:register("Benchmark", function()
	-- Configure messaging.
	Main.messaging:set_transmit_mode(true, true)
	-- FIXME: Initialize the game.
	Main.game = Game
	Main.game:init("benchmark")
	Main.game.sectors.unload_time = nil
	-- Start the subsystems.
	Client.benchmark = Benchmark()
	Ui:set_state("benchmark")
end)

Main.main_start_hooks:register(1000, function()
	if Settings.benchmark then
		Main:start_game("Benchmark")
		return Hooks.STOP
	end
end)

Client:register_init_hook(26, function()
	Client.camera_manager:register_camera("benchmark", BenchmarkCamera())
end)

Client:register_update_hook(11, function()
	if Client.benchmark then
		Client.benchmark:update(secs)
	end
end)
