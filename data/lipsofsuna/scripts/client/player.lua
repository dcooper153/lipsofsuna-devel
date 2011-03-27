Player = Class()
Player.light = Light{ambient = {0.3,0.3,0.3,1.0}, diffuse={0.6,0.6,0.6,1.0}, equation={1.5,0.3,0.04}, priority = 2}
Player.species = "aer" -- FIXME

local radian_wrap = function(x)
	if x < -math.pi then
		return x + 2 * math.pi
	elseif x > math.pi then
		return x - 2 * math.pi
	else
		return x
	end
end

--- Applies a world space quake.
-- @param clss Player class.
-- @param point Quake point in world space.
-- @param magnitude Quake magnitude.
Player.apply_quake = function(clss, point, magnitude)
	if point and magnitude and clss.object then
		local dist = (clss.object.position - point).length
		local quake = math.min(math.max(magnitude / (0.05 * dist * dist + 0.5), 0), 1)
		clss.camera.quake = math.max(clss.camera.quake or 0, quake)
	end
end

Player.get_camera_transform = function(clss)
	if clss.camera.mode == "first-person" then
		return clss:get_camera_transform_1st()
	else
		return clss:get_camera_transform_3rd()
	end
end

Player.get_camera_transform_1st = function(clss)
	-- Calculate the rotation.
	local turn = clss.camera_turn_state + clss.turn_state
	local tilt = clss.camera_tilt_state - clss.tilt_state
	local rot = Quaternion:new_euler(turn, 0, tilt)
	-- Find the camera offset.
	local spec = Species:find{name = clss.species}
	local rel = spec and spec.camera_center or Vector(0, 2, 0)
	local pos = clss.object.position + clss.object.rotation * rel
	local npos,nrot = clss.object:find_node{name = "#camera"}
	if npos then
		-- The position of the camera node is always used but the rotation is
		-- ignored most of the time since the rotation component of the node
		-- is highly annoying in many animations. However, the rotation is used
		-- when the player dies to avoid the camera facing inside the corpse.
		pos = clss.object.position + clss.object.rotation * npos
		if clss.object.dead then
			rot = rot * nrot
		end
	end
	return pos,rot
end

Player.get_camera_transform_3rd = function(clss)
	-- Calculate the rotation.
	local turn = clss.camera_turn_state + clss.turn_state
	local tilt = clss.camera_tilt_state - clss.tilt_state
	local rot = Quaternion:new_euler(turn, 0, tilt)
	-- Find the camera offset.
	local spec = Species:find{name = clss.species}
	local rel = spec and spec.camera_center or Vector(0, 2, 0)
	local turn1 = Quaternion:new_euler(turn)
	-- Calculate the center position.
	-- If there's room, the camera is placed slightly to the right so that
	-- the character doesn't obstruct the crosshair so badly.
	local best_center = clss.object.position + rel
	local best_score = -1
	local stepl = 0.26
	local stepn = 5
	local steps = stepn
	local r1 = clss.object.position + rel
	local r2 = r1 + turn1 * Vector(stepl * stepn)
	local ctr = Physics:cast_ray{collision_mask = Physics.MASK_CAMERA, src = r1, dst = r2}
	if ctr then
		steps = math.floor((ctr.point - r1).length / stepl)
		steps = math.max(0, steps - 1)
	end
	for i=0,steps do
		-- Calculate the score of the camera position.
		-- Camera positions that have the best displacement to the side and
		-- the most distance to the target before hitting a wall are favored.
		local center = r1 + turn1 * Vector(i * stepl)
		local back = Physics:cast_ray{collision_mask = Physics.MASK_CAMERA, src = center, dst = center + rot * Vector(0,0,5)}
		local dist = back and (back.point - center).length or 5
		local score = 3 * dist + (i + 1)
		-- Choose the best camera center.
		if best_score <= score then
			local side = i * stepl
			best_score = score
			best_center = center
		end
	end
	-- Return the final transformation.
	return best_center, rot
end

Player.get_picking_ray_1st = function(clss)
	if not clss.object then return end
	local spec = Species:find{name = clss.species}
	-- Calculate the rotation.
	local euler = Player.object.rotation.euler
	euler[3] = euler[3] - clss.tilt_state
	local rot = Quaternion:new_euler(euler)
	-- Project the ray.
	local ctr = Player.object.position + Player.object.rotation * spec.aim_ray_center
	return ctr,ctr + rot * Vector(0, 0, -5)
	--local pos,rot = clss:get_camera_transform_1st()
	--return pos,pos + rot * Vector(0,0,-5)
end

Player.get_picking_ray_3rd = function(clss)
	local pos,rot = clss:get_camera_transform_3rd()
	return pos,pos + rot * Vector(0,0,-50)
end

Player.get_ignored_objects = function(clss)
	local ignore = {clss.crosshair, clss.object}
	local slots = Slots:find{owner = clss.object}
	if slots then
		for k,v in pairs(slots.slots) do
			table.insert(ignore, v)
		end
	end
	return ignore
end

Player.pick_look = function(clss)
	-- Make sure that the player is logged in.
	if not clss.object then
		Target.target_object = nil
		clss.crosshair.realized = false
		return
	end
	-- Ray pick an object in front of the player.
	local r1,r2 = clss:get_picking_ray_1st()
	local p,o = Target:pick_ray{ray1 = r1, ray2 = r2}
	Target.target_object = o
	if o then
		if o.name and o.name ~= "" then
			Gui:set_target_text("Interact with " .. o.name)
		else
			Gui:set_target_text("Interact")
		end
		set = true
	else
		Gui:set_target_text()
	end
	-- Update the crosshair.
	clss.crosshair.position = (p or r2) - (r2 - r1):normalize() * 0.1
	clss.crosshair.realized = true
end

Player.camera = Camera{collision_mask = Physics.MASK_CAMERA, far = 40.0, fov = 1.1, mode = "third-person", near = 0.01, position_smoothing = 0.15, rotation_smoothing = 0.15}
Player.camera_tilt = 0
Player.camera_tilt_state = 0
Player.camera_turn = 0
Player.camera_turn_state = 0
Player.camera:zoom{rate = -8}
Player.update_camera = function(clss, secs)
	if Action.dict_press[Keys.LCTRL] then
		-- Update turning.
		clss.camera_turn_state = clss.camera_turn_state + clss.camera_turn * secs
		clss.camera_turn_state = radian_wrap(clss.camera_turn_state)
		clss.camera_turn = 0
		-- Update tilting.
		clss.camera_tilt_state = clss.camera_tilt_state - clss.camera_tilt * secs
		clss.camera_tilt_state = radian_wrap(clss.camera_tilt_state)
		clss.camera_tilt = 0
	else
		-- Reset mouse look.
		clss.camera_turn_state = clss.camera_turn_state * math.max(1 - 3 * secs, 0)
		clss.camera_tilt_state = clss.camera_tilt_state * math.max(1 - 3 * secs, 0)
	end
	-- Set the target transformation.
	local pos,rot = clss:get_camera_transform()
	if clss.camera.quake then
		local rnd = Vector(2*math.random()-1, 2*math.random()-1, 2*math.random()-1)
		pos = pos + rnd * 6 * math.min(1, clss.camera.quake)
		clss.camera.quake = clss.camera.quake - secs
		if clss.camera.quake < 0 then
			clss.camera.quake = 0
		end
	end
	Player.camera.target_position = pos
	Player.camera.target_rotation = rot
	-- Interpolate.
	Player.camera.viewport = {Gui.scene.x, Gui.scene.y, Gui.scene.width, Gui.scene.height}
	Player.camera:update(secs)
end

Player.tilt = 0
Player.turn = 0
Player.tilt_state = 0
Player.turn_state = 0
Player.rotation_curr = Quaternion()
Player.rotation_prev = Quaternion()
Player.rotation_sync_timer = 0
Player.update_rotation = function(clss, secs)
	if clss.object.dead then return end
	local spec = Species:find{name = Player.object.race}
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
	local r = Quaternion:new_euler(clss.turn_state, 0, -clss.tilt_state)
	clss.object:update_rotation(r)
	clss.rotation_curr = r
	-- Sync rotation with the server.
	-- Rotation takes at most 0.25 seconds to fully synchronize. Large changes
	-- are sent immediately whereas smaller changes are grouped together to
	-- reduce useless network traffic.
	clss.rotation_sync_timer = clss.rotation_sync_timer + secs
	if (clss.rotation_prev - r).length > math.max(0, 0.1 - 0.4 * clss.rotation_sync_timer) then
		clss:send_rotation()
	end
	-- Update compass rotation.
	Gui.scene.compass = math.pi - clss.rotation_curr.euler[1]
	Gui.scene.compass_quest = Views.Quests.inst:get_compass_direction()
end

Player.send_rotation = function(clss)
	local r = clss.rotation_curr
	clss.rotation_prev = r
	Network:send{packet = Packet(packets.PLAYER_TURN, "float", r.x, "float", r.y, "float", r.z, "float", r.w)}
end

Player.update_pose = function(clss, secs)
end

-- Periodically check if there's an object in front of the player.
Timer{delay = 0.05, func = function()
	if Player.object and Client.moving then
		Player:pick_look()
	else
		Target.target_object = nil
	end
end}
