local Binding = require("core/client/binding")

Binding{name = "editor_copy", mode = "press", key1 = Keysym.x, func = function(v)
	if Ui.root ~= "editor" then return end
	Client.editor:copy()
end}

Binding{name = "editor_extrude", mode = "analog", key1 = "mousez", key2 = "", func = function(v)
	if Ui.root ~= "editor" then return end
	if v == 0 then return end
	Client.editor:extrude(v > 0)
end}

Binding{name = "editor_fill", mode = "press", key1 = Keysym.t, func = function()
	if Ui.root ~= "editor" then return end
	if not Client.editor.prev_tiles[1] then return end
	if not Client.editor.prev_tiles[2] then return end
	Client.editor:fill(Client.editor.prev_tiles[1], Client.editor.prev_tiles[2], true)
end}

Binding{name = "editor_grab", mode = "press", key1 = Keysym.g, func = function(v)
	if Ui.root ~= "editor" then return end
	if Client.editor.mode == "grab" then
		Client.editor.mode = nil
	else
		Client.editor.mode = "grab"
	end
end}

Binding{name = "editor_move", mode = "analog", key1 = Keysym.w, key2 = Keysym.s, func = function(v)
	if Ui.root ~= "editor" then return end
	local mult = Client.input:is_pressed(Keysym.LCTRL) and 1 or 10
	if Client.input:is_pressed(Keysym.LSHIFT) then
		Client.editor.camera.lifting = -mult * v
		Client.editor.camera.movement = nil
	else
		Client.editor.camera.movement = -mult * v
		Client.editor.camera.lifting = nil
	end
end}

Binding{name = "editor_paste", mode = "press", key1 = Keysym.v, func = function(v)
	if Ui.root ~= "editor" then return end
	Client.editor:paste()
end}

Binding{name = "editor_rotate", mode = "press", key1 = Keysym.h, func = function(v)
	if Ui.root ~= "editor" then return end
	if Client.editor.mode == "rotate" then
		Client.editor.mode = nil
	else
		Client.editor.mode = "rotate"
	end
end}

Binding{name = "editor_select", mode = "toggle", key1 = "mouse1", func = function(v)
	if Ui.root ~= "editor" then return end
	if v then
		-- Grabbed.
		Client.editor.mode = "grab"
		local add = Client.input:is_pressed(Keysym.LSHIFT)
		Client.editor:select(add)
	else
		-- Released.
		Client.editor.mode = nil
	end
end}

Binding{name = "editor_select_rect", mode = "toggle", key1 = Keysym.LCTRL, func = function(v)
	if Ui.root ~= "editor" then return end
	Client.editor:set_rect_select(v)
end}

Binding{name = "editor_snap", mode = "press", key1 = Keysym.PERIOD, func = function()
	if Ui.root ~= "editor" then return end
	local mult = Client.input:is_pressed(Keysym.LCTRL) and 0.25 or 0.5
	for k,v in pairs(Client.editor.selection) do
		if v.object then
			v.object:snap(mult * Voxel.tile_size, mult * math.pi)
			v:refresh()
		end
	end
end}

Binding{name = "editor_strafe", mode = "analog", key1 = Keysym.a, key2 = Keysym.d, func = function(v)
	if Ui.root ~= "editor" then return end
	local mult = Client.input:is_pressed(Keysym.LCTRL) and 1 or 10
	Client.editor.camera.strafing = mult * v
end}

Binding{name = "editor_tilt", mode = "analog", key1 = "mousey", func = function(v)
	if Ui.root ~= "editor" then return end
	local sens = 0.01 * Client.options.mouse_sensitivity
	if Client.options.invert_mouse then sens = -sens end
	if Client.editor.mode == "grab" then
		Client.editor:grab(Vector(0, v * Client.editor.mouse_sensitivity))
	else
		Client.editor.camera:rotate(0, v * sens)
	end
end}

Binding{name = "editor_turn", mode = "analog", key1 = "mousex", func = function(v)
	if Ui.root ~= "editor" then return end
	local sens = 0.01 * Client.options.mouse_sensitivity
	if Client.editor.mode == "grab" then
		-- Move the selection
		Client.editor:grab(Vector(-v * Client.editor.mouse_sensitivity), 0)
	elseif Client.editor.mode ~= "rotate" then
		-- Rotate the camera.
		Client.editor.camera:rotate(-v * sens, 0)
	else
		-- Rotate the selected objects.
		-- If left control is pressed, rotation is 10x slower.
		local mult = Client.input:is_pressed(Keysym.LCTRL) and 0.1 or 1
		local drot = Quaternion{euler = {-v * mult * sens, 0, 0}}
		for k,v in pairs(Client.editor.selection) do
			v:rotate(drot)
		end
	end
end}
