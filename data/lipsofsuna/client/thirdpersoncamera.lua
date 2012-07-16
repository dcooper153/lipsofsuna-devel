local radian_wrap = function(x)
	if x < -math.pi then return x + 2 * math.pi
	elseif x > math.pi then return x - 2 * math.pi
	else return x end
end

ThirdPersonCamera = Class(Camera)

--- Creates a new third person camera.
-- @param clss Third person camera class.
-- @param args Arguments.
-- @return Third person camera.
ThirdPersonCamera.new = function(clss, args)
	local self = Camera.new(clss, args)
	self.collision_mask = Physics.MASK_CAMERA
	self.mode = "third-person"
	self.displacement_smoothing_rate = 0.1
	self.tilt_speed = 0
	self.tilt_state = 0
	self.turn_speed = 0
	self.turn_state = 0
	self.timer = 1/60
	self.tick = 1/60
	self:zoom{rate = -8}
	return self
end

ThirdPersonCamera.get_picking_ray = function(self)
	local pos,rot = self:get_transform()
	return pos,pos + rot * Vector(0,0,-50)
end

--- Calculates the camera center displacement.<br/>
--
-- If there's room, the camera is placed slightly to the right so that
-- the character doesn't obstruct the crosshair so badly.
--
-- @param self Third person camera.
-- @return Displacement in object space.
ThirdPersonCamera.get_position_displacement = function(self, pos, rot, turn)
	-- Determine the number of iterations.
	local stepl = 0.12
	local stepn = 6
	local steps = stepn
	local ctr = Physics:cast_ray{collision_mask = self.collision_mask, src = pos, dst = pos + turn * Vector(stepl * stepn)}
	if ctr then
		steps = math.floor((ctr.point - pos).length / stepl)
		steps = math.max(0, steps - 1)
	end
	-- Calculate the displacement iteratively.
	local best_score = -1
	local best_index = 0
	for i=0,steps do
		-- Favor positions that have the best displacement to the side and
		-- the most distance to the target before hitting a wall.
		local center = pos + turn * Vector(i * stepl)
		local back = Physics:cast_ray{collision_mask = self.collision_mask, src = center, dst = center + rot * Vector(0,0,5)}
		local dist = back and (back.point - center).length or 5
		local score = 3 * dist + (i + 1)
		-- Prevent the crosshair corrected rotation from diverging too much
		-- from the look direction when the target is very close.
		if Client.crosshair_position then
			local dir = (Client.crosshair_position - center):normalize()
			local rot1 = Quaternion{dir = dir, up = Vector(0,1,0)}
			local dir1 = rot * Vector(0,0,-1)
			local dir2 = rot1 * Vector(0,0,-1)
			if i > 0 and dir1:dot(dir2) < 0.95 then score = -2 end
		end
		-- Choose the best camera center.
		if best_score <= score then
			best_score = score
			best_index = i
		end
	end
	-- Return the displacement.
	return best_index/stepn, Vector(best_index * stepl)
end

ThirdPersonCamera.get_rotation_displacement = function(self, pos, rot, ratio)
	-- Rotate the 3D cursor to the screen center.
	local cur = Client.crosshair_position or pos + rot * Vector(0,0,-5)
	local dir = (cur - pos):normalize()
	local drot = Quaternion{dir = dir, up = Vector(0,1,0)}
	-- Damp cases when the 3D cursor is too close.
	drot = drot:nlerp(rot, ratio)
	-- Convert the rotation to the object space.
	return drot * rot.conjugate
end

ThirdPersonCamera.get_transform = function(self)
	-- Calculate the initial rotation.
	local turn = self.turn_state + self.object:get_turn_angle()
	local tilt = self.tilt_state + self.object:get_tilt_angle()
	local rot = Quaternion{euler = {turn, 0, tilt}}
	local turn1 = Quaternion{euler = {turn}}
	-- Calculate the initial center position.
	local spec = self.object.spec
	local rel = spec and spec.camera_center or Vector(0, 2, 0)
	local pos = self.object.position + rel + rot * Vector(0,0,0.5)
	-- Calculate the sideward displacement.
	local displ_pos = Vector()
	local displ_rot = Quaternion()
	if self.rotation_smoothing < 1 then
		local r,p = self:get_position_displacement(pos, rot, turn1)
		displ_pos = p
		displ_rot = self:get_rotation_displacement(pos + turn1 * p, rot, r)
	end
	-- Return the final transformation.
	return pos,rot,turn1,displ_pos,displ_rot
end

ThirdPersonCamera.update = function(self, secs)
	self.timer = self.timer + secs
	while self.timer > self.tick do
		self.timer = self.timer - self.tick
		if Operators.camera:get_rotation_mode() then
			-- Update turning.
			self.turn_state = self.turn_state + self.turn_speed * self.tick
			self.turn_state = radian_wrap(self.turn_state)
			self.turn_speed = 0
			-- Update tilting.
			self.tilt_state = self.tilt_state - self.tilt_speed * self.tick
			self.tilt_state = radian_wrap(self.tilt_state)
			self.tilt_speed = 0
		else
			-- Reset mouse look.
			self.turn_state = self.turn_state * math.max(1 - 3 * self.tick, 0)
			self.tilt_state = self.tilt_state * math.max(1 - 3 * self.tick, 0)
		end
		-- Calculate the target transformation.
		local pos,rot,turn,dpos,drot = self:get_transform()
		if self.rotation_smoothing < 1 and self.prev_target_pos then
			dpos:lerp(self.prev_target_pos, 1 - self.displacement_smoothing_rate)
			drot = drot:nlerp(self.prev_target_rot, self.displacement_smoothing_rate)
		end
		self.prev_target_pos = dpos
		self.prev_target_rot = drot
		-- Mix in the free rotation mode.
		local mix = math.max(math.abs(self.turn_state),math.abs(self.tilt_state))
		drot = drot:nlerp(Quaternion(), 1 - math.min(1, 30 * mix))
		-- Min in the camera quake.
		if self.quake then
			local rnd = Vector(2*math.random()-1, 2*math.random()-1, 2*math.random()-1)
			pos = pos + rnd * 6 * math.min(1, self.quake)
			self.quake = self.quake - self.tick
			if self.quake < 0 then
				self.quake = 0
			end
		end
		-- Set the target transformation.
		self.target_position = pos + turn * dpos
		self.target_rotation = drot * rot
		-- Interpolate.
		Camera.update(self, self.tick)
	end
end
