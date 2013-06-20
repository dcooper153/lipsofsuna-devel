--- Camera for the building subgame.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module building.camera
-- @alias BuildingCamera

local Camera = require("system/camera")
local Class = require("system/class")
local Quaternion = require("system/math/quaternion")
local Vector = require("system/math/vector")

--- Camera for the building subgame.
-- @type BuildingCamera
BuildingCamera = Class("BuildingCamera", Camera)

--- Creates a new building camera.
-- @param clss BuildingCamera class.
-- @return BuildingCamera.
BuildingCamera.new = function(clss)
	local self = Camera.new(clss)
	self:set_far(1000)
	self:set_near(0.3)
	self:set_mode("first-person")
	return self
end

--- Updates the camera transformation.
-- @param self Camera.
-- @param secs Seconds since the last update.
BuildingCamera.update = function(self, secs)
	if Main.building then
		Main.client.camera_manager:set_camera_mode("building")
		local camctr = Vector(505,100,500)
		local campos = Vector(525,110,510)
		local camrot = Quaternion{dir = camctr - campos, up = Vector(0,1,0)}
		self:set_target_position(campos)
		self:set_target_rotation(camrot)
		Camera.update(self, secs)
		self:warp()
	end
end

return BuildingCamera
