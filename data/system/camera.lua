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

--- Calculates the 1st person camera transformation.
-- @param self Camera.
-- @return Vector and quaternion.
Camera.calculate_1st_person_transform = function(self)
	local a,b = Los.camera_calculate_1st_person_transform(self.handle)
	return Vector:new_from_handle(a), Quaternion:new_from_handle(b)
end

--- Clips the 3rd person camera distance based on collisions.
-- @param self Camera.
-- @param center_pos Center position vector.
-- @param center_rot Center rotation quaternion.
-- @param distance Distance to the center.
-- @param collision_group Collision group. Nil for default.
-- @param collision_mask Collision mask. Nil for default.
-- @return Number.
Camera.calculate_3rd_person_clipped_distance = function(self, center_pos, center_rot, distance, collision_mask, collision_group)
	return Los.camera_calculate_3rd_person_clipped_distance(
		self.handle, center_pos.handle, center_rot.handle, distance, collision_mask, collision_group)
end

--- Calculates the 3rd person camera transformation.
-- @param self Camera.
-- @param center_pos Center position vector.
-- @param center_rot Center rotation quaternion.
-- @param distance Distance to the center.
-- @return Vector and quaternion.
Camera.calculate_3rd_person_transform = function(self, center_pos, center_rot, distance)
	local a,b = Los.camera_calculate_3rd_person_transform(
		self.handle, center_pos.handle, center_rot.handle, distance)
	return Vector:new_from_handle(a), Quaternion:new_from_handle(b)
end

--- Calculates the camera transformation after smoothing.
-- @param self Camera.
-- @param target_pos Target position vector.
-- @param target_rot Target rotation quaternion.
-- @param smooth_pos Position smoothing factor. Nil for no smoothing.
-- @param smooth_rot Rotation smoothing factor. Nil for no smoothing.
-- @return Vector and quaternion.
Camera.calculate_smoothed_transform = function(self, target_pos, target_rot, smooth_pos, smooth_rot)
	local a,b = Los.camera_calculate_smoothed_transform(
		self.handle, target_pos.handle, target_rot.handle, smooth_pos, smooth_rot)
	return Vector:new_from_handle(a), Quaternion:new_from_handle(b)
end

--- Moves the camera forward or backward.
-- @param self Camera.
-- @param rate Movement rate.
Camera.move = function(self, rate)
	Los.camera_move(self.handle, rate)
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
-- @param rate Tilting rate.
Camera.tilt = function(self, rate)
	Los.camera_tilt(self.handle, rate)
end

--- Sets the turning rate of the camera.
-- @param self Camera.
-- @param rate Turning rate.
Camera.turn = function(self, rate)
	Los.camera_turn(self.handle, rate)
end

--- Warps the camera to the target point.
-- @param self Camera.
Camera.warp = function(self)
	Los.camera_warp(self.handle, args)
end

--- Animates the camera.
-- @param self Camera.
-- @param secs Tick length.
Camera.update = function(self, secs)
	Los.camera_update(self.handle, secs)
end

--- Adjusts the zoom of the camera.
-- @param self Camera.
-- @param rate Zooming rate.
Camera.zoom = function(self, rate)
	Los.camera_zoom(self.handle, rate)
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
	self.__far = v
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

--- Sets the current position of the camera.
-- @param self Camera.
-- @param value Vector.
Camera.set_position = function(self, value)
	Los.camera_set_position(self.handle, value.handle)
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

--- Sets the current rotation of the camera.
-- @param self Camera.
-- @param value Quaternion.
Camera.set_rotation = function(self, value)
	Los.camera_set_rotation(self.handle, value.handle)
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
