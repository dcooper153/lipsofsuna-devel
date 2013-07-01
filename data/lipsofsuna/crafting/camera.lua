--- Camera for the crafting subgame.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module crafting.camera
-- @alias CraftingCamera

local Class = require("system/class")
local ThirdPersonCamera = require("core/camera/third/third-person-camera")

--- Camera for the crafting subgame.
-- @type CraftingCamera
CraftingCamera = Class("CraftingCamera", ThirdPersonCamera)

--- Creates a new crafting camera.
-- @param clss CraftingCamera class.
-- @return CraftingCamera.
CraftingCamera.new = function(clss)
	local self = ThirdPersonCamera.new(clss)
	self:set_far(1000)
	self:set_near(0.3)
	self:set_mode("third-person")
	return self
end

--- Updates the camera transformation.
-- @param self Camera.
-- @param secs Seconds since the last update.
CraftingCamera.update = function(self, secs)
	if Main.crafting then
		Main.client.camera_manager:set_camera_mode("crafting")
		ThirdPersonCamera.update(self, secs)
	end
end

return CraftingCamera
