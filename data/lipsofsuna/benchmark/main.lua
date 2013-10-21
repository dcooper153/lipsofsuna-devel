local Benchmark = require("benchmark/benchmark")
local BenchmarkCamera = require("benchmark/camera")
local Client = require("core/client/client")
local Game = require("core/server/game")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Main.game_modes:register("Benchmark", function()
	-- Initialize the game.
	Main.messaging:set_transmit_mode(true, true)
	Main.game = Game("benchmark")
	Main.game:start()
	Main.game_create_hooks:call()
	Main.objects:set_unloading()
	-- Start the subsystems.
	Main.benchmark = Benchmark()
	Ui:set_state("benchmark")
end)

Main.game_end_hooks:register(0, function(secs)
	if Main.benchmark then
		Main.benchmark:close()
		Main.benchmark = nil
	end
end)

Client:register_init_hook(26, function()
	Client.camera_manager:register_camera("benchmark", BenchmarkCamera())
end)

Client:register_update_hook(11, function()
	if Main.benchmark then
		Main.benchmark:update(secs)
	end
end)
