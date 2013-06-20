local Client = require("core/client/client")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Client:register_start_hook(0, function()
	if Main.settings.building then
		--Client.building = Building()
		Ui:set_state("building")
		return Hooks.STOP
	end
end)
