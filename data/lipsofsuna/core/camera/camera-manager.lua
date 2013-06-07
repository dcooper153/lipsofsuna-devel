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
local FirstPersonCamera = require("core/camera/first-person-camera")
local Program = require("system/core")
local Simulation = require("core/client/simulation")
local ThirdPersonCamera = require("core/camera/third-person-camera")

--- In-game camera management.
-- @type CameraManager
local CameraManager = Class("CameraManager")

--- Creates a new camera manager.
-- @param clss CameraManager class.
-- @return CameraManager.
CameraManager.new = function(clss, options)
	local self = Class.new(clss)
	self.camera1 = FirstPersonCamera()
	self.camera1:set_collision_mask(Game.PHYSICS_MASK_CAMERA)
	self.camera1:set_far(options.view_distance)
	self.camera1:set_fov(1.1)
	self.camera1:set_near(0.1)
	self.camera3 = ThirdPersonCamera()
	self.camera3:set_collision_mask(Game.PHYSICS_MASK_CAMERA)
	self.camera3:set_far(options.view_distance)
	self.camera3:set_fov(1.1)
	self.camera3:set_near(0.1)
	self:reset()
	return self
end

--- Resets the cameras.
--
-- Context: The game must have been joined and the character created.
--
-- @param self CameraManager.
CameraManager.reset = function(self)
	self.data = {}
	self.camera1.rotation_mode = false
	self.camera3.rotation_mode = false
	self:set_camera_mode("third-person")
	self:set_mouse_smoothing(Client.options.mouse_smoothing)
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
		self.camera1.object = Client.player_object
		self.camera3.object = Client.player_object
		self.camera1:update(secs)
		self.camera3:update(secs)
		Client.lighting:update(secs)
	end
	-- Update targeting.
	if Client.player_object then
		local r1,r2 = self.camera1:get_picking_ray()
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
		self.camera1:set_viewport(viewport)
		self.camera3:set_viewport(viewport)
		Client.lighting:set_dungeon_mode(false)
		self.camera1:set_far(Client.options.view_distance)
		self.camera3:set_far(Client.options.view_distance)
	end
end

--- Zooms the camera.
-- @param self CameraManager.
-- @param amount Zooming amount.
CameraManager.zoom = function(self, amount)
	self.camera:zoom{rate = -amount}
end

--- Gets the mode of the camera.
--
-- Context: The game must have been joined and the character created.
--
-- @param self CameraManager.
-- @return String.
CameraManager.get_camera_mode = function(self)
	if self.camera == self.camera1 then
		return "first-person"
	else
		return "third-person"
	end
end

--- Sets the mode of the camera.
--
-- Context: The game must have been joined and the character created.
--
-- @param self CameraManager.
-- @param v Mode.
CameraManager.set_camera_mode = function(self, v)
	if v == "first-person" then
		self.camera = self.camera1
	else
		self.camera = self.camera3
	end
	self.camera:reset()
end

--- Toggles mouse smoothing.
--
-- Context: The game must have been joined and the character created.
--
-- @param self CameraManager.
-- @param v Boolean.
CameraManager.set_mouse_smoothing = function(self, v)
	local s = v and 0.7 or 1
	self.camera3:set_rotation_smoothing(s)
	self.camera3:set_position_smoothing(s)
end

--- Returns true if the camera is in the rotation mode.
--
-- Context: The game must have been joined and the character created.
--
-- @param self CameraManager.
-- @return True if being rotated.
CameraManager.get_rotation_mode = function(self)
	return self.data.rotating
end

--- Enables or disables the camera rotation mode.
--
-- Context: The game must have been joined and the character created.
--
-- @param self CameraManager.
-- @param value True to enable rotation.
CameraManager.set_rotation_mode = function(self, value)
	self.data.rotating = value
	self.camera1.rotation_mode = value
	self.camera3.rotation_mode = value
end

return CameraManager
