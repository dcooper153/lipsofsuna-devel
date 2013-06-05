--- Generic camera interface.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.camera
-- @alias Camera

local Class = require("system/class")

if not Los.program_load_extension("camera") then
	error("loading extension `camera' failed")
end

------------------------------------------------------------------------------

--- Generic camera interface.
-- @type Camera
local Camera = Class("Camera")

--- Creates a new camera.
-- @param clss Camera class.
-- @param args Arguments.
Camera.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.camera_new()
	if args then
		for k,v in pairs(args) do self[k] = v end
	end
	return self
end

--- Moves the camera forward or backward.
-- @param self Camera.
-- @param args Arguments.<ul>
--   <li>rate: Movement rate.</li>
--   <li>keep: True if should keep moving.</li></ul>
Camera.move = function(self, args)
	Los.camera_move(self.handle, args)
end

--- Creates a picking ray for the current camera configuration.
-- @param self Camera.
-- @param args Arguments.<ul>
--   <li>cursor: Cursor position, in pixels.</li>
--   <li>far: Ray end distance, in world coordinate units.</li>
--   <li>near: Ray start distance, in world coordinate units.</li></ul>
-- @return Start point, end point.
Camera.picking_ray = function(self, args)
	local a,b = Los.camera_picking_ray(self.handle, {cursor = args.cursor.handle, far = args.far, near = args.near})
	return Vector:new_from_handle(a), Vector:new_from_handle(b)
end

--- Resets the look spring transformation of the camera.
-- @param self Camera.
Camera.reset = function(self)
	Los.camera_reset(self.handle)
end

--- Sets the tilting rate of the camera.
-- @param self Camera.
-- @param args Arguments.<ul>
--   <li>rate: Tilting rate.</li>
--   <li>keep: True if should keep tilting.</li></ul>
Camera.tilt = function(self, args)
	Los.camera_tilt(self.handle, args)
end

--- Sets the turning rate of the camera.
-- @param self Camera.
-- @param args Arguments.<ul>
--   <li>rate: Turning rate.</li>
--   <li>keep: True if should keep turning.</li></ul>
Camera.turn = function(self, args)
	Los.camera_turn(self.handle, args)
end

--- Warps the camera to the target point.
-- @param self Camera.
Camera.warp = function(self)
	Los.camera_warp(self.handle, args)
end

--- Animates the camera.
--
-- @param self Camera.
-- @param args Arguments.<ul>
--   <li>1,secs: Tick length.</li></ul>
Camera.update = function(self, args)
	Los.camera_update(self.handle, args)
end

--- Adjusts the zoom of the camera.
-- @param self Camera.
-- @param args Arguments.<ul>
--   <li>rate: Zooming rate.</li>
--   <li>keep: True if should keep zooming.</li></ul>
Camera.zoom = function(self, args)
	Los.camera_zoom(self.handle, args)
end

--- Gets the collision group of the camera.
-- @param self Camera.
-- @return Collision group.
Camera.get_collision_group = function(self)
	return Los.camera_get_collision_group(self.handle)
end

--- Sets the collision group of the camera.
-- @param self Camera.
-- @param v Collision group.
Camera.set_collision_group = function(self, v)
	Los.camera_set_collision_group(self.handle, v)
end

--- Gets the collision mask of the camera.
-- @param self Camera.
-- @return Collision mask.
Camera.get_collision_mask = function(self)
	return Los.camera_get_collision_mask(self.handle)
end

--- Sets the collision mask of the camera.
-- @param self Camera.
-- @param v Collision mask.
Camera.set_collision_mask = function(self, v)
	Los.camera_set_collision_mask(self.handle, v)
end

--- Gets the far plane distance of the camera.
-- @param self Camera.
-- @return Number.
Camera.get_far = function(self)
	return self.__far or 75
end

--- Sets the far plane distance of the camera.
-- @param self Camera.
-- @param v Number.
Camera.set_far = function(self, v)
	rawset(self, "__far", v)
	Los.camera_set_far(self.handle, v)
end

--- Gets the field of view of the camera.
-- @param self Camera.
-- @return Angle in radians.
Camera.get_fov = function(self)
	return Los.camera_get_fov(self.handle)
end

--- Sets the field of view of the camera.
-- @param self Camera.
-- @param v Angle in radians.
Camera.set_fov = function(self, v)
	Los.camera_set_fov(self.handle, v)
end

--- Gets the current camera mode.<br/>
--
-- Recognized values are "first-person", "manual" and "third-person".
--
-- @param self Camera.
-- @return String.
Camera.get_mode = function(self)
	return Los.camera_get_mode(self.handle)
end

--- Sets the current camera mode.<br/>
--
-- Recognized values are "first-person", "manual" and "third-person".
--
-- @param self Camera.
-- @param v String.
Camera.set_mode = function(self, v)
	Los.camera_set_mode(self.handle, v)
end

--- Gets the modelview matrix of the camera.
-- @param self Camera.
-- @return List of 16 numbers.
Camera.get_modelview = function(self)
	return Los.camera_get_modelview(self.handle)
end

--- Gets the current position of the camera.
-- @param self Camera.
-- @return Vector.
Camera.get_position = function(self)
	return Vector:new_from_handle(Los.camera_get_position(self.handle))
end

--- Gets the position smoothing factor of the camera.
-- @param self Camera.
-- @return Number.
Camera.get_position_smoothing = function(self)
	return Los.camera_get_position_smoothing(self.handle)
end

--- Sets the position smoothing factor of the camera.
-- @param self Camera.
-- @param v Number.
Camera.set_position_smoothing = function(self, v)
	Los.camera_set_position_smoothing(self.handle, v)
end

--- Gets the projection matrix of the camera.
-- @param self Camera.
-- @return List of 16 numbers.
Camera.get_projection = function(self)
	return Los.camera_get_projection(self.handle)
end

--- Gets the near plane distance of the camera.
-- @param self Camera.
-- @return Number.
Camera.get_near = function(self)
	return self.__near or 1
end

--- Sets the near plane distance of the camera.
-- @param self Camera.
-- @param v Number.
Camera.set_near = function(self, v)
	self.__near = v
	Los.camera_set_near(self.handle, v)
end

--- Gets the current rotation of the camera.
-- @param self Camera.
-- @return Quaternion.
Camera.get_rotation = function(self)
	return Quaternion:new_from_handle(Los.camera_get_rotation(self.handle))
end

--- Gets the rotation smoothing factor of the camera.
-- @param self Camera.
-- @return Number.
Camera.get_rotation_smoothing = function(self)
	return Los.camera_get_rotation_smoothing(self.handle)
end

--- Sets the rotation smoothing factor of the camera.
-- @param self Camera.
-- @param v Number.
Camera.set_rotation_smoothing = function(self, v)
	Los.camera_set_rotation_smoothing(self.handle, v)
end

--- Gets the target position of the camera.
-- @param self Camera.
-- @return Vector.
Camera.get_target_position = function(self)
	return Vector:new_from_handle(Los.camera_get_target_position(self.handle))
end

--- Sets the target position of the camera.
-- @param self Camera.
-- @param v Vector.
Camera.set_target_position = function(self, v)
	Los.camera_set_target_position(self.handle, v.handle)
end

--- Gets the target rotation of the camera.
-- @param self Camera.
-- @return Quaternion.
Camera.get_target_rotation = function(self)
	return Quaternion:new_from_handle(Los.camera_get_target_rotation(self.handle))
end

--- Sets the target rotation of the camera.
-- @param self Camera.
-- @param v Quaternion.
Camera.set_target_rotation = function(self, v)
	Los.camera_set_target_rotation(self.handle, v.handle)
end

--- Gets the viewport of the camera.
-- @param self Camera.
-- @return Table of four numbers.
Camera.get_viewport = function(self)
	return Los.camera_get_viewport(self.handle)
end

--- Sets the viewport of the camera.
-- @param self Camera.
-- @param v Table of four numbers.
Camera.set_viewport = function(self, v)
	Los.camera_set_viewport(self.handle, v)
end

return Camera
