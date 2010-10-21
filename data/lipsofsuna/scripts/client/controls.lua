controls = {}

-- Camera.
controls.CAMERA_SWITCH = Action{id = "camera-switch", name = "Switch view mode", desc = "Switch view mode"}
controls.CAMERA_SWITCH.callback = function(event)
	if event.active then
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
	end
end
controls.CAMERA_TILT = Action{id = "camera-tilt", name = "Tilt camera", desc = "Tilt the camera"}
controls.CAMERA_TILT.callback = function(event)
	Player.camera_tilt = event.value
end
controls.CAMERA_TURN = Action{id = "camera-turn", name = "Turn camera", desc = "Turn the camera"}
controls.CAMERA_TURN.callback = function(event)
	Player.camera_turn = event.value
end
controls.CAMERA_ZOOM = Action{id = "camera-zoom", name = "Zoom camera", desc = "Zoom the camera"}
controls.CAMERA_ZOOM.callback = function(event)
	Player.camera:zoom{rate = event.value}
end

-- Crouch.
controls.CROUCH = Action{id = "crouch", name = "Crouch", desc = "Crouch or fly downwards"}
controls.CROUCH.callback = function(event)
	Network:send{packet = Packet(packets.CROUCH, "bool", event.active)}
end

-- Cycle focus.
controls.CYCLE_FOCUS = Action{id = "cycle-focus", name = "Cycle focus", desc = "Cycle focus"}
controls.CYCLE_FOCUS.callback = function(event)
	if event.active then
		Widgets:cycle_focus{backward = event.value < 0}
	end
end

-- Cycle window focus.
controls.CYCLE_WINDOW_FOCUS = Action{id = "cycle-window-focus", name = "Cycle window focus", desc = "Cycle window focus"}
controls.CYCLE_WINDOW_FOCUS.callback = function(event)
	if event.active then
		Widgets:cycle_window_focus{backward = event.value < 0}
	end
end

-- Editing.
controls.EDIT_SELECT = Action{id = "edit-select", name = "Select", desc = "Select an object for editing"}
controls.EDIT_SELECT.callback = function(event)
	if event.active then
		local pos,object = Gui.scene:pick()
		if object then
			object.selected = not object.selected
		end
	end
end

-- Jump.
controls.JUMP = Action{id = "jump", name = "Jump", desc = "Jump or fly upwards"}
controls.JUMP.callback = function(event)
	Network:send{packet = Packet(packets.JUMP)}
end

-- Macro.
controls.MACRO = Action{id = "macro", name = "Chat macro", desc = "Send a predefined chat message"}
controls.MACRO.callback = function(event)
	if event.active then
		Network:send{packet = Packet(packets.CHAT, "string", event.params)}
	end
end

-- Menu.
controls.MENU = Action{id = "menu", name = "Menu", desc = "Show or hide the menu"}
controls.MENU.callback = function(event)
	if event.active then
		if Target:active() then
			Target:cancel()
		else
			if Client.moving then
				Gui.menus:open{level = 1, widget = Gui.menu_widget_main}
			else
				Gui.menus:close()
			end
		end
	end
end

-- Move.
controls.MOVE = Action{id = "move", name = "Move", desc = "Move forward or backward"}
controls.MOVE.callback = function(event)
	Network:send{packet = Packet(packets.PLAYER_MOVE, "int8", event.value * -127)}
end

-- Pick up.
controls.PICKUP = Action{id = "pick-up", name = "Pick up", desc = "Pick up items"}
controls.PICKUP.callback = function(event)
	if event.active then
		if Client.moving then
			controls.MENU.callback(event)
		end
		Commands:pickup()
	end
end

-- Quickslots.
controls.QUICKSLOT = Action{id = "quickslot", name = "Quickslot", desc = "Activate a quickslot"}
controls.QUICKSLOT.callback = function(event)
	if event.params then
		Quickslots:activate(tonumber(event.params))
	end
end

-- Run.
controls.RUN = Action{id = "run", name = "Run", desc = "Move faster"}
controls.RUN.callback = function(event)
	Network:send{packet = Packet(packets.RUN, "bool", event.active)}
end

-- Screenshot.
controls.SCREENSHOT = Action{id = "screenshot", name = "Screenshot", desc = "Save a screenshot"}
controls.SCREENSHOT.callback = function(event)
	if event.active then
		local n = Client:screenshot()
		Gui.chat_history:append{text = "Screenshot: " .. n}
	end
end

-- Shoot.
controls.SHOOT = Action{id = "shoot", name = "Shoot", desc = "Shoot or attack"}
controls.SHOOT.enabled = false
controls.SHOOT.callback = function(event)
	if event.active then
		Network:send{packet = Packet(packets.SHOOT)}
	end
end

-- Strafe.
controls.STRAFE = Action{id = "strafe", name = "Strafe", desc = "Strafe left or right"}
controls.STRAFE.enabled = true
controls.STRAFE.callback = function(event)
	if event.active then
		Network:send{packet = Packet(packets.STRAFE, "int8", event.value * 127)}
	else
		Network:send{packet = Packet(packets.STRAFE, "int8", 0)}
	end
end

-- Skill.
controls.SKILL = Action{id = "skill", name = "Skill", desc = "Uses a skill"}
controls.SKILL.callback = function(event)
	if event.params then
		Network:send{packet = Packet(packets.SKILL, "string", event.params, "bool", event.active)}
	end
end

-- Turn.
controls.TURN = Action{id = "turn", name = "Turn", desc = "Turn left or right"}
controls.TURN.callback = function(event)
	Player.turn = -event.value
end

-- Tilt.
controls.TILT = Action{id = "tilt", name = "Tilt", desc = "Tilt up or down"}
controls.TILT.callback = function(event)
	Player.tilt = event.value
end

-- Use.
controls.USE = Action{id = "use", name = "Use", desc = "Chat, loot, read, ..."}
controls.USE.callback = function(event)
	if event.active then
		if Client.moving then
			controls.MENU.callback(event)
		end
		Commands:use()
	end
end

Binding{action = "cycle-focus", key = 9} --TAB
Binding{action = "cycle-focus", key = 9, mods = 0x1, mult = -1.0} --LSHIFT+TAB
Binding{action = "cycle-window-focus", key = 9, mods = 0x40} --LCTRL+TAB
Binding{action = "cycle-window-focus", key = 9, mods = 0x41, mult = -1.0} --LCTRL+LSHIFT+TAB
Binding{action = "menu", key = 27} --ESCAPE

Binding{action = "move", key = 119, mult = -1.0} --w
Binding{action = "strafe", key = 97, mult = -1.0} --a
Binding{action = "move", key = 115} --s
Binding{action = "strafe", key = 100} --d
Binding{action = "jump", key = 99} --c
Binding{action = "crouch", key = 120} --x
Binding{action = "pick-up", key = 44} --COMMA
Binding{action = "use", key = 32} --SPACE
Binding{action = "run", key = 304} --LSHIFT
Binding{action = "shoot", key = 305} --RCTRL
Binding{action = "camera-switch", key = 121} --y
Binding{action = "screenshot", key = 316} --PRINT

Binding{action = "shoot", mousebutton = 1} --BUTTON1
Binding{action = "run", mousebutton = 3} --BUTTON3
Binding{action = "camera-zoom", mousebutton = 4, mult = -2.0} --BUTTON4
Binding{action = "camera-zoom", mousebutton = 5, mult = 2.0} --BUTTON5
Binding{action = "edit-select", mousebutton = 1, mods = 0x40} --LCTRL+BUTTON1
Binding{action = "turn", mousedelta = 0, mult = 1.0} --AXIS0
Binding{action = "tilt", mousedelta = 1, mult = 0.5} --AXIS1
Binding{action = "camera-turn", mousedelta = 0, mods = 0x40, mult = 1.0} --AXIS0+LCTRL
Binding{action = "camera-tilt", mousedelta = 1, mods = 0x40, mult = 0.5} --AXIS1+LCTRL
--Binding{action = "camera-tilt", key = 100, mult = 10.0} --d
--Binding{action = "camera-tilt", key = 99, mult = -10.0} --c

Binding{action = "jump", joystickbutton = 1} --BUTTON1
Binding{action = "run", joystickbutton = 2} --BUTTON2
Binding{action = "shoot", joystickbutton = 3} --BUTTON3
Binding{action = "use", joystickbutton = 4} --BUTTON4
Binding{action = "move", joystickaxis = 1} --AXIS1
Binding{action = "turn", joystickaxis = 0} --AXIS0

for i = 1,12 do
	Binding{action = "quickslot", params = "" .. i, key = 282 + i - 1} --F1..F12
end
