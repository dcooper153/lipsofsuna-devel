Eventhandler{type = "keypress", func = function(self, args)
	if Views.Controls.inst.editing_binding then
		Views.Controls.inst:input(args)
		Action:event(args, {})
		return
	end
	if Gui.mode ~= "game" and Widgets:handle_event(args) then
		-- Widget input.
		Action:event(args, {})
	elseif Gui.chat_active then
		-- Chat input.
		if args.code == Keysym.ESCAPE then
			Gui.chat_active = false
		else
			Gui.chat_entry:event(args)
			if args.code == Keysym.RETURN then
				Gui.chat_active = false
			end
		end
		Action:event(args, {})
	else
		-- Game controls.
		Action:event(args)
	end
end}

Eventhandler{type = "keyrelease", func = function(self, args)
	if Views.Controls.inst.editing_binding then
		Views.Controls.inst:input(args)
		Action:event(args, {})
		return
	end
	Action:event(args)
end}

Eventhandler{type = "mousepress", func = function(self, args)
	if Views.Controls.inst.editing_binding then
		Views.Controls.inst:input(args)
		Action:event(args, {})
		return
	end
	if args.button == 4 and Drag.drag then
		Drag:change_count(1)
	elseif args.button == 5 and Drag.drag then
		Drag:change_count(-1)
	elseif Gui.mode ~= "game" then
		Widgets:handle_event(args)
		Action:event(args, {})
	else
		Action:event(args)
	end
end}

Eventhandler{type = "mouserelease", func = function(self, args)
	if Views.Controls.inst.editing_binding then
		Views.Controls.inst:input(args)
		Action:event(args, {})
		return
	end
	if Gui.mode ~= "game" then
		Widgets:handle_event(args)
		Action:event(args, {})
	else
		Action:event(args)
	end
end}

Eventhandler{type = "mousemotion", func = function(self, args)
	if Views.Controls.inst.editing_binding then
		Views.Controls.inst:input(args)
		Action:event(args, {})
		return
	end
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
	if Object.deform_mesh then
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
	end
	-- Interpolate objects.
	ipolt = math.min(ipolt + args.secs, 1)
	while ipolt > 1/60 do
		for k,v in pairs(Object.objects) do
			v:update_motion_state(1/60)
		end
		ipolt = ipolt - 1/60
	end
	-- Update player state.
	if Player.object then
		Player:update_pose(args.secs)
		Player:update_rotation(args.secs)
		Player:update_camera(args.secs)
		Player:update_light(args.secs)
		-- Sound playback.
		Sound.listener_position = Player.object.position
		Sound.listener_rotation = Player.object.rotation
		local vel = Player.object.velocity
		if vel then Sound.listener_velocity = vel end
		-- Refresh the active portion of the map.
		Player.object:refresh()
	end
	-- Update slots and special effects of objects.
	for k,v in pairs(Object.objects) do
		v:update(args.secs)
	end
end}
