--- Provides access to the physics simulation.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.physics
-- @alias Physics

local Class = require("system/class")
local Vector = require("system/math/vector")

if not Los.program_load_extension("physics") then
	error("loading extension `physics' failed")
end

------------------------------------------------------------------------------

--- Provides access to the physics simulation.
-- @type Physics
local Physics = Class("Physics")

--- Performs a ray cast test.
-- @param self Physics class.
-- @param src Start point vector in world space.
-- @param dst End point vector in world space.
-- @param mask Collision mask.
-- @param ignore Table of physics objects to ignore. Nil to not ignore any.
-- @return Table with point, normal, object and tile. Nil if no collision occurred.
Physics.cast_ray = function(self, src, dst, mask, ignore)
	local ignore_
	if ignore then
		ignore_ = {}
		for k,v in pairs(ignore) do ignore_[k] = v.handle end
	end
	local r = Los.physics_cast_ray(src.handle, dst.handle, mask, ignore_)
	if not r then return end
	r.point = Vector:new_from_handle(r.point)
	r.normal = Vector:new_from_handle(r.normal)
	if r.tile then r.tile = Vector:new_from_handle(r.tile) end
	return r
end

--- Performs a sphere cast test.
-- @param self Physics class.
-- @param src Start point vector in world space.
-- @param dst End point vector in world space.
-- @param radius Radius in world units.
-- @param mask Collision mask.
-- @param ignore Table of physics objects to ignore. Nil to not ignore any.
-- @return Table with point, normal, object and tile. Nil if no collision occurred.
Physics.cast_sphere = function(self, src, dst, radius, mask, ignore)
	local ignore_
	if ignore then
		ignore_ = {}
		for k,v in pairs(ignore) do ignore_[k] = v.handle end
	end
	local r = Los.physics_cast_sphere(src.handle, dst.handle, radius, mask, ignore_)
	if not r then return end
	r.point = Vector:new_from_handle(r.point)
	r.normal = Vector:new_from_handle(r.normal)
	if r.tile then r.tile = Vector:new_from_handle(r.tile) end
	return r
end

--- Gets the physics simulation enable status.
-- @param self Physics class.
-- @return Boolean.
Physics.get_enable_simulation = function(self)
	return Los.physics_get_enable_simulation()
end

--- Sets the physics simulation enable status.
-- @param self Physics class.
-- @param v Boolean.
Physics.set_enable_simulation = function(self, v)
	Los.physics_set_enable_simulation(v)
end

return Physics
