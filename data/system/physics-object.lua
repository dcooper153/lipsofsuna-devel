--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.phyics_object
-- @alias PhysicsObject

local Aabb = require("system/math/aabb")
local Class = require("system/class")
local Quaternion = require("system/math/quaternion")
local Vector = require("system/math/vector")

if not Los.program_load_extension("physics-object") then
	error("loading extension `physics-object' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type PhysicsObject
local PhysicsObject = Class("PhysicsObject")

--- Creates a new physics object.
-- @param clss PhysicsObject class.
-- @return PhysicsObject.
PhysicsObject.new = function(clss)
	local self = Class.new(clss)
	self.handle = Los.physics_object_new()
	self.__velocity = Vector()
	return self
end

--- Makes the object approach a point.
-- @param self PhysicsObject.
-- @param args Arguments.
--   <li>dist: Distance how close to target to get.</li>
--   <li>point: Point vector in world space. (required)</li>
--   <li>speed: Movement speed multiplier.</li></ul>
PhysicsObject.approach = function(self, args)
	Los.physics_object_approach(self.handle, {dist = args.dist, point = args.point.handle, speed = args.speec})
end

--- Lets an impulse force affect the object.
-- @param self PhysicsObject.
-- @param impulse Force of the impulse.
-- @param point Point of impulse, or nil.
PhysicsObject.impulse = function(self, impulse, point)
	Los.physics_object_impulse(self.handle, impulse.handle, point and point.handle)
end

--- Creates a hinge constraint.
-- @param self PhysicsObject.
-- @param args Arguments.<ul>
--   <li>position: Position vector.</li>
--   <li>axis: Axis of rotation.</li></ul>
PhysicsObject.insert_hinge_constraint = function(self, args)
	Los.physics_object_insert_hinge_constraint(self.handle, args.position.handle, args.axis and args.axis.handle)
end

--- Causes the object to jump.
-- @param self PhysicsObject.
-- @param impulse Force of the impulse.
PhysicsObject.jump = function(self, impulse)
	Los.physics_object_jump(self.handle, impulse.handle)
end

--- Gets the activation state of the object.
-- @param self PhysicsObject.
-- @return Boolean.
PhysicsObject.get_activated = function(self)
	return Los.physics_object_get_activated(self.handle)
end

--- Sets the activation state of the object.
-- @param self PhysicsObject.
-- @param v Boolean.
PhysicsObject.set_activated = function(self, v)
	return Los.physics_object_set_activated(self.handle, v)
end

--- Gets the angular velocity of the object.<br/>
--
-- Angular velocity specifies how the object rotates. The direction of the
-- vector points towards the axis of rotation and the length of the vector
-- specifies how fast the object rotates around its center point.<br/>
--
-- Only supported by rigid bodies. Other kind of objects always return
-- a zero vector.
--
-- @param self PhysicsObject.
-- @return Vector.
PhysicsObject.get_angular = function(self)
	return Vector:new_from_handle(Los.physics_object_get_angular(self.handle))
end

--- Sets the angular velocity of the object.<br/>
--
-- Only supported by rigid bodies.
--
-- @param self PhysicsObject.
-- @param v Vector.
PhysicsObject.set_angular = function(self, v)
	Los.physics_object_set_angular(self.handle, v.handle)
end

--- Gets the local bounding box of the collision shape of the object.
-- @param self PhysicsObject.
-- @return Aabb.
PhysicsObject.get_bounding_box = function(self)
	local h1,h2 = Los.physics_object_get_bounding_box(self.handle)
	local min = Vector:new_from_handle(h1)
	local max = Vector:new_from_handle(h2)
	return Aabb{point = min, size = max - min}
end

--- Gets the local center offset of the collision shape of the object.
-- @param self PhysicsObject.
-- @return Vector
PhysicsObject.get_center_offset = function(self)
	local h = Los.physics_object_get_center_offset(self.handle)
	return Vector:new_from_handle(h)
end

--- Gets the collision group number of the object.
-- @param self PhysicsObject.
-- @return Number.
PhysicsObject.get_collision_group = function(self)
	return Los.physics_object_get_collision_group(self.handle)
end

--- Sets the collision group number of the object.
-- @param self PhysicsObject.
-- @param v Number.
PhysicsObject.set_collision_group = function(self, v)
	Los.physics_object_set_collision_group(self.handle, v)
end

--- Gets the collision bitmask of the object.
-- @param self PhysicsObject.
-- @return Number.
PhysicsObject.get_collision_mask = function(self)
	return Los.physics_object_get_collision_mask(self.handle)
end

--- Sets the collision bitmask of the object.
-- @param self PhysicsObject.
-- @param v Number.
PhysicsObject.set_collision_mask = function(self, v)
	Los.physics_object_set_collision_mask(self.handle, v)
end

--- Gets the contact event generation enable status.
-- @param self PhysicsObject.
-- @return Boolean.
PhysicsObject.get_contact_events = function(self)
	return Los.physics_object_get_contact_events(self.handle)
end

--- Sets the contact event generation enable status.
-- @param self PhysicsObject.
-- @param v Boolean.
PhysicsObject.set_contact_events = function(self, v)
	Los.physics_object_set_contact_events(self.handle, v)
end

--- Gets the liquid friction coefficient of the object.
-- @param self PhysicsObject.
-- @return Number.
PhysicsObject.get_friction_liquid = function(self)
	Los.physics_object_get_friction_liquid(self.handle)
end

--- Sets the liquid friction coefficient of the object.
-- @param self PhysicsObject.
-- @param v Number.
PhysicsObject.set_friction_liquid = function(self, v)
	Los.physics_object_set_friction_liquid(self.handle, v)
end

--- Gets the gravity vector of the object.
-- @param self PhysicsObject.
-- @return Vector.
PhysicsObject.get_gravity = function(self)
	return Vector:new_from_handle(Los.physics_object_get_gravity(self.handle))
end

--- Sets the gravity vector of the object.
-- @param self PhysicsObject.
-- @param v Vector.
PhysicsObject.set_gravity = function(self, v)
	if not v then return end -- FIXME: Shouldn't be called with nil
	Los.physics_object_set_gravity(self.handle, v.handle)
end

--- Gets the liquid gravity vector of the object.
-- @param self PhysicsObject.
-- @return Vector.
PhysicsObject.get_gravity_liquid = function(self)
	return Vector:new_from_handle(Los.physics_object_get_gravity_liquid(self.handle))
end

--- Sets the liquid gravity vector of the object.
-- @param self PhysicsObject.
-- @param v Vector.
PhysicsObject.set_gravity_liquid = function(self, v)
	if not v then return end -- FIXME: Shouldn't be called with nil
	Los.physics_object_set_gravity_liquid(self.handle, v.handle)
end

--- Gets the ground contact status of the object.<br/>
--
-- Only supported by kinematic objects. Other kind of objects always
-- return false.
--
-- @param self PhysicsObject.
-- @return Boolean.
PhysicsObject.get_ground = function(self)
	return Los.physics_object_get_ground(self.handle)
end

--- Gets the ID of the object.
-- @param self PhysicsObject.
-- @return Number, or nil
PhysicsObject.get_id = function(self)
	return Los.physics_object_get_id(self.handle)
end

--- Sets the ID of the object.
-- @param self PhysicsObject.
-- @param value Number
PhysicsObject.set_id = function(self, value)
	Los.physics_object_set_id(self.handle, value)
end

--- Gets the model of the object.
-- @param self Object.
-- @return Model.
PhysicsObject.get_model = function(self)
	return self.__model
end

--- Sets the model of the object.
-- @param self PhysicsObject.
-- @param v Model.
PhysicsObject.set_model = function(self, v)
	self.__model = v
	Los.physics_object_set_model(self.handle, v and v.handle)
end

--- Gets the position of the object.
-- @param self PhysicsObject.
-- @return Vector.
PhysicsObject.get_position = function(self)
	local v = self.__position
	if not v then
		v = Vector()
		self.__position = v
	end
	Los.physics_object_get_position(self.handle, v.handle)
	return v
end

--- Sets the position of the object.
-- @param self PhysicsObject.
-- @param v Vector.
PhysicsObject.set_position = function(self, v)
	if not v then return end
	Los.physics_object_set_position(self.handle, v.handle)
end

--- Gets the rotation of the object.
-- @param self PhysicsObject.
-- @return Quaternion.
PhysicsObject.get_rotation = function(self)
	local v = self.__rotation
	if not v then
		v = Quaternion()
		self.__rotation = v
	end
	Los.physics_object_get_rotation(self.handle, v.handle)
	return v
end

--- Sets the rotation of the object.
-- @param self PhysicsObject.
-- @param v Quaternion.
PhysicsObject.set_rotation = function(self, v)
	if not v then return end
	Los.physics_object_set_rotation(self.handle, v.handle)
end

--- Gets the mass of the object.
-- @param self PhysicsObject.
-- @return Number.
PhysicsObject.get_mass = function(self)
	return Los.physics_object_get_mass(self.handle)
end

--- Sets the mass of the object.
-- @param self PhysicsObject.
-- @param v Number.
PhysicsObject.set_mass = function(self, v)
	Los.physics_object_set_mass(self.handle, v)
end

--- Gets the movement direction of the object.
-- @param self PhysicsObject.
-- @return Number.
PhysicsObject.get_movement = function(self)
	return Los.physics_object_get_movement(self.handle)
end

--- Sets the movement direction of the object.<br/>
--
-- Only used by kinematic objects. The value of -1 means that the actor is
-- moving forward at walking speed. The value of 1 means backward, and the
-- value of 0 means no strafing.
--
-- @param self PhysicsObject.
-- @param v Number.
PhysicsObject.set_movement = function(self, v)
	Los.physics_object_set_movement(self.handle, v)
end

--- Gets the physics simulation mode of the object.
-- @param self PhysicsObject.
-- @return String.
PhysicsObject.get_physics = function(self)
	return Los.physics_object_get_physics(self.handle)
end

--- Sets the physics simulation mode of the object.<br/>
--
-- Specifies the physics simulation mode of the object.
-- The recognized values are:
--
-- <ul>
-- <li>"kinematic": Kinematic character.</li>
-- <li>"none": Not included to the simulation.</li>
-- <li>"rigid": Rigid body simulation.</li>
-- <li>"static": Static obstacle.</li>
-- <li>"vehicle": Vehicle physics.</li>
-- </ul>
--
-- @param self PhysicsObject.
-- @param v String.
PhysicsObject.set_physics = function(self, v)
	Los.physics_object_set_physics(self.handle, v)
end

--- Gets the physics shape name of the object.
-- @param self PhysicsObject.
-- @return String.
PhysicsObject.get_shape = function(self)
	return Los.physics_object_get_shape(self.handle)
end

--- Sets the physics shape name of the object.<br/>
--
-- A model can contain multiple physics shapes. By setting the field,
-- you can switch to one of the alternative shapes. This can be used
-- to, for example, set a smaller collision shape when the actor
-- is crouching.
--
-- @param self PhysicsObject.
-- @param v String.
PhysicsObject.set_shape = function(self, v)
	Los.physics_object_set_shape(self.handle, v)
end

--- Gets the movement speed of the object.
-- @param self PhysicsObject.
-- @return Number.
PhysicsObject.get_speed = function(self)
	return Los.physics_object_get_speed(self.handle)
end

--- Sets the movement speed of the object.<br/>
--
-- Only used by kinematic objects.
--
-- @param self PhysicsObject.
-- @param v Number.
PhysicsObject.set_speed = function(self, v)
	Los.physics_object_set_speed(self.handle, v)
end

--- Gets the strafing direction of the object.
-- @param self PhysicsObject.
-- @return Number.
PhysicsObject.get_strafing = function(self)
	return Los.physics_object_get_strafing(self.handle)
end

--- Sets the strafing direction of the object.<br/>
--
-- Only used by kinematic objects. The value of -1 means that the actor is
-- strafing to the left at walking speed. The value of 1 means right, and the
-- value of 0 means no strafing.
--
-- @param self PhysicsObject.
-- @param v Number.
PhysicsObject.set_strafing = function(self, v)
	Los.physics_object_set_strafing(self.handle, v)
end

--- Gets the linear velocity of the object.
-- @param self PhysicsObject.
-- @return Vector.
PhysicsObject.get_velocity = function(self)
	Los.physics_object_get_velocity(self.handle, self.__velocity.handle)
	return self.__velocity
end

--- Sets the linear velocity of the object.
-- @param self PhysicsObject.
-- @param v Vector.
PhysicsObject.set_velocity = function(self, v)
	Los.physics_object_set_velocity(self.handle, v.handle)
end

--- Gets the visiblity of the object.
-- @param self PhysicsObject.
-- @return True if visible, false if not.
PhysicsObject.get_visible = function(self)
	return Los.physics_object_get_visible(self.handle)
end

--- Sets the visiblity of the object.
-- @param self PhysicsObject.
-- @param v True for visible, false for not.
PhysicsObject.set_visible = function(self, v)
	if v then
		Los.physics_object_set_visible(self.handle, true)
	else
		Los.physics_object_set_visible(self.handle, false)
	end
end

return PhysicsObject
