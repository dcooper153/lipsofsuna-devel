--- In-game camera management.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.camera.camera_manager
-- @alias CameraManager

local Class = require("system/class")
local Client = require("core/client/client")
local Program = require("system/core")
local Simulation = require("core/client/simulation")

--- In-game camera management.
-- @type CameraManager
local CameraManager = Class("CameraManager")

--- Creates a new camera manager.
-- @param clss CameraManager class.
-- @return CameraManager.
CameraManager.new = function(clss)
	local self = Class.new(clss)
	self.cameras = {}
	self.data = {}
	return self
end

--- Resets the camera.
-- @param self CameraManager.
CameraManager.reset = function(self)
	self.data = {}
	for k,v in pairs(self.cameras) do
		v.rotation_mode = false -- FIXME: should be a function
	end
	self:set_camera_mode("third-person") -- FIXME
	self:set_mouse_smoothing(Client.options.mouse_smoothing)
end

--- Quakes the camera.
-- @param self CameraManager.
-- @param amount Quake amount.
CameraManager.quake = function(self, amount)
	for k,v in pairs(self.cameras) do
		v.quake = math.max(v.quake or 0, amount) -- FIXME: should be a function
	end
end

--- Registers a new camera type.
-- @param self CameraManager.
-- @param type Camera type.
-- @param camera Camera object.
CameraManager.register_camera = function(self, type, camera)
	self.cameras[type] = camera
end

--- Tilts the camera.
-- @param self CameraManager.
-- @param amount Tilting amount.
CameraManager.tilt = function(self, amount)
	self.camera.tilt_speed = self.camera.tilt_speed + amount
end

--- Turns the camera.
-- @param self CameraManager.
-- @param amount Turning amount.
CameraManager.turn = function(self, amount)
	self.camera.turn_speed = self.camera.turn_speed + amount
end

--- Updates the cameras.
-- @param self CameraManager.
-- @param secs Seconds since the last update.
CameraManager.update = function(self, secs)
	-- Update the camera.
	if Client.player_object then
		for k,v in pairs(self.cameras) do
			v.object = Client.player_object -- FIXME: should be a function
			v:update(secs)
		end
		Client.lighting:update(secs)
	end
	-- Update targeting.
	if Client.player_object then
		local r1,r2 = self.cameras["first-person"]:get_picking_ray() --FIXME
		if r1 then
			local p,o = Simulation:pick_scene_by_ray(r1, r2)
			Client.player_state:set_targeted_object(o)
			Client.player_state.crosshair = (p or r2) - (r2 - r1):normalize() * 0.1
		end
	else
		Client.player_state:set_targeted_object()
		Client.player_state.crosshair = nil
	end
	-- Update the viewport.
	if Client.player_object then
		--Program:set_multisamples(Client.options.multisamples)
		Program:set_camera_far(self.camera:get_far())
		Program:set_camera_near(self.camera:get_near())
		Program:set_camera_position(self.camera:get_position())
		Program:set_camera_rotation(self.camera:get_rotation())
		local mode = Program:get_video_mode()
		local viewport = {0, 0, mode[1], mode[2]}
		for k,v in pairs(self.cameras) do
			v:set_viewport(viewport)
		end
		Client.lighting:set_dungeon_mode(false)
		for k,v in pairs(self.cameras) do
			v:set_far(Client.options.view_distance)
		end
	end
end

--- Zooms the camera.
-- @param self CameraManager.
-- @param amount Zooming amount.
CameraManager.zoom = function(self, amount)
	if self.camera.zoom then
		self.camera:zoom{rate = -amount}
	end
end

--- Gets the mode of the camera.
-- @param self CameraManager.
-- @return String.
CameraManager.get_camera_mode = function(self)
	for k,v in pairs(self.cameras) do
		if v == self.camera then
			return k
		end
	end
end

--- Sets the mode of the camera.
-- @param self CameraManager.
-- @param mode Mode.
CameraManager.set_camera_mode = function(self, mode)
	local camera = self.cameras[mode]
	if camera then
		self.camera = camera
		self.camera:reset()
	end
end

--- Toggles mouse smoothing.
-- @param self CameraManager.
-- @param v Boolean.
CameraManager.set_mouse_smoothing = function(self, v)
	local s = v and 0.7 or 1
	for k,v in pairs(self.cameras) do
		if v.set_rotation_smoothing then
			v:set_rotation_smoothing(s)
		end
		if v.set_position_smoothing then
			v:set_position_smoothing(s)
		end
	end
end

--- Returns true if the camera is in the rotation mode.
-- @param self CameraManager.
-- @return True if being rotated.
CameraManager.get_rotation_mode = function(self)
	return self.data.rotating
end

--- Enables or disables the camera rotation mode.
-- @param self CameraManager.
-- @param value True to enable rotation.
CameraManager.set_rotation_mode = function(self, value)
	self.data.rotating = value
	for k,v in pairs(self.cameras) do
		v.rotation_mode = value -- FIXME: Should be a function
	end
end

return CameraManager
