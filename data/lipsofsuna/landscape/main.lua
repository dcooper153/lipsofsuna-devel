local Client = require("core/client/client")
local Hooks = require("system/hooks")
local Keysym = require("system/keysym")
local Landscape = require("landscape/landscape")
local LandscapeCamera = require("landscape/camera")
local Ui = require("ui/ui")

Client:register_init_hook(26, function()
	Client.camera_manager:register_camera("landscape", LandscapeCamera())
end)

Client:register_init_hook(510, function()
	Client.input:register_binding{name = "move", mode = "analog", key1 = Keysym.w, key2 = Keysym.s, func = function(v)
		if not Main.landscape then return end
		local camera = Main.client.camera_manager:find_camera_by_type("landscape")
		if not camera then return end
		camera:fly(v)
	end}
	Client.input:register_binding{name = "strafe", mode = "analog", key1 = Keysym.a, key2 = Keysym.d, func = function(v)
		if not Main.landscape then return end
		local camera = Main.client.camera_manager:find_camera_by_type("landscape")
		if not camera then return end
		camera:strafe(v)
	end}
	Client.input:register_binding{name = "tilt", mode = "analog", key1 = "mousey", func = function(v)
		if not Main.landscape then return end
		local sens = Client.options.mouse_sensitivity
		if Client.options.invert_mouse then sens = -sens end
		Client.camera_manager:tilt(v * sens)
	end}
	Client.input:register_binding{name = "turn", mode = "analog", key1 = "mousex", func = function(v)
		if not Main.landscape then return end
		local sens = Client.options.mouse_sensitivity
		Client.camera_manager:turn(v * sens)
	end}
end)

Client:register_start_hook(0, function()
	if Main.settings.landscape then
		Main.landscape = Landscape()
		Ui:set_state("landscape")
		return Hooks.STOP
	end
end)

Main.update_hooks:register(0, function(secs)
	if Main.landscape then
		Main.timing:start_action("landscape")
		Main.landscape:update()
	end
end)
