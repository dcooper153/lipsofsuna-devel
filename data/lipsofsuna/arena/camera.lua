--- Camera for the arena subgame.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module arena.camera
-- @alias ArenaCamera

local Class = require("system/class")
local ThirdPersonCamera = require("core/camera/third/third-person-camera")

--- Camera for the arena subgame.
-- @type ArenaCamera
ArenaCamera = Class("ArenaCamera", ThirdPersonCamera)

--- Creates a new arena camera.
-- @param clss ArenaCamera class.
-- @return ArenaCamera.
ArenaCamera.new = function(clss)
	local self = ThirdPersonCamera.new(clss)
	self:set_far(1000)
	self:set_near(0.3)
	self:set_mode("third-person")
	return self
end

--- Updates the camera transformation.
-- @param self Camera.
-- @param secs Seconds since the last update.
ArenaCamera.update = function(self, secs)
	if Main.arena then
		Main.client.camera_manager:set_camera_mode("arena")
		ThirdPersonCamera.update(self, secs)
	end
end

return ArenaCamera
