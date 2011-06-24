Action{name = "erase", mode = "press", key1 = Keysym.k, func = function()
	if Editor.inst.prev_tiles[1] and Editor.inst.prev_tiles[2] then
		Editor.inst:fill(Editor.inst.prev_tiles[1], Editor.inst.prev_tiles[2], true)
	end
end}

Action{name = "extrude", mode = "analog", key1 = "mouse4", key2 = "mouse5", func = function(v)
	if Program.cursor_grabbed then
		Editor.inst:extrude(v < 0)
	end
end}

Action{name = "fill", mode = "press", key1 = Keysym.f, func = function()
	if Editor.inst.prev_tiles[1] and Editor.inst.prev_tiles[2] then
		Editor.inst:fill(Editor.inst.prev_tiles[1], Editor.inst.prev_tiles[2])
	end
end}

Action{name = "grab", mode = "press", key1 = Keysym.g, func = function(v)
	if Editor.inst.mode == "grab" then
		Editor.inst.mode = nil
	else
		Editor.inst.mode = "grab"
	end
end}

Action{name = "menu", mode = "press", key1 = Keysym.ESCAPE, func = function()
	Program.cursor_grabbed = not Program.cursor_grabbed
end}

Action{name = "move", mode = "analog", key1 = Keysym.w, key2 = Keysym.s, func = function(v)
	local mult = Action.dict_press[Keysym.LCTRL] and 1 or 10
	if Action.dict_press[Keysym.LSHIFT] then
		Editor.inst.camera.lifting = -mult * v
		Editor.inst.camera.movement = nil
	else
		Editor.inst.camera.movement = -mult * v
		Editor.inst.camera.lifting = nil
	end
end}

Action{name = "rotate", mode = "press", key1 = Keysym.r, func = function(v)
	if Editor.inst.mode == "rotate" then
		Editor.inst.mode = nil
	else
		Editor.inst.mode = "rotate"
	end
end}

Action{name = "snap", mode = "press", key1 = Keysym.TAB, func = function(v)
	local mult = Action.dict_press[Keysym.LCTRL] and 0.25 or 0.5
	for k,v in pairs(Editor.inst.selection) do
		if v.object then
			v.object:snap(mult * Voxel.tile_size, mult * math.pi)
			v:refresh()
		end
	end
end}

Action{name = "strafe", mode = "analog", key1 = Keysym.a, key2 = Keysym.d, func = function(v)
	local mult = Action.dict_press[Keysym.LCTRL] and 1 or 10
	Editor.inst.camera.strafing = mult * v
end}

Action{name = "tilt", mode = "analog", key1 = "mousey", func = function(v)
	Editor.inst.camera:rotate(0, v * Editor.inst.mouse_sensitivity)
end}

Action{name = "select", mode = "press", key1 = "mouse1", func = function(v)
	if Editor.inst.mode then
		Editor.inst.mode = nil
		return
	end
	local add = Action.dict_press[Keysym.LSHIFT]
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
		-- If left control is pressed, rotation is 10x slower.
		local mult = Action.dict_press[Keysym.LCTRL] and 0.1 or 1
		local drot = Quaternion{euler = {-v * mult * Editor.inst.mouse_sensitivity, 0, 0}}
		for k,v in pairs(Editor.inst.selection) do
			v:rotate(drot)
		end
	end
end}

Action{name = "screenshot", mode = "press", key1 = Keysym.PRINT, func = function()
	Program:capture_screen()
end}
