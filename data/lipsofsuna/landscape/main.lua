local Client = require("core/client/client")
local Hooks = require("system/hooks")
local Keysym = require("system/keysym")
local Landscape = require("landscape/landscape")
local LandscapeCamera = require("landscape/camera")
local Ui = require("ui/ui")

Main.game_modes:register("Landscape", function()
	Main.landscape = Landscape()
	Main.game_create_hooks:call()
	Ui:set_state("landscape")
end)

Main.game_create_hooks:register(10, function(secs)
	if Main.landscape then
		local camera = Main.client.camera_manager:find_camera_by_type("landscape")
		if not camera then return end
		camera.position.y = 20 + Main.terrain_generator.world_planner:get_height(
			camera.position.x / Main.terrain.grid_size,
			camera.position.z / Main.terrain.grid_size)
	end
end)

Main.game_end_hooks:register(0, function(secs)
	if Main.landscape then
		Main.landscape:close()
		Main.landscape = nil
	end
end)

Main.update_hooks:register(0, function(secs)
	if Main.landscape then
		Main.timing:start_action("landscape")
		Main.landscape:update(secs)
	end
end)

Main.main_start_hooks:register(130, function(secs)
	Client.camera_manager:register_camera("landscape", LandscapeCamera())
end)

Main.main_start_hooks:register(510, function(secs)
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
