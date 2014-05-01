--- First person camera.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.camera.first_person_camera.first_person_camera
-- @alias FirstPersonCamera

local Camera = require("system/camera")
local Class = require("system/class")
local MathUtils = require("system/math/utils")
local Physics = require("system/physics")
local PhysicsConsts = require("core/server/physics-consts")

--- First person camera.
-- @type FirstPersonCamera
local FirstPersonCamera = Class("FirstPersonCamera", Camera)

--- Creates a new first person camera.
-- @param clss First person camera class.
-- @param args Arguments.
-- @return First person camera.
FirstPersonCamera.new = function(clss, args)
	local self = Camera.new(clss, args)
	self:set_collision_mask(PhysicsConsts.MASK_CAMERA)
	self:set_mode("first-person")
	self.camera_node_name = "#camera"
	self.camera_node_offset = nil
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
	return ctr,Vector(0, 0, -5):transform(rot, ctr)
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
	local npos,nrot = self.object:find_node_by_name(self.camera_node_name)
	if npos and self.camera_node_offset then
		npos:add(self.camera_node_offset)
	end
	if npos then
		pos = self.object:transform_local_to_global(npos)
		rot = nrot * rot
	end
	return pos,rot
end

FirstPersonCamera.update = function(self, secs)
	if not self.object then return end
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
