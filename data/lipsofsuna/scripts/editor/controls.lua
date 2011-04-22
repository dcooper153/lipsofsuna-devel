Action{name = "erase", mode = "press", key1 = Keysym.k, func = function()
	if Editor.inst.prev_tiles[1] and Editor.inst.prev_tiles[2] then
		Editor.inst:fill(Editor.inst.prev_tiles[1], Editor.inst.prev_tiles[2], true)
	end
end}

Action{name = "move", mode = "analog", key1 = Keysym.w, key2 = Keysym.s, func = function(v)
	local vel = Editor.inst.camera:get_velocity()
	Editor.inst.camera:set_velocity(Vector(vel.x, vel.y, 10 * v))
end}

Action{name = "turn", mode = "analog", key1 = "mousex", func = function(v)
	Editor.inst.camera:rotate(-v * Editor.inst.mouse_sensitivity, 0)
end}

Action{name = "tilt", mode = "analog", key1 = "mousey", func = function(v)
	Editor.inst.camera:rotate(0, v * Editor.inst.mouse_sensitivity)
end}

Action{name = "menu", mode = "press", key1 = Keysym.ESCAPE, func = function()
	Client.moving = not Client.moving
end}

Action{name = "fill", mode = "press", key1 = Keysym.f, func = function()
	if Editor.inst.prev_tiles[1] and Editor.inst.prev_tiles[2] then
		Editor.inst:fill(Editor.inst.prev_tiles[1], Editor.inst.prev_tiles[2])
	end
end}

Action{name = "screenshot", mode = "press", key1 = Keysym.PRINT, func = function()
	Client:screenshot()
end}

Action{name = "strafe", mode = "analog", key1 = Keysym.a, key2 = Keysym.d, func = function(v)
	local vel = Editor.inst.camera:get_velocity()
	Editor.inst.camera:set_velocity(Vector(10 * v, vel.y, vel.z))
end}
