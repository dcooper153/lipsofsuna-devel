Action{name = "attack", mode = "toggle", key1 = "mouse1", func = function(v)
	if Client.player_object then
		-- Game controls.
		Network:send{packet = Packet(packets.PLAYER_ATTACK, "bool", v)}
	elseif Ui.root == "editor" then
		-- Editor controls.
		if v then

			Client.editor.mode = "grab"

			local add = Action.dict_press[Keysym.LSHIFT]
			Client.editor:select(add)
		else
			-- Released.
			Client.editor.mode = nil

		end
	end
end}

Action{name = "block", mode = "toggle", key1 = "mouse3", func = function(v)
	if Client.player_object then
		Network:send{packet = Packet(packets.PLAYER_BLOCK, "bool", v)}
	end
end}

Action{name = "camera", mode = "press", key1 = Keysym.y, func = function()
	if Client.player_object then
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
	end
end}

Action{name = "chat", mode = "press", key1 = Keysym.t, func = function()
	if Client.player_object then
		Ui.state = "chat"
	end
end}

Action{name = "climb", mode = "press", key1 = Keysym.c, func = function()
	if Client.player_object then
		Network:send{packet = Packet(packets.PLAYER_CLIMB)}
	end
end}

Action{name = "editor_rotate", mode = "press", key1 = Keysym.v, func = function(v)
	if Ui.root == "editor" then
		-- Editor controls.
		if Client.editor.mode == "rotate" then
			Client.editor.mode = nil
		else
			Client.editor.mode = "rotate"
		end
	end
end}

Action{name = "editor_select_rect", mode = "toggle", key1 = Keysym.LCTRL, func = function(v)
	if Ui.root == "editor" then
		-- Editor controls.
		Client.editor:set_rect_select(v)
	end
end}

Action{name = "feats", mode = "press", key1 = Keysym.f, func = function()
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

Action{name = "grab", mode = "press", key1 = Keysym.g, func = function(v)
	if Client.player_object then
		-- Game controls.
		-- TODO
	elseif Ui.root == "editor" then
		-- Editor controls.
		if Client.editor.mode == "grab" then
			Client.editor.mode = nil
		else
			Client.editor.mode = "grab"
		end
	end
end}

Action{name = "copy", mode = "press", key1 = Keysym.x, func = function(v)
	if Client.player_object then
		-- Game controls.
		-- TODO
	elseif Ui.root == "editor" then
		-- Editor controls.
		Client.editor:copy()
	end
end}

Action{name = "paste", mode = "press", key1 = Keysym.v, func = function(v)
	if Client.player_object then
		-- Game controls.
		-- TODO
	elseif Ui.root == "editor" then
		-- Editor controls.
		Client.editor:paste()
	end
end}

Action{name = "inventory", mode = "press", key1 = Keysym.i, func = function()
	if Client.player_object then
		if Ui.state ~= "inventory" then
			Ui.state = "inventory"
		else
			Ui.state = "play"
		end
	end
end}

Action{name = "jump", mode = "press", key1 = Keysym.SPACE, func = function()
	if Client.player_object then
		Network:send{packet = Packet(packets.PLAYER_JUMP)}
	end
end}

Action{name = "map", mode = "press", key1 = Keysym.m, func = function()
	if Client.player_object then
		if Ui.state ~= "map" then
			Ui.state = "map"
		else
			Ui.state = "play"
		end
	end
end}

Action{name = "menu", mode = "press", key1 = Keysym.TAB, func = function()
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

Action{name = "menu up", mode = "press", key1 = Keysym.r, func = function()
	Ui:command("up")
end}

Action{name = "menu down", mode = "press", key1 = Keysym.f, func = function()
	Ui:command("down")
end}

Action{name = "menu back", mode = "press", key1 = Keysym.q, func = function()
	Ui:command("back")
end}

Action{name = "menu apply", mode = "press", key1 = Keysym.e, func = function()
	Ui:command("apply")
end}

Action{name = "mouse_grab", mode = "press", key1 = Keysym.ESCAPE, func = function()
	-- Toggle mouse grabbing.
	local grab = not Program.cursor_grabbed
	Client.options.grab_cursor = grab
	Client.options:save()
	Program.cursor_grabbed = grab
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

Action{name = "move", mode = "analog", key1 = Keysym.w, key2 = Keysym.s, func = function(v)
	if Client.player_object then
		-- Game controls.
		v = math.max(-1, math.min(1, v))
		Network:send{packet = Packet(packets.PLAYER_MOVE, "int8", v * -127)}
	elseif Ui.root == "editor" then
		-- Editor controls.
		local mult = Action.dict_press[Keysym.LCTRL] and 1 or 10
		if Action.dict_press[Keysym.LSHIFT] then
			Client.editor.camera.lifting = -mult * v
			Client.editor.camera.movement = nil
		else
			Client.editor.camera.movement = -mult * v
			Client.editor.camera.lifting = nil
		end
	end
end}

Action{name = "options", mode = "press", key1 = Keysym.o, func = function()
	if Client.player_object then
		if Ui.state ~= "options" then
			Ui.state = "options"
		else
			Ui.state = "play"
		end
	end
end}

Action{name = "pick_up", mode = "press", key1 = Keysym.COMMA, func = function()
	if Client.player_object then
		-- TODO
	end
end}

Action{name = "quests", mode = "press", key1 = Keysym.n, func = function()
	if Client.player_object then
		if Ui.state ~= "quests" then
			Ui.state = "quests"
		else
			Ui.state = "play"
		end
	end
end}

Action{name = "skills", mode = "press", key1 = Keysym.k, func = function()
	if Client.player_object then
		if Ui.state ~= "skills" then
			Ui.state = "skills"
		else
			Ui.state = "play"
		end
	elseif Ui.root == "editor" then
		-- Editor controls.
		if Client.editor.prev_tiles[1] and Client.editor.prev_tiles[2] then
			Client.editor:fill(Client.editor.prev_tiles[1], Client.editor.prev_tiles[2], true)
		end
	end
end}

Action{name = "quickslot_mode", mode = "press", key1 = Keysym.PERIOD, func = function()
	if Client.player_object then
		-- Game controls.
		if Quickslots.mode ~= "feats" then
			Quickslots.mode = "feats"
		else
			Quickslots.mode = "items"
		end
	elseif Ui.root == "editor" then
		-- Editor controls.
		local mult = Action.dict_press[Keysym.LCTRL] and 0.25 or 0.5
		for k,v in pairs(Client.editor.selection) do
			if v.object then
				v.object:snap(mult * Voxel.tile_size, mult * math.pi)
				v:refresh()
			end
		end
	end
end}

Action{name = "quickslot_1", mode = "press", key1 = Keysym.NUM1, func = function()
	if Client.player_object then
		Quickslots:activate(1)
	end
end}

Action{name = "quickslot_2", mode = "press", key1 = Keysym.NUM2, func = function()
	if Client.player_object then
		Quickslots:activate(2)
	end
end}

Action{name = "quickslot_3", mode = "press", key1 = Keysym.NUM3, func = function()
	if Client.player_object then
		Quickslots:activate(3)
	end
end}

Action{name = "quickslot_4", mode = "press", key1 = Keysym.NUM4, func = function()
	if Client.player_object then
		Quickslots:activate(4)
	end
end}

Action{name = "quickslot_5", mode = "press", key1 = Keysym.NUM5, func = function()
	if Client.player_object then
		Quickslots:activate(5)
	end
end}

Action{name = "quickslot_6", mode = "press", key1 = Keysym.NUM6, func = function()
	if Client.player_object then
		Quickslots:activate(6)
	end
end}

Action{name = "quickslot_7", mode = "press", key1 = Keysym.NUM7, func = function()
	if Client.player_object then
		Quickslots:activate(7)
	end
end}

Action{name = "quickslot_8", mode = "press", key1 = Keysym.NUM8, func = function()
	if Client.player_object then
		Quickslots:activate(8)
	end
end}

Action{name = "quickslot_9", mode = "press", key1 = Keysym.NUM9, func = function()
	if Client.player_object then
		Quickslots:activate(9)
	end
end}

Action{name = "quickslot_10", mode = "press", key1 = Keysym.NUM0, func = function()
	if Client.player_object then
		Quickslots:activate(10)
	end
end}

Action{name = "run", mode = "toggle", key1 = Keysym.LSHIFT, func = function(v)
	if Client.player_object then
		Network:send{packet = Packet(packets.PLAYER_RUN, "bool", not v)}
	end
end}

Action{name = "screenshot", mode = "press", key1 = Keysym.PRINT, func = function()
	local n = Program:capture_screen()
	Client:append_log("Screenshot: " .. n)
end}

Action{name = "strafe", mode = "analog", key1 = Keysym.a, key2 = Keysym.d, func = function(v)
	if Client.player_object then
		-- Game controls.
		Network:send{packet = Packet(packets.PLAYER_STRAFE, "int8", v * 127)}
	elseif Ui.root == "editor" then
		-- Editor controls.
		local mult = Action.dict_press[Keysym.LCTRL] and 1 or 10
		Client.editor.camera.strafing = mult * v
	end
end}

Action{name = "tilt", mode = "analog", key1 = "mousey", func = function(v)
	local sens = 0.01 * Client.options.mouse_sensitivity
	if Client.options.invert_mouse then sens = -sens end
	if Client.player_object then
		-- Game controls.
		if Action.dict_press[Keysym.LCTRL] then
			Client.camera.tilt_state = Client.camera.tilt_state - v * sens
		else
			Player.tilt_state = Player.tilt_state + v * sens
		end
	elseif Ui.root == "editor" then
		-- Editor controls.
		if Client.editor.mode == "grab" then
			Client.editor:grab(Vector(0, v * Client.editor.mouse_sensitivity))
		else
			Client.editor.camera:rotate(0, v * sens)
		end
	end
end}

Action{name = "turn", mode = "analog", key1 = "mousex", func = function(v)
	local sens = 0.01 * Client.options.mouse_sensitivity
	if Client.player_object then
		-- Game controls.
		if Action.dict_press[Keysym.LCTRL] then
			Client.camera.turn_state = Client.camera.turn_state + v * sens
		else
			Player.turn_state = Player.turn_state - v * sens
		end
	elseif Ui.root == "editor" then
		-- Editor controls.
		if Client.editor.mode == "grab" then
			-- Move the selection
			Client.editor:grab(Vector(-v * Client.editor.mouse_sensitivity), 0)
		elseif Client.editor.mode ~= "rotate" then
			-- Rotate the camera.
			Client.editor.camera:rotate(-v * sens, 0)
		else
			-- Rotate the selected objects.
			-- If left control is pressed, rotation is 10x slower.
			local mult = Action.dict_press[Keysym.LCTRL] and 0.1 or 1
			local drot = Quaternion{euler = {-v * mult * sens, 0, 0}}
			for k,v in pairs(Client.editor.selection) do
				v:rotate(drot)
			end
		end
	end
end}

Action{name = "use", mode = "press", key1 = Keysym.b, func = function()
	if Client.player_object then
		Ui.state = "world/object"
	end
end}

Action{name = "zoom", mode = "analog", key1 = "mousez", key2 = "", func = function(v)
	if Client.player_object then
		-- Game controls.
		Client.camera:zoom{rate = -v}
	elseif Ui.root == "editor" then
		-- Editor controls.
		if Program.cursor_grabbed and v ~= 0 then
			Client.editor:extrude(v > 0)
		end
	end
end}
