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
	self.tilt_speed = 0
	self.tilt_state = 0
	self.turn_speed = 0
	self.turn_state = 0
	self:zoom{rate = -8}
	return self
end

ThirdPersonCamera.get_picking_ray = function(self)
	local pos,rot = self:get_transform()
	return pos,pos + rot * Vector(0,0,-50)
end

ThirdPersonCamera.get_transform = function(self)
	-- Calculate the rotation.
	local turn = self.turn_state + self.object.rotation.euler[1]
	local tilt = self.tilt_state + (self.object.tilt or 0)
	local rot = Quaternion{euler = {turn, 0, tilt}}
	-- Find the camera offset.
	local spec = self.object.spec
	local rel = spec and spec.camera_center or Vector(0, 2, 0)
	local turn1 = Quaternion{euler = {turn}}
	-- Calculate the center position.
	-- If there's room, the camera is placed slightly to the right so that
	-- the character doesn't obstruct the crosshair so badly.
	local best_center = self.object.position + rel
	local best_score = -1
	local stepl = 0.26
	local stepn = 5
	local steps = stepn
	local r1 = self.object.position + rel
	local r2 = r1 + turn1 * Vector(stepl * stepn)
	local ctr = Physics:cast_ray{collision_mask = self.collision_mask, src = r1, dst = r2}
	if ctr then
		steps = math.floor((ctr.point - r1).length / stepl)
		steps = math.max(0, steps - 1)
	end
	for i=0,steps do
		-- Calculate the score of the camera position.
		-- Camera positions that have the best displacement to the side and
		-- the most distance to the target before hitting a wall are favored.
		local center = r1 + turn1 * Vector(i * stepl)
		local back = Physics:cast_ray{collision_mask = self.collision_mask, src = center, dst = center + rot * Vector(0,0,5)}
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

ThirdPersonCamera.update = function(self, secs)
	if Action.dict_press[Keysym.LCTRL] then
		-- Update turning.
		self.turn_state = self.turn_state + self.turn_speed * secs
		self.turn_state = radian_wrap(self.turn_state)
		self.turn_speed = 0
		-- Update tilting.
		self.tilt_state = self.tilt_state - self.tilt_speed * secs
		self.tilt_state = radian_wrap(self.tilt_state)
		self.tilt_speed = 0
	else
		-- Reset mouse look.
		self.turn_state = self.turn_state * math.max(1 - 3 * secs, 0)
		self.tilt_state = self.tilt_state * math.max(1 - 3 * secs, 0)
	end
	-- Set the target transformation.
	local pos,rot = self:get_transform()
	if self.quake then
		local rnd = Vector(2*math.random()-1, 2*math.random()-1, 2*math.random()-1)
		pos = pos + rnd * 6 * math.min(1, self.quake)
		self.quake = self.quake - secs
		if self.quake < 0 then
			self.quake = 0
		end
	end
	self.target_position = pos
	self.target_rotation = rot
	-- Interpolate.
	self.viewport = {Gui.scene.x, Gui.scene.y, Gui.scene.width, Gui.scene.height}
	Camera.update(self, secs)
end
