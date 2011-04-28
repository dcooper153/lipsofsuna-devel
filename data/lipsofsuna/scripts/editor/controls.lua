Action{name = "erase", mode = "press", key1 = Keysym.k, func = function()
	if Editor.inst.prev_tiles[1] and Editor.inst.prev_tiles[2] then
		Editor.inst:fill(Editor.inst.prev_tiles[1], Editor.inst.prev_tiles[2], true)
	end
end}

Action{name = "fill", mode = "press", key1 = Keysym.f, func = function()
	if Editor.inst.prev_tiles[1] and Editor.inst.prev_tiles[2] then
		Editor.inst:fill(Editor.inst.prev_tiles[1], Editor.inst.prev_tiles[2])
	end
end}

Action{name = "menu", mode = "press", key1 = Keysym.ESCAPE, func = function()
	Client.moving = not Client.moving
end}

Action{name = "move", mode = "analog", key1 = Keysym.w, key2 = Keysym.s, func = function(v)
	local vel = Editor.inst.camera:get_velocity()
	Editor.inst.camera:set_velocity(Vector(vel.x, vel.y, 10 * v))
end}

Action{name = "rotate", mode = "analog", key1 = "mouse4", key2 = "mouse5", func = function(v)
	if Client.moving then
		Editor.inst:extrude(v > 0)
	else
		local point,object,tile = Target:pick_ray{camera = Editor.inst.camera}
		if not object then return end
		object:rotate(v, 4)
	end
end}

Action{name = "snap", mode = "press", key1 = Keysym.KP0, func = function(v)
	local point,object,tile = Target:pick_ray{camera = Editor.inst.camera}
	if not object then return end
	object:snap(0.25 * Voxel.tile_size)
end}

Action{name = "strafe", mode = "analog", key1 = Keysym.a, key2 = Keysym.d, func = function(v)
	local vel = Editor.inst.camera:get_velocity()
	Editor.inst.camera:set_velocity(Vector(10 * v, vel.y, vel.z))
end}

Action{name = "tilt", mode = "analog", key1 = "mousey", func = function(v)
	Editor.inst.camera:rotate(0, v * Editor.inst.mouse_sensitivity)
end}

Action{name = "translatex", mode = "analog", key1 = Keysym.KP4, key2 = Keysym.KP6, func = function(v)
	local point,object,tile = Target:pick_ray{camera = Editor.inst.camera}
	if not object then return end
	object:move(Vector(-v,0,0), 0.25 * Voxel.tile_size)
end}

Action{name = "translatey", mode = "analog", key1 = Keysym.KP1, key2 = Keysym.KP7, func = function(v)
	local point,object,tile = Target:pick_ray{camera = Editor.inst.camera}
	if not object then return end
	object:move(Vector(0,v,0), 0.25 * Voxel.tile_size)
end}

Action{name = "translatez", mode = "analog", key1 = Keysym.KP2, key2 = Keysym.KP8, func = function(v)
	local point,object,tile = Target:pick_ray{camera = Editor.inst.camera}
	if not object then return end
	object:move(Vector(0,0,v), 0.25 * Voxel.tile_size)
end}

Action{name = "select", mode = "press", key1 = "mouse1", func = function(v)
	local add = Action.dict_press[Keysym.LSHIFT] or Action.dict_press[Keysym.RSHIFT]
	Editor.inst:select(add)
end}

Action{name = "rectselect", mode = "toggle", key1 = Keysym.LCTRL, func = function(v)
	Editor.inst:set_rect_select(v)
end}

Action{name = "turn", mode = "analog", key1 = "mousex", func = function(v)
	Editor.inst.camera:rotate(-v * Editor.inst.mouse_sensitivity, 0)
end}

Action{name = "screenshot", mode = "press", key1 = Keysym.PRINT, func = function()
	Client:screenshot()
end}
