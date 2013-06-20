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
	return self
end

--- Updates the camera transformation.
-- @param self Camera.
-- @param secs Seconds since the last update.
LandscapeCamera.update = function(self, secs)
	if Main.landscape then
		Main.client.camera_manager:set_camera_mode("landscape")
		local camctr = Vector(505,100,500)
		local campos = Vector(525,170,510)
		local camrot = Quaternion{dir = camctr - campos, up = Vector(0,1,0)}
		self:set_target_position(campos)
		self:set_target_rotation(camrot)
		Camera.update(self, secs)
		self:warp()
	end
end

return LandscapeCamera
