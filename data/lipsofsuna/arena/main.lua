local Client = require("core/client/client")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Client:register_start_hook(0, function()
	if Main.settings.arena then
		--Client.arena = Arena()
		Ui:set_state("arena")
		return Hooks.STOP
	end
end)
