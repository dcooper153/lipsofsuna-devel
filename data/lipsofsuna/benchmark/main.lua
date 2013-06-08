local Benchmark = require("benchmark/benchmark")
local BenchmarkCamera = require("benchmark/camera")
local Client = require("core/client/client")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Client:register_init_hook(26, function()
	Client.camera_manager:register_camera("benchmark", BenchmarkCamera())
end)

Client:register_start_hook(0, function()
	if Settings.benchmark then
		Ui:set_state("benchmark")
		Client.benchmark = Benchmark()
		return Hooks.STOP
	end
end)

Client:register_update_hook(11, function()
	if Client.benchmark then
		Client.benchmark:update(secs)
	end
end)
