--- Camera for the building subgame.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module building.camera
-- @alias BuildingCamera

local Class = require("system/class")
local ThirdPersonCamera = require("core/camera/third/third-person-camera")

--- Camera for the building subgame.
-- @type BuildingCamera
local BuildingCamera = Class("BuildingCamera", ThirdPersonCamera)

--- Creates a new building camera.
-- @param clss BuildingCamera class.
-- @return BuildingCamera.
BuildingCamera.new = function(clss)
	local self = ThirdPersonCamera.new(clss)
	self:set_far(1000)
	self:set_near(0.3)
	self:set_mode("third-person")
	return self
end

--- Updates the camera transformation.
-- @param self Camera.
-- @param secs Seconds since the last update.
BuildingCamera.update = function(self, secs)
	if Main.building then
		Main.client.camera_manager:set_camera_mode("building")
		ThirdPersonCamera.update(self, secs)
	end
end

return BuildingCamera
