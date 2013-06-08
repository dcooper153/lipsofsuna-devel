local Benchmark = require("benchmark/benchmark")
local Client = require("core/client/client")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

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
		return Hooks.STOP
	end
end)
