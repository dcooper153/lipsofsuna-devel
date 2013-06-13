local Client = require("core/client/client")
local Keysym = require("system/keysym")
local Ui = require("ui/ui")

Client:register_init_hook(500, function()

	Client.input:register_binding{name = "camera", mode = "press", key1 = Keysym.y, func = function()
		if not Client.player_object then return end
		if Client.camera_manager:get_camera_mode() == "first-person" then
			Client.camera_manager:set_camera_mode("third-person")
			if Client.player_object then
				local e = Client.player_object:get_rotation().euler
				e[3] = 0
				Client.player_object:set_rotation(Quaternion{euler = e})
			end
		else
			Client.camera_manager:set_camera_mode("first-person")
		end
	end}

	Client.input:register_binding{name = "rotate_camera", mode = "toggle", key1 = Keysym.LCTRL, func = function(v)
		if not Client.player_object then return end
		Client.camera_manager:set_rotation_mode(v)
	end}

	-- FIXME: Also has movement code.
	Client.input:register_binding{name = "tilt", mode = "analog", key1 = "mousey", func = function(v)
		if not Client.player_object then return end
		local sens = Client.options.mouse_sensitivity
		if Client.options.invert_mouse then sens = -sens end
		if Client.camera_manager:get_rotation_mode() then
			Client.camera_manager:tilt(v * sens)
		else
			Client.player_state:tilt(-v * sens)
		end
	end}

	-- FIXME: Also has movement code.
	Client.input:register_binding{name = "turn", mode = "analog", key1 = "mousex", func = function(v)
		if not Client.player_object then return end
		local sens = Client.options.mouse_sensitivity
		if Client.camera_manager:get_rotation_mode() then
			Client.camera_manager:turn(v * sens)
		else
			Client.player_state:turn(-v * sens)
		end
	end}

	Client.input:register_binding{name = "zoom", mode = "analog", key1 = "mousez", key2 = "", func = function(v)
		if not Client.player_object then return end
		if Ui:get_state() ~= "play" then return end
		Client.camera_manager:zoom(v)
	end}

end)