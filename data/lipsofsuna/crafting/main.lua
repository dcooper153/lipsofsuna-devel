local Client = require("core/client/client")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Client:register_init_hook(26, function()
	--Client.crafting = Crafting()
end)

Client:register_reset_hook(10, function()
	--Client.crafting:reset()
end)

Client:register_start_hook(0, function()
	if Main.settings.crafting then
		--Client.crafting:init(true)
		Ui:set_state("crafting")
		return Hooks.STOP
	end
end)
