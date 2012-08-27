--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.math.aabb
-- @alias Aabb

local Class = require("system/class")
local Vector = require("system/math/vector")

--- TODO:doc
-- @type Aabb
local Aabb = Class("Aabb")

--- Creates a new axis-aligned bounding box.
-- @param clss Aabb class.
-- @param args Arguments.<ul>
--   <li>point: Minimum point vector.</li>
--   <li>size: Size vector.</li></ul>
-- @return Aabb.
Aabb.new = function(clss, args)
	local self = Class.new(clss)
	self.point = args.point or Vector()
	self.size = args.size or Vector()
	return self
end

--- Returns the distance to the given point.
-- @param self Aabb.
-- @param point Vector.
-- @return Number.
Aabb.get_distance_to_point = function(self, point)
	return (point - self:get_nearest_point(point)).length
end

--- Returns the point inside the box closest to the given point.
-- @param self Aabb.
-- @param point Vector.
-- @return Vector.
Aabb.get_nearest_point = function(self, point)
	return Vector(
		math.min(math.max(point.x, self.point.x), self.point.x + self.size.x),
		math.min(math.max(point.y, self.point.y), self.point.y + self.size.y),
		math.min(math.max(point.z, self.point.z), self.point.z + self.size.z))
end

--- Return true if the AABB intersects with another AABB.
-- @param self Aabb.
-- @param aabb Aabb.
-- @return True if intersects.
Aabb.intersects = function(self, aabb)
	if self.point.x + self.size.x <= aabb.point.x then return end
	if self.point.y + self.size.y <= aabb.point.y then return end
	if self.point.z + self.size.z <= aabb.point.z then return end
	if self.point.x >= aabb.point.x + aabb.size.x then return end
	if self.point.y >= aabb.point.y + aabb.size.y then return end
	if self.point.z >= aabb.point.z + aabb.size.z then return end
	return true
end

--- Return true if the AABB intersects with a point.
-- @param self Aabb.
-- @param point Position vector.
-- @return True if intersects.
Aabb.intersects_point = function(self, point)
	if point.x < self.point.x then return end
	if point.y < self.point.y then return end
	if point.z < self.point.z then return end
	if point.x > self.point.x + self.size.x then return end
	if point.y > self.point.y + self.size.x then return end
	if point.z > self.point.z + self.size.x then return end
	return true
end

return Aabb


