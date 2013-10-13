--- Third person camera.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.camera.third_person_camera.third_person_camera
-- @alias ThirdPersonCamera

local Camera = require("system/camera")
local Class = require("system/class")
local MathUtils = require("system/math/utils")
local Physics = require("system/physics")
local PhysicsConsts = require("core/server/physics-consts")

--- Third person camera.
-- @type ThirdPersonCamera
local ThirdPersonCamera = Class("ThirdPersonCamera", Camera)

--- Creates a new third person camera.
-- @param clss Third person camera class.
-- @param args Arguments.
-- @return Third person camera.
ThirdPersonCamera.new = function(clss, args)
	local self = Camera.new(clss, args)
	self:set_collision_mask(PhysicsConsts.MASK_CAMERA)
	self.mode = "third-person"
	self.displacement_smoothing_rate = 0.1
	self.tilt_speed = 0
	self.tilt_state = 0
	self.turn_speed = 0
	self.turn_state = 0
	self.timer = 1/60
	self.tick = 1/60
	self.__zoom = 4
	self.__death_ipol = 1
	return self
end

--- Quakes the camera.
-- @param self Camera.
-- @param amount Magnitude in the range of [0, 1].
ThirdPersonCamera.quake = function(self, amount)
	self.__quake_vector = Vector()
	self.__quake_force = Vector(2*math.random()-1, 2*math.random()-1, 2*math.random()-1)
	self.__quake_force:normalize()
	self.__quake_force:multiply(10 * amount)
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
-- @param pos Non-displaced position.
-- @param rot Non-displaced rotation.
-- @param turn Non-displaced turning rotation.
-- @return Displacement in object space.
ThirdPersonCamera.get_position_displacement = function(self, pos, rot, turn)
	-- Determine the number of iterations.
	local stepl = 0.12
	local stepn = 6
	local steps = stepn
	local ctr = Physics:cast_ray(pos, Vector(stepl * stepn):transform(turn, pos), self:get_collision_mask())
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
		local center = Vector(i * stepl):transform(turn, pos)
		local back = Physics:cast_ray(center, Vector(0,0,5):transform(rot, center), self:get_collision_mask())
		local dist = back and (back.point - center).length or 5
		local score = 3 * dist + (i + 1)
		-- Prevent the crosshair corrected rotation from diverging too much
		-- from the look direction when the target is very close.
		local crosshair = Client.player_state:get_crosshair_position()
		if crosshair then
			local dir = (crosshair - center):normalize()
			local rot1 = Quaternion{dir = dir, up = Vector(0,1,0)}
			local dir1 = Vector(0,0,-1):transform(rot)
			local dir2 = Vector(0,0,-1):transform(rot1)
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
	local cur = Client.player_state:get_crosshair_position() or Vector(0,0,-5):transform(rot, pos)
	local dir = cur:copy():subtract(pos):normalize()
	local drot = Quaternion{dir = dir, up = Vector(0,1,0)}
	-- Damp cases when the 3D cursor is too close.
	drot:nlerp(rot, ratio)
	-- Convert the rotation to the object space.
	return drot:concat(rot.conjugate)
end

--- Calculates the transformation of the center of the view.
-- @param self ThirdPersonCamera.
-- @return Vector, quaternion, quaternion, vector, quaternion.
ThirdPersonCamera.get_center_transform = function(self)
	-- Calculate the initial rotation.
	local ipol = self.__death_ipol
	local turn = self.turn_state + self.object:get_turn_angle() * (1 - ipol)
	local tilt = (self.tilt_state + self.object:get_tilt_angle()) * (1 - ipol) - 0.7 * ipol
	local rot = Quaternion{euler = {turn, 0, tilt}}
	local turn1 = Quaternion{euler = {turn}}
	-- Calculate the initial center position.
	local spec = self.object.spec
	local rel = spec and spec.camera_center or Vector(0, 2, 0)
	local pos = Vector(0,0,0.5):transform(rot, rel):add(self.object:get_position())
	-- Calculate the sideward displacement.
	local displ_pos = Vector()
	local displ_rot = Quaternion()
	if self:get_rotation_smoothing() < 1 then
		local r,p = self:get_position_displacement(pos, rot, turn1)
		displ_pos = p
		displ_rot = self:get_rotation_displacement(p:copy():transform(turn1, pos), rot, r)
	end
	-- Add the extra displacement.
	if self.displacement then
		displ_pos = displ_pos + self.displacement
	end
	-- Return the final transformation.
	return pos,rot,turn1,displ_pos,displ_rot
end

--- Calculates the transformation of the camera lens.
-- @param self ThirdPersonCamera.
-- @return Vector, quaternion.
ThirdPersonCamera.get_eye_transform = function(self)
	-- Calculate the target transformation.
	local pos,rot,turn,dpos,drot = self:get_center_transform()
	if self:get_rotation_smoothing() < 1 and self.prev_target_pos then
		dpos:lerp(self.prev_target_pos, 1 - self.displacement_smoothing_rate)
		drot:nlerp(self.prev_target_rot, self.displacement_smoothing_rate)
	end
	self.prev_target_pos = dpos:copy()
	self.prev_target_rot = drot:copy()
	-- Mix in the free rotation mode.
	local mix = math.max(math.abs(self.turn_state),math.abs(self.tilt_state))
	drot:nlerp(Quaternion(), 1 - math.min(1, 30 * mix))
	-- Calculate the eye transformation.
	local eye_pos = dpos:transform(turn, pos)
	local eye_rot = drot:concat(rot)
	return eye_pos,eye_rot
end

--- Updates the camera transformation.
-- @param self Camera.
-- @param secs Seconds since the last update.
ThirdPersonCamera.update = function(self, secs)
	if not self.object then return end
	self.timer = self.timer + secs
	while self.timer > self.tick do
		-- Update the rotation state.
		self.timer = self.timer - self.tick
		if self.rotation_mode then
			-- Update turning.
			self.turn_state = self.turn_state + self.turn_speed
			self.turn_state = MathUtils:radian_wrap(self.turn_state)
			self.turn_speed = 0
			-- Update tilting.
			self.tilt_state = self.tilt_state - self.tilt_speed
			self.tilt_state = MathUtils:radian_wrap(self.tilt_state)
			self.tilt_speed = 0
		else
			-- Reset mouse look.
			self.turn_state = self.turn_state * math.max(1 - 3 * self.tick, 0)
			self.tilt_state = self.tilt_state * math.max(1 - 3 * self.tick, 0)
		end
		-- Update quake.
		if self.__quake_vector then
			if self.__quake_force.length > 0.01 or self.__quake_vector.length > 0.01 then
				self.__quake_force:multiply(0.7)
				self.__quake_force:subtract(self.__quake_vector:copy())
				self.__quake_vector:add(self.__quake_force:copy())
			else
				self.__quake_force = nil
				self.__quake_vector = nil
			end
		end
	end
	-- Update the death camera.
	local ipol = self.__death_ipol
	local zoom = ipol * 10 + (1 - ipol) * self.__zoom
	if self.object.dead then
		self.__death_ipol = math.min(ipol + 2 * secs, 1)
	else
		self.__death_ipol = math.max(ipol - 2 * secs, 0)
	end
	-- Calculate the final transformation.
	local pos,rot = self:get_eye_transform()
	local dist = self:calculate_3rd_person_clipped_distance(pos, rot,
		zoom, nil, self:get_collision_group(), self:get_collision_mask())
	dist = math.max(dist - 1.5, 0)
	pos,rot = self:calculate_3rd_person_transform(pos, rot, dist)
	-- Mix in the camera quake.
	if self.__quake_vector then
		pos = pos + self.__quake_vector
	end
	self:set_position(pos)
	self:set_rotation(rot)
end

ThirdPersonCamera.zoom = function(self, rate)
	local z = self.__zoom + rate
	z = math.max(1.5, z)
	z = math.min(100, z)
	self.__zoom = z
end

return ThirdPersonCamera
