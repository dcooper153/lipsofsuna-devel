--- Camera for the landscape subgame.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module landscape.camera
-- @alias LandscapeCamera

local Camera = require("system/camera")
local Class = require("system/class")
local MathUtils = require("system/math/utils")
local Quaternion = require("system/math/quaternion")
local Vector = require("system/math/vector")

--- Camera for the landscape subgame.
-- @type LandscapeCamera
LandscapeCamera = Class("LandscapeCamera", Camera)

--- Creates a new landscape camera.
-- @param clss LandscapeCamera class.
-- @return LandscapeCamera.
LandscapeCamera.new = function(clss)
	local self = Camera.new(clss)
	self:set_far(1000)
	self:set_near(0.3)
	self:set_mode("first-person")
	self.position = Vector(525,170,510)
	self.rotation = Quaternion:new_from_dir(Vector(-20,-70,-10), Vector(0,1,0))
	self.turning = 0
	self.tilting = 0
	self.turn_speed = 0
	self.tilt_speed = 0
	return self
end

--- Flies forward or backward.
-- @param self Camera.
-- @param value Amount of motion.
LandscapeCamera.fly = function(self, value)
	self.movement = 50 * value
end

--- Flies sideward.
-- @param self Camera.
-- @param value Amount of motion.
LandscapeCamera.strafe = function(self, value)
	self.strafing = 50 * value
end

--- Updates the camera transformation.
-- @param self Camera.
-- @param secs Seconds since the last update.
LandscapeCamera.update = function(self, secs)
	if not Main.landscape then return end
	-- Update the position.
	local vel = Vector()
	if self.lifting then vel = vel + Vector(0, self.lifting, 0) end
	if self.movement then vel = vel + self.rotation * Vector(0, 0, self.movement) end
	if self.strafing then vel = vel + self.rotation * Vector(self.strafing, 0, 0) end
	self.position = self.position + vel * secs
	self:set_target_position(self.position)
	-- Update the rotation.
	if self.turn_speed then
		self.turning = MathUtils:radian_wrap(self.turning - 0.1 * self.turn_speed)
		self.turn_speed = 0
	end
	if self.tilt_speed then
		self.tilting = MathUtils:radian_wrap(self.tilting - 0.1 * self.tilt_speed)
		self.tilt_speed = 0
	end
	self.rotation = Quaternion:new_from_euler(self.turning, 0, self.tilting)
	self:set_target_rotation(self.rotation)
	-- Warp to the destination.
	Main.client.camera_manager:set_camera_mode("landscape")
	Camera.update(self, secs)
	self:warp()
end

return LandscapeCamera
