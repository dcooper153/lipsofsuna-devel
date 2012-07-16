PlayerState = Class()

local radian_wrap = function(x)
	local y = x
	while y < -math.pi do
		y = y + 2 * math.pi
	end
	while y > math.pi do
		y = y - 2 * math.pi
	end
	return y
end

PlayerState.get_ignored_objects = function(clss)
	local ignore = {Client.player_object}
	local slots = Client.player_object.slots
	if slots then
		for k,v in pairs(slots.slots) do
			table.insert(ignore, v)
		end
	end
	return ignore
end

PlayerState.pick_look = function(clss)
	-- Make sure that the player is logged in.
	if not Client.player_object then
		Target.target_object = nil
		return
	end
	-- Ray pick an object in front of the player.
	local r1,r2 = Client.camera1:get_picking_ray()
	if not r1 then return end
	local p,o = Target:pick_ray{ray1 = r1, ray2 = r2}
	-- Update highlighting.
	if o ~= Target.target_object then
		if Target.target_object then
			Target.target_object.render:set_effect()
		end
		if o then
			o.render:set_effect{shader = "highlight"}
		end
	end
	Target.target_object = o
	-- Update the interaction text.
	if o and o.spec and o.spec.interactive then
		local action = Binding.dict_name["use"]
		local key = action and action.key1 and Keycode[action.key1] or "--"
		if o.name and o.name ~= "" then
			Client:set_target_text(string.format("[%s] Interact with %s", key, o.name))
		elseif o.spec.name then
			Client:set_target_text(string.format("[%s] Interact with %s", key, o.spec.name))
		else
			Client:set_target_text(string.format("[%s] Interact", key))
		end
		set = true
	else
		Client:set_target_text()
	end
	-- Update the 3D crosshair position.
	Client.crosshair_position = (p or r2) - (r2 - r1):normalize() * 0.1
end

PlayerState.tilt = 0
PlayerState.turn = 0
PlayerState.tilt_state = 0
PlayerState.turn_state = 0
PlayerState.rotation_curr = Quaternion()
PlayerState.rotation_prev = Quaternion()
PlayerState.rotation_sync_timer = 0
PlayerState.update_rotation = function(clss, secs)
	if Client.player_object.dead then return end
	local spec = Client.player_object.spec
	-- Update turning.
	clss.turn_state = clss.turn_state + clss.turn * secs
	clss.turn_state = radian_wrap(clss.turn_state)
	clss.turn = 0
	-- Update tilting.
	clss.tilt_state = clss.tilt_state + clss.tilt * secs
	if spec then
		clss.tilt_state = math.min(spec.tilt_limit, clss.tilt_state)
		clss.tilt_state = math.max(-spec.tilt_limit, clss.tilt_state)
	end
	clss.tilt = 0
	-- Update rotation.
	if Game.initialized then
		Game.messaging:client_event("rotate", clss.turn_state, clss.tilt_state)
	end
end

PlayerState.update_pose = function(clss, secs)
end
