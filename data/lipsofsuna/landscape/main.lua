local Client = require("core/client/client")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Client:register_start_hook(0, function()
	if Main.settings.landscape then
		--Client.landscape = Landscape()
		Ui:set_state("landscape")
		return Hooks.STOP
	end
end)
