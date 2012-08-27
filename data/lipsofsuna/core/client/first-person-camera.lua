--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.client.first_person_camera
-- @alias FirstPersonCamera

local Camera = require("system/camera")
local Class = require("system/class")
local Physics = require("system/physics")

--- TODO:doc
-- @type FirstPersonCamera
local FirstPersonCamera = Class("FirstPersonCamera", Camera)

local radian_wrap = function(x)
	if x < -math.pi then return x + 2 * math.pi
	elseif x > math.pi then return x - 2 * math.pi
	else return x end
end

--- Creates a new first person camera.
-- @param clss First person camera class.
-- @param args Arguments.
-- @return First person camera.
FirstPersonCamera.new = function(clss, args)
	local self = Camera.new(clss, args)
	self:set_collision_mask(Physics.MASK_CAMERA)
	self:set_mode("first-person")
	self.tilt_speed = 0
	self.tilt_state = 0
	self.turn_speed = 0
	self.turn_state = 0
	return self
end

FirstPersonCamera.get_picking_ray = function(self)
	if not self.object then return end
	local spec = self.object.spec
	-- Calculate the rotation.
	local euler = self.object:get_rotation().euler
	euler[3] = self.object:get_tilt_angle() - self.tilt_state
	local rot = Quaternion{euler = euler}
	-- Project the ray.
	local ctr = self.object:transform_local_to_global(spec.aim_ray_center)
	return ctr,ctr + rot * Vector(0, 0, -5)
end

FirstPersonCamera.get_transform = function(self)
	-- Calculate the rotation.
	local turn = self.turn_state + self.object:get_turn_angle()
	local tilt = self.tilt_state + self.object:get_tilt_angle()
	local rot = Quaternion{euler = {turn, 0, tilt}}
	-- Find the camera offset.
	local spec = self.object.spec
	local rel = spec.camera_center or Vector(0, 2, 0)
	local pos = self.object:transform_local_to_global(rel)
	local npos,nrot = self.object:find_node{name = "#camera"}
	if npos then
		-- The position of the camera node is always used but the rotation is
		-- ignored most of the time since the rotation component of the node
		-- is highly annoying in many animations. However, the rotation is used
		-- when the player dies to avoid the camera facing inside the corpse.
		pos = self.object:transform_local_to_global(npos)
		if self.object.dead then
			rot = rot * nrot
		end
	end
	return pos,rot
end

FirstPersonCamera.update = function(self, secs)
	if Operators.camera:get_rotation_mode() then
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
	self:set_target_position(pos)
	self:set_target_rotation(rot)
	-- Interpolate.
	Camera.update(self, secs)
end

return FirstPersonCamera


