local Aabb = require("system/math/aabb")
local Object = require("system/object")
local Quaternion = require("system/math/quaternion")
local Vector = require("system/math/vector")

if not Los.program_load_extension("object-physics") then
	error("loading extension `object-physics' failed")
end

------------------------------------------------------------------------------

--- Makes the object approach a point.
-- @param self Object.
-- @param args Arguments.
--   <li>dist: Distance how close to target to get.</li>
--   <li>point: Point vector in world space. (required)</li>
--   <li>speed: Movement speed multiplier.</li></ul>
Object.approach = function(self, args)
	Los.object_approach(self.handle, {dist = args.dist, point = args.point.handle, speed = args.speec})
end

--- Lets an impulse force affect the object.
-- @param self Object.
-- @param ... Arguments.<ul>
--   <li>1,impulse: Force of impulse.</li>
--   <li>2,point: Point of impulse or nil.</li></ul>
Object.impulse = function(self, ...)
	local a,b = ...
	if a.class then
		Los.object_impulse(self.handle, a.handle, b and b.handle)
	else
		Los.object_impulse(self.handle, a.impulse.handle, a.point and a.point.handle)
	end
end

--- Creates a hinge constraint.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>position: Position vector.</li>
--   <li>axis: Axis of rotation.</li></ul>
Object.insert_hinge_constraint = function(self, args)
	Los.object_insert_hinge_constraint(self.handle, args.position.handle, args.axis and args.axis.handle)
end

--- Causes the object to jump.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>1,impulse: Force of impulse.</li></ul>
Object.jump = function(self, args)
	if args.class then
		Los.object_jump(self.handle, args.handle)
	else
		Los.object_jump(self.handle, args.impulse.handle)
	end
end

--- Gets the activation state of the object.
-- @param self Object.
-- @return Boolean.
Object.get_activated = function(self)
	return Los.object_get_activated(self.handle)
end

--- Sets the activation state of the object.
-- @param self Object.
-- @param v Boolean.
Object.set_activated = function(self, v)
	return Los.object_set_activated(self.handle, v)
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
-- @param self Object.
-- @return Vector.
Object.get_angular = function(self)
	return Vector:new_from_handle(Los.object_get_angular(self.handle))
end

--- Sets the angular velocity of the object.<br/>
--
-- Only supported by rigid bodies.
--
-- @param self Object.
-- @param v Vector.
Object.set_angular = function(self, v)
	Los.object_set_angular(self.handle, v.handle)
end

--- Gets the local bounding box of the collision shape of the object.
-- @oaram self Object.
-- @return Aabb.
Object.get_bounding_box_physics = function(self)
	local h1,h2 = Los.object_get_bounding_box_physics(self.handle)
	local min = Vector:new_from_handle(h1)
	local max = Vector:new_from_handle(h2)
	return Aabb{point = min, size = max - min}
end

--- Gets the local center offset of the collision shape of the object.
-- @param self Object.
-- @return Vector
Object.get_center_offset_physics = function(self)
	local h = Los.object_get_center_offset_physics(self.handle)
	return Vector:new_from_handle(h)
end

--- Gets the collision group number of the object.
-- @param self Object.
-- @return Number.
Object.get_collision_group = function(self)
	return Los.object_get_collision_group(self.handle)
end

--- Sets the collision group number of the object.
-- @param self Object.
-- @param v Number.
Object.set_collision_group = function(self, v)
	Los.object_set_collision_group(self.handle, v)
end

--- Gets the collision bitmask of the object.
-- @param self Object.
-- @return Number.
Object.get_collision_mask = function(self)
	return Los.object_get_collision_mask(self.handle)
end

--- Sets the collision bitmask of the object.
-- @param self Object.
-- @return Number.
Object.set_collision_mask = function(self, v)
	Los.object_set_collision_mask(self.handle, v)
end

--- Gets the contact event generation enable status.
-- @oaram self Object.
-- @return Boolean.
Object.get_contact_events = function(self)
	return Los.object_get_contact_events(self.handle)
end

--- Sets the contact event generation enable status.
-- @oaram self Object.
-- @param v Boolean.
Object.set_contact_events = function(self, v)
	Los.object_set_contact_events(self.handle, v)
end

--- Gets the liquid friction coefficient of the object.
-- @param self Object.
-- @return Number.
Object.get_friction_liquid = function(self)
	Los.object_get_friction_liquid(self.handle)
end

--- Sets the liquid friction coefficient of the object.
-- @param self Object.
-- @param v Number.
Object.set_friction_liquid = function(self, v)
	Los.object_set_friction_liquid(self.handle, v)
end

--- Gets the gravity vector of the object.
-- @param self Object.
-- @return Vector.
Object.get_gravity = function(self)
	return Vector:new_from_handle(Los.object_get_gravity(self.handle))
end

--- Sets the gravity vector of the object.
-- @param self Object.
-- @param v Vector.
Object.set_gravity = function(self, v)
	if not v then return end -- FIXME: Shouldn't be called with nil
	Los.object_set_gravity(self.handle, v.handle)
end

--- Gets the liquid gravity vector of the object.
-- @param self Object.
-- @return Vector.
Object.get_gravity_liquid = function(self)
	return Vector:new_from_handle(Los.object_get_gravity_liquid(self.handle))
end

--- Sets the liquid gravity vector of the object.
-- @param self Object.
-- @param v Vector.
Object.set_gravity_liquid = function(self, v)
	if not v then return end -- FIXME: Shouldn't be called with nil
	Los.object_set_gravity_liquid(self.handle, v.handle)
end

--- Gets the ground contact status of the object.<br/>
--
-- Only supported by kinematic objects. Other kind of objects always
-- return false.
--
-- @param self Object.
-- @return Boolean.
Object.get_ground = function(self)
	return Los.object_get_ground(self.handle)
end

--- Gets the ground contact status of the object.<br/>
--
-- Only supported by kinematic objects.
--
-- @param self Object.
-- @param v Boolean.
Object.set_ground = function(self, v)
	Los.object_set_ground(self.handle, v)
end

--- Gets the mass of the object.
-- @param self Object.
-- @return Number.
Object.get_mass = function(self)
	return Los.object_get_mass(self.handle)
end

--- Sets the mass of the object.
-- @param self Object.
-- @param v Number.
Object.set_mass = function(self, v)
	Los.object_set_mass(self.handle, v)
end

--- Gets the movement direction of the object.
-- @param self Object.
-- @return Number.
Object.get_movement = function(self)
	return Los.object_get_movement(self.handle)
end

--- Sets the movement direction of the object.<br/>
--
-- Only used by kinematic objects. The value of -1 means that the actor is
-- moving forward at walking speed. The value of 1 means backward, and the
-- value of 0 means no strafing.
--
-- @param self Object.
-- @param v Number.
Object.set_movement = function(self, v)
	Los.object_set_movement(self.handle, v)
end

--- Gets the physics simulation mode of the object.
-- @param self Object.
-- @return String.
Object.get_physics = function(self)
	return Los.object_get_physics(self.handle)
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
-- @param self Object.
-- @param v String.
Object.set_physics = function(self, v)
	Los.object_set_physics(self.handle, v)
end

--- Gets the physics shape name of the object.
-- @param self Object.
-- @return String.
Object.get_shape = function(self)
	return Los.object_get_shape(self.handle)
end

--- Sets the physics shape name of the object.<br/>
--
-- A model can contain multiple physics shapes. By setting the field,
-- you can switch to one of the alternative shapes. This can be used
-- to, for example, set a smaller collision shape when the actor
-- is crouching.
--
-- @param self Object.
-- @param v String.
Object.set_shape = function(self, v)
	Los.object_set_shape(self.handle, v)
end

--- Gets the movement speed of the object.
-- @param self Object.
-- @return Number.
Object.get_speed = function(self)
	return Los.object_get_speed(self.handle)
end

--- Sets the movement speed of the object.<br/>
--
-- Only used by kinematic objects.
--
-- @param self Object.
-- @param v Number.
Object.set_speed = function(self, v)
	Los.object_set_speed(self.handle, v)
end

--- Gets the strafing direction of the object.
-- @param self Object.
-- @return Number.
Object.get_strafing = function(self)
	return Los.object_get_strafing(self.handle)
end

--- Sets the strafing direction of the object.<br/>
--
-- Only used by kinematic objects. The value of -1 means that the actor is
-- strafing to the left at walking speed. The value of 1 means right, and the
-- value of 0 means no strafing.
--
-- @param self Object.
-- @param v Number.
Object.set_strafing = function(self, v)
	Los.object_set_strafing(self.handle, v)
end

--- Gets the linear velocity of the object.
-- @param self Object.
-- @return Vector.
Object.get_velocity = function(self)
	return Vector:new_from_handle(Los.object_get_velocity(self.handle))
end

--- Sets the linear velocity of the object.
-- @param self Object.
-- @param v Vector.
Object.set_velocity = function(self, v)
	Los.object_set_velocity(self.handle, v.handle)
end

return Object
