Eventhandler{type = "keypress", func = function(self, args)
	Binding:event(args, Ui:handle_event(args))
end}

Eventhandler{type = "keyrelease", func = function(self, args)
	Binding:event(args, Ui:handle_event(args))
end}

Eventhandler{type = "mousepress", func = function(self, args)
	Binding:event(args, Ui:handle_event(args))
end}

Eventhandler{type = "mouserelease", func = function(self, args)
	Binding:event(args, Ui:handle_event(args))
end}

Eventhandler{type = "mousescroll", func = function(self, args)
	Binding:event(args, Ui:handle_event(args))
end}

Eventhandler{type = "mousemotion", func = function(self, args)
	Binding:event(args, Ui:handle_event(args))
end}

Eventhandler{type = "music-ended", func = function(self, args)
	Sound:cycle_music_track()
end}

Eventhandler{type = "quit", func = function(self, args)
	Program.quit = true
end}

local compasst = 0
local fpst = 0
Eventhandler{type = "tick", func = function(self, args)
	-- Update the connection status.
	if Client.connected and not Network.connected then
		Client:terminate_game()
		Client.options.host_restart = false
		Client.data.connection.active = false
		Client.data.connection.waiting = false
		Client.data.connection.connecting = false
		Client.data.connection.text = "Lost connection to the server!"
		Client.data.load.next_state = "start-game"
		Ui.state = "load"
	end
	-- Update the user interface state.
	Ui:update(args.secs)
	-- Update the window size.
	if Ui.was_resized then
		local v = Program.video_mode
		Client.options.window_width = v[1]
		Client.options.window_height = v[2]
		Client.options.fullscreen = v[3]
		Client.options.vsync = v[4]
		Client.options:save()
	end
	-- Update built models.
	while true do
		local msg = Client.threads.model_builder:pop_message()
		if not msg then break end
		if msg.model then
			local obj = Object:find{id = tonumber(msg.name)}
			if obj and obj.spec then
				msg.model:changed()
				obj:set_model(msg.model)
			end
		end
	end
	-- Update active objects.
	for k,v in pairs(Object.dict_active) do
		-- Update sound.
		if k.animated then
			k:update_sound(animt)
		end
		-- Interpolate positions.
		k:update_motion_state(args.secs)
		-- Update slots and special effects.
		k:update(args.secs)
		-- Maintain activity.
		if k.spec and k.spec.type ~= "species" then
			v = v - args.secs
			if v <= 0 then v = nil end
			Object.dict_active[k] = v
		end
	end
	-- Update player state.
	if Client.player_object then
		Player:update_pose(args.secs)
		Player:update_rotation(args.secs)
		Client.camera1.object = Client.player_object
		Client.camera3.object = Client.player_object
		Client.camera1:update(args.secs)
		Client.camera3:update(args.secs)
		Lighting:update(args.secs)
		-- Sound playback.
		local p,r = Client.player_object:find_node{name = "#neck", space = "world"}
		if p then
			Sound.listener_position = p
			Sound.listener_rotation = r
		else
			Sound.listener_position = Client.player_object.position + Vector(0,1.5,0)
			Sound.listener_rotation = Client.player_object.rotation
		end
		local vel = Client.player_object.velocity
		if vel then Sound.listener_velocity = vel end
		-- Refresh the active portion of the map.
		Client.player_object:refresh()
	end
	-- Update text bubbles.
	-- Must be done after camera for the bubbles to stay fixed.
	for k,v in pairs(TextBubble.dict) do
		k:update(args.secs)
	end
	-- Update the 3D cursor.
	-- This really needs to be done every frame since the 3rd person
	-- camera suffers greatly from any big cursor position changes.
	if Client.player_object and Ui.pointer_grab then
		Player:pick_look()
	else
		Target.target_object = nil
	end
end}
