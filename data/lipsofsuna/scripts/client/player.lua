Player = Class()
Player.light = Light{ambient = {0.3,0.3,0.3,1.0}, diffuse={0.6,0.6,0.6,1.0}, equation={1.5,0.3,0.04}, priority = 2}
Player.species = "aer" -- FIXME

local radian_wrap = function(x)
	if x < -math.pi then
		return math.pi - math.mod(x, math.pi)
	elseif x > math.pi then
		return -math.pi + math.mod(x, math.pi)
	else
		return x
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
	local pos,rot = clss:get_camera_transform_3rd()
	local node = clss.object:find_node{name = "#camera"}
	if node then pos = clss.object.position + clss.object.rotation * node end
	return pos,rot
end

Player.get_camera_transform_3rd = function(clss)
	local turn = clss.camera_turn_state + clss.turn_state
	local tilt = clss.camera_tilt_state - clss.tilt_state
	local spec = Species:find{name = clss.species}
	local ctr = spec and spec.camera_center or Vector(0, 2, 0)
	return clss.object.position + ctr, Quaternion:new_euler(turn, 0, tilt)
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

Player.camera = Camera{far = 40.0, fov = 1.1, mode = "third-person", near = 0.01, position_smoothing = 0.15, rotation_smoothing = 0.15}
Player.camera_tilt = 0
Player.camera_tilt_state = 0
Player.camera_turn = 0
Player.camera_turn_state = 0
Player.camera:zoom{rate = -8}
Player.update_camera = function(clss, secs)
	-- Update turning.
	clss.camera_turn_state = clss.camera_turn_state + clss.camera_turn * secs
	clss.camera_turn_state = radian_wrap(clss.camera_turn_state)
	clss.camera_turn = 0
	-- Update tilting.
	clss.camera_tilt_state = clss.camera_tilt_state - clss.camera_tilt * secs
	clss.camera_tilt_state = radian_wrap(clss.camera_tilt_state)
	clss.camera_tilt = 0
	-- Set the target transformation.
	local pos,rot = clss:get_camera_transform()
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
	Gui.scene.compass = -clss.rotation_curr.euler[1] / (2 * math.pi)
	Gui.scene.compass_quest = Views.Quests.inst:get_compass_direction()
end

Player.send_rotation = function(clss)
	local r = clss.rotation_curr
	clss.rotation_prev = r
	Network:send{packet = Packet(packets.PLAYER_TURN, "float", r.x, "float", r.y, "float", r.z, "float", r.w)}
end

Player.update_pose = function(clss, secs)
end
