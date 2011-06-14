Action{name = "erase", mode = "press", key1 = Keysym.k, func = function()
	if Editor.inst.prev_tiles[1] and Editor.inst.prev_tiles[2] then
		Editor.inst:fill(Editor.inst.prev_tiles[1], Editor.inst.prev_tiles[2], true)
	end
end}

Action{name = "extrude", mode = "analog", key1 = "mouse4", key2 = "mouse5", func = function(v)
	if Client.moving then
		Editor.inst:extrude(v < 0)
	end
end}

Action{name = "fill", mode = "press", key1 = Keysym.f, func = function()
	if Editor.inst.prev_tiles[1] and Editor.inst.prev_tiles[2] then
		Editor.inst:fill(Editor.inst.prev_tiles[1], Editor.inst.prev_tiles[2])
	end
end}

Action{name = "grab", mode = "press", key1 = Keysym.g, func = function(v)
	if Editor.inst.mode then
		Editor.inst.mode = nil
	else
		Editor.inst.mode = "grab"
	end
end}

Action{name = "menu", mode = "press", key1 = Keysym.ESCAPE, func = function()
	Client.moving = not Client.moving
end}

Action{name = "move", mode = "analog", key1 = Keysym.w, key2 = Keysym.s, func = function(v)
	local vel = Editor.inst.camera:get_velocity()
	Editor.inst.camera:set_velocity(Vector(vel.x, vel.y, 10 * v))
end}

Action{name = "snap", mode = "press", key1 = Keysym.x, func = function(v)
	for k,v in pairs(Editor.inst.selection) do
		if v.object then
			v.object:snap(0.25 * Voxel.tile_size)
		end
	end
end}

Action{name = "strafe", mode = "analog", key1 = Keysym.a, key2 = Keysym.d, func = function(v)
	local vel = Editor.inst.camera:get_velocity()
	Editor.inst.camera:set_velocity(Vector(10 * v, vel.y, vel.z))
end}

Action{name = "tilt", mode = "analog", key1 = "mousey", func = function(v)
	Editor.inst.camera:rotate(0, v * Editor.inst.mouse_sensitivity)
end}

Action{name = "rotate", mode = "press", key1 = Keysym.r, func = function(v)
	if Editor.inst.mode then
		Editor.inst.mode = nil
	else
		Editor.inst.mode = "rotate"
	end
end}

Action{name = "select", mode = "press", key1 = "mouse1", func = function(v)
	if Editor.inst.mode then
		Editor.inst.mode = nil
		return
	end
	local add = Action.dict_press[Keysym.LSHIFT] or Action.dict_press[Keysym.RSHIFT]
	Editor.inst:select(add)
end}

Action{name = "rectselect", mode = "toggle", key1 = Keysym.LCTRL, func = function(v)
	Editor.inst:set_rect_select(v)
end}

Action{name = "turn", mode = "analog", key1 = "mousex", func = function(v)
	if Editor.inst.mode ~= "rotate" then
		-- Rotate the camera.
		Editor.inst.camera:rotate(-v * Editor.inst.mouse_sensitivity, 0)
	else
		-- Rotate the selected objects.
		-- If left shift is pressed, rotation is 10x slower.
		local mult = Action.dict_press[Keysym.LSHIFT] and 0.1 or 1
		local drot = Quaternion{euler = {-v * mult * Editor.inst.mouse_sensitivity, 0, 0}}
		for k,v in pairs(Editor.inst.selection) do
			v:transform(nil, nil, drot)
		end
	end
end}

Action{name = "screenshot", mode = "press", key1 = Keysym.PRINT, func = function()
	Client:screenshot()
end}
