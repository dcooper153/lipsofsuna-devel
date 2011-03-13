Eventhandler{type = "keypress", func = function(self, args)
	if Gui.mode ~= "game" and Widgets:handle_event(args) then
		Action:event(args, {})
	else
		Action:event(args)
	end
end}

Eventhandler{type = "keyrelease", func = function(self, args)
	Action:event(args)
end}

Eventhandler{type = "mousepress", func = function(self, args)
	if Gui.mode ~= "game" then
		Widgets:handle_event(args)
		Action:event(args, {})
	else
		Action:event(args)
	end
end}

Eventhandler{type = "mouserelease", func = function(self, args)
	if Gui.mode ~= "game" then
		Widgets:handle_event(args)
		Action:event(args, {})
	else
		Action:event(args)
	end
end}

Eventhandler{type = "mousemotion", func = function(self, args)
	if Gui.mode ~= "game" then
		Widgets:handle_event(args)
		Action:event(args, {})
	else
		Action:event(args)
	end
end}

Eventhandler{type = "quit", func = function(self, args)
	Program.quit = true
end}

local animt = 0
local ipolt = 0
Eventhandler{type = "tick", func = function(self, args)
	-- Update the cursor.
	Widgets.Cursor.inst:update()
	-- Update animations.
	animt = animt + args.secs
	if animt > 0.2 * (1 - Views.Options.inst.animation_quality) then
		for k,v in pairs(Object.objects) do
			if v.animated then
				v:update_animations{secs = animt}
				v:deform_mesh()
			end
		end
		animt = 0
	end
	-- Interpolate objects.
	ipolt = math.min(ipolt + args.secs, 1)
	while ipolt > 1/60 do
		for k,v in pairs(Object.objects) do
			v:update_motion_state(1/60)
		end
		ipolt = ipolt - 1/60
	end
	-- Update special effects.
	for k,v in pairs(Object.objects) do
		v:update(secs)
	end
	-- Update equipment positions.
	for k,v in pairs(Slots.dict_owner) do
		v:update()
	end
	-- Update player state.
	if Player.object then
		Player:update_pose(args.secs)
		Player:update_rotation(args.secs)
		Player:update_camera(args.secs)
		-- Update the light ball.
		Player.light.position = Player.object.position + Player.object.rotation * Vector(0, 2, -3)
		Player.light.enabled = true
		-- Sound playback.
		Sound.listener_position = Player.object.position
		Sound.listener_rotation = Player.object.rotation
		Sound.listener_velocity = Player.object.velocity
		-- Refresh the active portion of the map.
		Player.object:refresh()
	end
end}
