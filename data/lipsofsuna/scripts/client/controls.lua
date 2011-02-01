Keys = {ESCAPE = 27, SPACE = 32, COMMA = 44,
	a = 97, c = 99, d = 100, f = 102, i = 105, k = 107, o = 111, q = 113, y = 121,
	x = 120, w = 119, s = 115, F1 = 282, F2 = 283, F3 = 284, F4 = 285, F5 = 286,
	F6 = 287, F7 = 288, F8 = 289, F9 = 290, F10 = 291, F11 = 292, F12 = 293,
	LSHIFT = 304, RCTRL = 305, LCTRL = 306, PRINT = 316}
Mods = {LSHIFT = 0x0001, RSHIFT= 0x0002, LCTRL = 0x0040, RCTRL = 0x0080,
	LALT = 0x0100, RALT = 0x0200, LMETA = 0x0400, RMETA = 0x0800,
	NUM = 0x1000, CAPS = 0x2000, MODE = 0x4000} 

Action{name = "Attack", mode = "press", key1 = "mouse1", func = function()
	Network:send{packet = Packet(packets.SHOOT)}
end}

Action{name = "Camera", mode = "press", key1 = Keys.y, func = function()
	if Player.camera.mode == "first-person" then
		Player.camera.mode = "third-person"
		if Player.object then
			local e = Player.object.rotation.euler
			e[3] = 0
			Player.object.rotation = Quaternion:new_euler(e)
		end
	else
		Player.camera.mode = "first-person"
	end
	Player.camera:reset()
end}

Action{name = "Feats", mode = "press", key1 = Keys.f, func = function()
	Gui:set_mode("feats")
end}

Action{name = "Inventory", mode = "press", key1 = Keys.i, func = function()
	Gui:set_mode("inventory")
end}

Action{name = "Jump", mode = "press", key1 = Keys.c, func = function()
	Network:send{packet = Packet(packets.JUMP)}
end}

Action{name = "Menu", mode = "press", key1 = Keys.ESCAPE, func = function()
	if Gui.mode ~= "game" then
		Gui:set_mode("game")
	else
		Gui:set_mode("menu")
	end
end}

Action{name = "Move", mode = "analog", key1 = Keys.w, key2 = Keys.s, func = function(v)
	v = math.max(-1, math.min(1, v))
	Network:send{packet = Packet(packets.PLAYER_MOVE, "int8", v * -127)}
end}

Action{name = "Options", mode = "press", key1 = Keys.o, func = function()
	Gui:set_mode("options")
end}

Action{name = "Pick up", mode = "press", key1 = Keys.COMMA, func = function()
	Commands:pickup()
end}

Action{name = "Quests", mode = "press", key1 = Keys.q, func = function()
	Gui:set_mode("quests")
end}

Action{name = "Skills", mode = "press", key1 = Keys.k, func = function()
	Gui:set_mode("skills")
end}

Action{name = "Quickslot 1", mode = "press", key1 = Keys.F1, func = function()
	Quickslots:activate(1)
end}

Action{name = "Quickslot 2", mode = "press", key1 = Keys.F2, func = function()
	Quickslots:activate(2)
end}

Action{name = "Quickslot 3", mode = "press", key1 = Keys.F3, func = function()
	Quickslots:activate(3)
end}

Action{name = "Quickslot 4", mode = "press", key1 = Keys.F4, func = function()
	Quickslots:activate(4)
end}

Action{name = "Quickslot 5", mode = "press", key1 = Keys.F5, func = function()
	Quickslots:activate(5)
end}

Action{name = "Quickslot 6", mode = "press", key1 = Keys.F6, func = function()
	Quickslots:activate(6)
end}

Action{name = "Quickslot 7", mode = "press", key1 = Keys.F7, func = function()
	Quickslots:activate(7)
end}

Action{name = "Quickslot 8", mode = "press", key1 = Keys.F8, func = function()
	Quickslots:activate(8)
end}

Action{name = "Quickslot 9", mode = "press", key1 = Keys.F9, func = function()
	Quickslots:activate(9)
end}

Action{name = "Quickslot 10", mode = "press", key1 = Keys.F10, func = function()
	Quickslots:activate(10)
end}

Action{name = "Quickslot 11", mode = "press", key1 = Keys.F11, func = function()
	Quickslots:activate(11)
end}

Action{name = "Quickslot 12", mode = "press", key1 = Keys.F12, func = function()
	Quickslots:activate(12)
end}

Action{name = "Run", mode = "toggle", key1 = Keys.LSHIFT, func = function(v)
	Network:send{packet = Packet(packets.RUN, "bool", v)}
end}

Action{name = "Screenshot", mode = "press", key1 = Keys.PRINT, func = function()
	local n = Client:screenshot()
	Gui.chat_history:append{text = "Screenshot: " .. n}
end}

Action{name = "Strafe", mode = "analog", key1 = Keys.a, key2 = Keys.d, func = function(v)
	Network:send{packet = Packet(packets.STRAFE, "int8", v * 127)}
end}

Action{name = "Turn", mode = "analog", key1 = "mousex", func = function(v)
	if Action.dict_press[Keys.LCTRL] then
		Player.camera_turn = v * Views.Options.inst.mouse_sensitivity
	else
		Player.turn = -v * Views.Options.inst.mouse_sensitivity
	end
end}

Action{name = "Tilt", mode = "analog", key1 = "mousey", func = function(v)
	if Action.dict_press[Keys.LCTRL] then
		Player.camera_tilt = v * Views.Options.inst.mouse_sensitivity
	else
		Player.tilt = v * Views.Options.inst.mouse_sensitivity
	end
end}

Action{name = "Use", mode = "press", key1 = Keys.SPACE, func = function()
	Commands:use()
end}

Action{name = "Zoom", mode = "analog", key1 = "mouse4", key2 = "mouse5", func = function(v)
	Player.camera:zoom{rate = 2 * v}
end}

------------------------------------------------------------------------------

Eventhandler{type = "keypress", func = function(self, args)
	local w = Gui.mode ~= "game" and Widgets.focused_widget_prev
	if w and w.event then
		w:event(args)
		Action:event(args, {})
	else
		Action:event(args)
	end
end}

Eventhandler{type = "keyrelease", func = function(self, args)
	Action:event(args)
end}

Eventhandler{type = "mousepress", func = function(self, args)
	Action:event(args, Gui.mode ~= "game" and {})
end}

Eventhandler{type = "mouserelease", func = function(self, args)
	Action:event(args, Gui.mode ~= "game" and {})
end}

Eventhandler{type = "mousemotion", func = function(self, args)
	Action:event(args, Gui.mode ~= "game" and {})
end}
