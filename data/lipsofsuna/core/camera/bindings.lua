local Binding = require("core/client/binding")
local Client = require("core/client/client")
local Keysym = require("system/keysym")
local Quickslots = require("core/quickslots/quickslots")

Binding{name = "camera", mode = "press", key1 = Keysym.y, func = function()
	if not Client.player_object then return end
	if Client:get_camera_mode() == "first-person" then
		Client:set_camera_mode("third-person")
		if Client.player_object then
			local e = Client.player_object:get_rotation().euler
			e[3] = 0
			Client.player_object:set_rotation(Quaternion{euler = e})
		end
	else
		Client:set_camera_mode("first-person")
	end
end}

Binding{name = "rotate_camera", mode = "toggle", key1 = Keysym.LCTRL, func = function(v)
	if not Client.player_object then return end
	Operators.camera:set_rotation_mode(v)
end}

-- FIXME: Also has movement code.
Binding{name = "tilt", mode = "analog", key1 = "mousey", func = function(v)
	if not Client.player_object then return end
	local sens = Client.options.mouse_sensitivity
	if Client.options.invert_mouse then sens = -sens end
	if Operators.camera:get_rotation_mode() then
		Client.camera.tilt_speed = Client.camera.tilt_speed + v * sens
	else
		Client.player_state:tilt(-v * sens)
	end
end}

-- FIXME: Also has movement code.
Binding{name = "turn", mode = "analog", key1 = "mousex", func = function(v)
	if not Client.player_object then return end
	local sens = Client.options.mouse_sensitivity
	if Operators.camera:get_rotation_mode() then
		Client.camera.turn_speed = Client.camera.turn_speed + v * sens
	else
		Client.player_state:turn(-v * sens)
	end
end}

Binding{name = "zoom", mode = "analog", key1 = "mousez", key2 = "", func = function(v)
	if not Client.player_object then return end
	if Ui:get_state() ~= "play" then return end
	Client.camera:zoom{rate = -v}
end}
