Action{name = "attack", mode = "toggle", key1 = "mouse1", func = function(v)
	Network:send{packet = Packet(packets.SHOOT, "bool", v)}
end}

Action{name = "block", mode = "toggle", key1 = "mouse3", func = function(v)
	Network:send{packet = Packet(packets.PLAYER_BLOCK, "bool", v)}
end}

Action{name = "camera", mode = "press", key1 = Keysym.y, func = function()
	if Player.camera.mode == "first-person" then
		Player.camera.mode = "third-person"
		if Player.object then
			local e = Player.object.rotation.euler
			e[3] = 0
			Player.object.rotation = Quaternion{euler = e}
		end
	else
		Player.camera.mode = "first-person"
	end
	Player.camera:reset()
end}

Action{name = "chat", mode = "press", key1 = Keysym.t, func = function()
	Gui.chat_active = not Gui.chat_active
end}

Action{name = "feats", mode = "press", key1 = Keysym.f, func = function()
	Gui:set_mode("feats")
end}

Action{name = "inventory", mode = "press", key1 = Keysym.i, func = function()
	Gui:set_mode("inventory")
end}

Action{name = "jump", mode = "press", key1 = Keysym.c, func = function()
	Network:send{packet = Packet(packets.JUMP)}
end}

Action{name = "menu", mode = "press", key1 = Keysym.ESCAPE, func = function()
	if Gui.mode ~= "game" then
		Gui:set_mode("game")
	else
		Gui:set_mode("menu")
	end
end}

Action{name = "move", mode = "analog", key1 = Keysym.w, key2 = Keysym.s, func = function(v)
	v = math.max(-1, math.min(1, v))
	Network:send{packet = Packet(packets.PLAYER_MOVE, "int8", v * -127)}
end}

Action{name = "options", mode = "press", key1 = Keysym.o, func = function()
	Gui:set_mode("options")
end}

Action{name = "pick_up", mode = "press", key1 = Keysym.COMMA, func = function()
	Commands:pickup()
end}

Action{name = "quests", mode = "press", key1 = Keysym.q, func = function()
	Gui:set_mode("quests")
end}

Action{name = "skills", mode = "press", key1 = Keysym.k, func = function()
	Gui:set_mode("skills")
end}

Action{name = "quickslot_1", mode = "press", key1 = Keysym.F1, func = function()
	Quickslots:activate(1)
end}

Action{name = "quickslot_2", mode = "press", key1 = Keysym.F2, func = function()
	Quickslots:activate(2)
end}

Action{name = "quickslot_3", mode = "press", key1 = Keysym.F3, func = function()
	Quickslots:activate(3)
end}

Action{name = "quickslot_4", mode = "press", key1 = Keysym.F4, func = function()
	Quickslots:activate(4)
end}

Action{name = "quickslot_5", mode = "press", key1 = Keysym.F5, func = function()
	Quickslots:activate(5)
end}

Action{name = "quickslot_6", mode = "press", key1 = Keysym.F6, func = function()
	Quickslots:activate(6)
end}

Action{name = "quickslot_7", mode = "press", key1 = Keysym.F7, func = function()
	Quickslots:activate(7)
end}

Action{name = "quickslot_8", mode = "press", key1 = Keysym.F8, func = function()
	Quickslots:activate(8)
end}

Action{name = "quickslot_9", mode = "press", key1 = Keysym.F9, func = function()
	Quickslots:activate(9)
end}

Action{name = "quickslot_10", mode = "press", key1 = Keysym.F10, func = function()
	Quickslots:activate(10)
end}

Action{name = "quickslot_11", mode = "press", key1 = Keysym.F11, func = function()
	Quickslots:activate(11)
end}

Action{name = "quickslot_12", mode = "press", key1 = Keysym.F12, func = function()
	Quickslots:activate(12)
end}

Action{name = "run", mode = "toggle", key1 = Keysym.LSHIFT, func = function(v)
	Network:send{packet = Packet(packets.RUN, "bool", v)}
end}

Action{name = "screenshot", mode = "press", key1 = Keysym.PRINT, func = function()
	local n = Client:screenshot()
	Gui.chat_history:append{text = "Screenshot: " .. n}
end}

Action{name = "strafe", mode = "analog", key1 = Keysym.a, key2 = Keysym.d, func = function(v)
	Network:send{packet = Packet(packets.STRAFE, "int8", v * 127)}
end}

Action{name = "turn", mode = "analog", key1 = "mousex", func = function(v)
	local scale = 0.01 * Views.Options.inst.mouse_sensitivity
	if Action.dict_press[Keysym.LCTRL] then
		Player.camera_turn_state = Player.camera_turn_state + v * scale
	else
		Player.turn_state = Player.turn_state - v * scale
	end
end}

Action{name = "tilt", mode = "analog", key1 = "mousey", func = function(v)
	local scale = 0.01 * Views.Options.inst.mouse_sensitivity
	if Action.dict_press[Keysym.LCTRL] then
		Player.camera_tilt_state = Player.camera_tilt_state - v * scale
	else
		Player.tilt_state = Player.tilt_state + v * scale
	end
end}

Action{name = "use", mode = "press", key1 = Keysym.SPACE, func = function()
	Commands:use()
end}

Action{name = "zoom", mode = "analog", key1 = "mouse4", key2 = "mouse5", func = function(v)
	Player.camera:zoom{rate = 1 * v}
end}
