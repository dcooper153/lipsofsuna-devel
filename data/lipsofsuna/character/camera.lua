--- Camera for the character creator.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module character.camera
-- @alias ChargenCamera

local Camera = require("system/camera")
local Class = require("system/class")
local Client = require("core/client/client")
local Quaternion = require("system/math/quaternion")
local Vector = require("system/math/vector")

--- Camera for the character creator.
-- @type ChargenCamera
ChargenCamera = Class("ChargenCamera", Camera)

--- Creates a new character creation camera.
-- @param clss ChargenCamera class.
-- @return ChargenCamera.
ChargenCamera.new = function(clss)
	local self = Camera.new(clss)
	self:set_far(60)
	self:set_near(0.3)
	self:set_mode("first-person")
	return self
end

--- Updates the camera transformation.
-- @param self Camera.
-- @param secs Seconds since the last update.
ChargenCamera.update = function(self, secs)
	local pos = Client.chargen:get_camera_focus()
	if pos then
		self:set_target_position(pos)
		self:set_target_rotation(Quaternion{axis = Vector(0, 1, 0), angle = math.pi})
		Camera.update(self, secs)
	end
end

return ChargenCamera
