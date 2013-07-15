--- Camera for the character creator.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module character.camera
-- @alias ChargenCamera

local Class = require("system/class")
local Client = require("core/client/client")
local Quaternion = require("system/math/quaternion")
local ThirdPersonCamera = require("core/camera/third/third-person-camera")

--- Camera for the character creator.
-- @type ChargenCamera
local ChargenCamera = Class("ChargenCamera", ThirdPersonCamera)

--- Creates a new character creation camera.
-- @param clss ChargenCamera class.
-- @return ChargenCamera.
ChargenCamera.new = function(clss)
	local self = ThirdPersonCamera.new(clss)
	self:set_far(60)
	self:set_near(0.3)
	self:set_mode("third-person")
	self:set_collision_group(0)
	self:set_collision_mask(0)
	self.displacement = Vector(-0.5,0,0)
	self.turn_state = math.pi
	return self
end

--- Updates the camera transformation.
-- @param self Camera.
-- @param secs Seconds since the last update.
ChargenCamera.update = function(self, secs)
	if Client.chargen.data.active then
		self.rotation_mode = true
		self:set_position_smoothing(1)
		self:set_rotation_smoothing(1)
		ThirdPersonCamera.update(self, secs)
	end
end

return ChargenCamera
