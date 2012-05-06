Binding{name = "attack", mode = "toggle", key1 = "mouse1", func = function(v)
	if not Client.player_object then return end
	Network:send{packet = Packet(packets.PLAYER_ATTACK, "bool", v)}
end}

Binding{name = "block", mode = "toggle", key1 = "mouse3", func = function(v)
	if Client.player_object then
		Network:send{packet = Packet(packets.PLAYER_BLOCK, "bool", v)}
	end
end}

Binding{name = "camera", mode = "press", key1 = Keysym.y, func = function()
	if not Client.player_object then return end
	if Client.camera_mode == "first-person" then
		Client.camera_mode = "third-person"
		if Client.player_object then
			local e = Client.player_object.rotation.euler
			e[3] = 0
			Client.player_object.rotation = Quaternion{euler = e}
		end
	else
		Client.camera_mode = "first-person"
	end
end}

Binding{name = "chat", mode = "press", key1 = Keysym.t, func = function()
	if not Client.player_object then return end
	Ui.state = "chat"
end}

Binding{name = "climb", mode = "press", key1 = Keysym.c, func = function()
	if not Client.player_object then return end
	Network:send{packet = Packet(packets.PLAYER_CLIMB)}
end}

Binding{name = "feats", mode = "press", key1 = Keysym.u, func = function()
	if Client.player_object then
		if Ui.state ~= "feats" then
			Ui.state = "feats"
		else
			Ui.state = "play"
		end
	elseif Ui.root == "editor" then
		-- Editor controls.
		if Client.editor.prev_tiles[1] and Client.editor.prev_tiles[2] then
			Client.editor:fill(Client.editor.prev_tiles[1], Client.editor.prev_tiles[2])
		end
	end
end}

Binding{name = "inventory", mode = "press", key1 = Keysym.i, func = function()
	if not Client.player_object then return end
	if Ui.state ~= "inventory" then
		Ui.state = "inventory"
	else
		Ui.state = "play"
	end
end}

Binding{name = "jump", mode = "toggle", key1 = Keysym.SPACE, func = function(v)
	if not Client.player_object then return end
	Network:send{packet = Packet(packets.PLAYER_JUMP, "bool", v)}
end}

Binding{name = "map", mode = "press", key1 = Keysym.m, func = function()
	if not Client.player_object then return end
	if Ui.state ~= "map" then
		Ui.state = "map"
	else
		Ui.state = "play"
	end
end}

Binding{name = "menu", mode = "press", key1 = Keysym.TAB, func = function()
	if Ui.root == "play" then
		-- Game controls.
		if Ui.state ~= "play" then
			Ui.state = "play"
		else
			Ui.state = "menu"
		end
	elseif Ui.root == "editor" then
		-- Editor controls.
		if Ui.state ~= "editor" then
			Ui.state = "editor"
		else
			Ui.state = "editor/menu"
		end
	elseif Ui.root == "chargen" then
		Ui.state = "chargen"
	else
		-- Main menu controls.
		Ui.state = "mainmenu"
	end
end}

Binding{name = "menu up", mode = "toggle", key1 = Keysym.r, func = function(v)
	Ui:command("up", v)
end}

Binding{name = "menu down", mode = "toggle", key1 = Keysym.f, func = function(v)
	Ui:command("down", v)
end}

Binding{name = "menu back", mode = "toggle", key1 = Keysym.q, func = function(v)
	Ui:command("back", v)
end}

Binding{name = "menu apply", mode = "toggle", key1 = Keysym.e, func = function(v)
	Ui:command("apply", v)
end}

Binding{name = "mouse_grab", mode = "press", key1 = Keysym.ESCAPE, func = function()
	-- Toggle mouse grabbing.
	local grab = not Ui.pointer_grab
	Client.options.grab_cursor = grab
	Client.options:save()
	Ui.pointer_grab = grab
	-- Open the in-game menu at ungrab.
	if not grab then
		if Ui.state == "play" then
			Ui.state = "menu"
		elseif Ui.state == "editor" then
			Ui.state = "editor/menu"
		end
	-- Close the in-game menu at grab.
	elseif Ui.root == "play" then
		Ui.state = "play"
	end
end}

Binding{name = "move", mode = "analog", key1 = Keysym.w, key2 = Keysym.s, func = function(v)
	if not Client.player_object then return end
	v = math.max(-1, math.min(1, v))
	Network:send{packet = Packet(packets.PLAYER_MOVE, "int8", v * -127)}
end}

Binding{name = "options", mode = "press", key1 = Keysym.o, func = function()
	if not Client.player_object then return end
	if Ui.state ~= "options" then
		Ui.state = "options"
	else
		Ui.state = "play"
	end
end}

Binding{name = "pick_up", mode = "press", key1 = Keysym.COMMA, func = function()
	if not Client.player_object then return end
	-- TODO
end}

Binding{name = "quests", mode = "press", key1 = Keysym.n, func = function()
	if not Client.player_object then return end
	if Ui.state ~= "quests" then
		Ui.state = "quests"
	else
		Ui.state = "play"
	end
end}

Binding{name = "skills", mode = "press", key1 = Keysym.k, func = function()
	if not Client.player_object then return end
	if Ui.state ~= "skills" then
		Ui.state = "skills"
	else
		Ui.state = "play"
	end
end}

Binding{name = "quickslot_mode", mode = "press", key1 = Keysym.PERIOD, func = function()
	if not Client.player_object then return end
	if Quickslots.mode ~= "feats" then
		Quickslots.mode = "feats"
	else
		Quickslots.mode = "items"
	end
end}

Binding{name = "quickslot_1", mode = "press", key1 = Keysym.NUM1, func = function()
	if not Client.player_object then return end
	Quickslots:activate(1)
end}

Binding{name = "quickslot_2", mode = "press", key1 = Keysym.NUM2, func = function()
	if Client.player_object then
		Quickslots:activate(2)
	end
end}

Binding{name = "quickslot_3", mode = "press", key1 = Keysym.NUM3, func = function()
	if not Client.player_object then return end
	Quickslots:activate(3)
end}

Binding{name = "quickslot_4", mode = "press", key1 = Keysym.NUM4, func = function()
	if not Client.player_object then return end
	Quickslots:activate(4)
end}

Binding{name = "quickslot_5", mode = "press", key1 = Keysym.NUM5, func = function()
	if not Client.player_object then return end
	Quickslots:activate(5)
end}

Binding{name = "quickslot_6", mode = "press", key1 = Keysym.NUM6, func = function()
	if not Client.player_object then return end
	Quickslots:activate(6)
end}

Binding{name = "quickslot_7", mode = "press", key1 = Keysym.NUM7, func = function()
	if not Client.player_object then return end
	Quickslots:activate(7)
end}

Binding{name = "quickslot_8", mode = "press", key1 = Keysym.NUM8, func = function()
	if not Client.player_object then return end
	Quickslots:activate(8)
end}

Binding{name = "quickslot_9", mode = "press", key1 = Keysym.NUM9, func = function()
	if not Client.player_object then return end
	Quickslots:activate(9)
end}

Binding{name = "quickslot_10", mode = "press", key1 = Keysym.NUM0, func = function()
	if not Client.player_object then return end
	Quickslots:activate(10)
end}

Binding{name = "rotate_camera", mode = "toggle", key1 = Keysym.LCTRL, func = function(v)
	Operators.camera:set_rotation_mode(v)
end}

Binding{name = "run", mode = "toggle", key1 = Keysym.LSHIFT, func = function(v)
	if not Client.player_object then return end
	Network:send{packet = Packet(packets.PLAYER_RUN, "bool", not v)}
end}

Binding{name = "screenshot", mode = "press", key1 = Keysym.PRINT, func = function()
	local n = Program:capture_screen()
	Client:append_log("Screenshot: " .. n)
end}

Binding{name = "strafe", mode = "analog", key1 = Keysym.a, key2 = Keysym.d, func = function(v)
	if not Client.player_object then return end
	Network:send{packet = Packet(packets.PLAYER_STRAFE, "int8", v * 127)}
end}

Binding{name = "tilt", mode = "analog", key1 = "mousey", func = function(v)
	if not Client.player_object then return end
	local sens = 0.01 * Client.options.mouse_sensitivity
	if Client.options.invert_mouse then sens = -sens end
	if Operators.camera:get_rotation_mode() then
		Client.camera.tilt_state = Client.camera.tilt_state - v * sens
	else
		Player.tilt_state = Player.tilt_state + v * sens
	end
end}

Binding{name = "turn", mode = "analog", key1 = "mousex", func = function(v)
	if not Client.player_object then return end
	local sens = 0.01 * Client.options.mouse_sensitivity
	if Operators.camera:get_rotation_mode() then
		Client.camera.turn_state = Client.camera.turn_state + v * sens
	else
		Player.turn_state = Player.turn_state - v * sens
	end
end}

Binding{name = "use", mode = "press", key1 = Keysym.b, func = function()
	if not Client.player_object then return end
	Ui.state = "world/object"
end}

Binding{name = "zoom", mode = "analog", key1 = "mousez", key2 = "", func = function(v)
	if not Client.player_object then return end
	Client.camera:zoom{rate = -v}
end}
