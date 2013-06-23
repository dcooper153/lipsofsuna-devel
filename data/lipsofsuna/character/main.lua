local Client = require("core/client/client")
local Chargen = require("character/chargen")
local Hooks = require("system/hooks")
local Ui = require("ui/ui")

Client:register_init_hook(26, function()
	Client.chargen = Chargen()
	Client.camera_manager:register_camera("chargen", Client.chargen.camera)
end)

Client:register_reset_hook(10, function()
	Client.chargen:reset()
end)

Client:register_start_hook(0, function()
	if Main.settings.character then
		Client.chargen:init(true)
		Ui:set_state("chargen")
		return Hooks.STOP
	end
end)
