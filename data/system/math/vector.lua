--- Vector maths class.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.math.vector
-- @alias Vector

local Class = require("system/class")

if not Los.program_load_extension("math") then
	error("loading extension `math' failed")
end

------------------------------------------------------------------------------

--- Vector maths class.
-- @type Vector
local Vector = Class("Vector")

--- Length.
-- @name Vector.length
-- @class table

--- X value.
-- @name Vector.x
-- @class table

--- Y value.
-- @name Vector.y
-- @class table

--- Z value.
-- @name Vector.z
-- @class table

local vector_getters = {
	length = function(self) return Los.vector_get_length(self.handle) end,
	x = function(self) return Los.vector_get_x(self.handle) end,
	y = function(self) return Los.vector_get_y(self.handle) end,
	z = function(self) return Los.vector_get_z(self.handle) end}

local vector_setters = {
	length = function(self, v) Los.vector_set_length(self.handle, v) end,
	x = function(self, v) Los.vector_set_x(self.handle, v) end,
	y = function(self, v) Los.vector_set_y(self.handle, v) end,
	z = function(self, v) Los.vector_set_z(self.handle, v) end}

local vector_index = function(self, k)
	local func = vector_getters[k]
	if func then
		return func(self)
	else
		local res = rawget(self, k)
		if res ~= nil then return res end
		local clss = rawget(self, "class")
		return clss[k]
	end
end

local vector_newindex = function(self, k, v)
	local func = vector_setters[k]
	if func then
		func(self, v)
	else
		rawset(self, k, v)
	end
end

--- Calculates the sum of two vectors.
-- @param self Vector.
-- @param v Vector.
-- @return New vector.
local vector_add = function(self, v)
	local copy = self:copy()
	Los.vector_add(copy.handle, v.handle)
	return copy
end

--- Multiplies the vector by a scalar or quaternion.
-- @param self Vector.
-- @param value Scalar or quaternion.
-- @return New vector.
local vector_mul = function(self, value)
	if type(value) == "number" then
		local copy = self:copy()
		Los.vector_mul(copy.handle, value)
		return copy
	elseif value.class_name == "Quaternion" then
		local copy = self:copy()
		Los.quaternion_mul(value.handle, copy.handle)
		return copy
	end
end

--- Subtracts a vector from another.
-- @param self Vector.
-- @param v Vector.
-- @return New vector.
local vector_sub = function(self, v)
	local copy = self:copy()
	Los.vector_sub(copy.handle, v.handle)
	return copy
end

--- Converts the vector to a string.
-- @param self Vector.
-- @return String.
local vector_tostring = function(self)
	return Los.vector_tostring(self.handle)
end

--- Creates a new vector.
-- @param clss Vector class.
-- @param x X coordinate.
-- @param y Y coordinate.
-- @param z Z coordinate.
-- @return New vector.
Vector.new = function(clss, x, y, z)
	local self = Vector:new_from_handle(Los.vector_new())
	Los.vector_set_x(self.handle, x)
	Los.vector_set_y(self.handle, y)
	Los.vector_set_z(self.handle, z)
	return self
end

--- Creates a new vector from an internal handle.
-- @param clss Vector class.
-- @param h Handle.
-- @return New vector.
Vector.new_from_handle = function(clss, h)
	local self = Class.new(clss)
	self.handle = h
	self.__index = vector_index
	self.__newindex = vector_newindex
	self.__add = vector_add
	self.__mul = vector_mul
	self.__sub = vector_sub
	self.__tostring = vector_tostring
	return self
end

--- Calculates the absolute of the vector in-place.
-- @param self Vector.
-- @return New vector.
Vector.abs = function(self)
	self.x = math.abs(self.x)
	self.y = math.abs(self.y)
	self.z = math.abs(self.z)
	return self
end

--- Add a vector to another in-place.
-- @param self Vector.
-- @param v Vector.
-- @return Self.
Vector.add = function(self, v)
	Los.vector_add(self.handle, v.handle)
	return self
end

--- Add components to the vector in-place.
-- @param self Vector.
-- @param x X component.
-- @param y Y component.
-- @param z Z component.
-- @return Self.
Vector.add_xyz = function(self, x, y, z)
	if x then self.x = self.x + x end
	if y then self.y = self.y + y end
	if z then self.z = self.z + z end
	return self
end

--- Calculates the ceil of the vector in-place.
-- @param self Vector.
-- @return Self.
Vector.ceil = function(self)
	self.x = math.ceil(self.x)
	self.y = math.ceil(self.y)
	self.z = math.ceil(self.z)
	return self
end

--- Returns a copy of the vector.
-- @param self Vector.
-- @return New vector.
Vector.copy = function(self)
	return Vector(self.x, self.y, self.z)
end

--- Divides the vector by a scalar in-place.
-- @param self Vector.
-- @param value Scalar.
-- @return Self.
Vector.divide = function(self, value)
	if value ~= 0 then
		Los.vector_mul(self.handle, 1/value)
	end
	return self
end

--- Calculates the floor of the vector in-place.
-- @param self Vector.
-- @return Self.
Vector.floor = function(self)
	self.x = math.floor(self.x)
	self.y = math.floor(self.y)
	self.z = math.floor(self.z)
	return self
end

--- Calculates the cross product of two vectors in-place.
-- @param self Vector.
-- @param v Vector.
-- @return Self.
Vector.cross = function(self, v)
	Los.vector_cross(self.handle, v.handle)
	return self
end

--- Calculates the dot product of two vectors.
-- @param self Vector.
-- @param v Vector.
-- @return Scalar.
Vector.dot = function(self, v)
	return Los.vector_dot(self.handle, v.handle)
end

--- Interpolates two vectors linearly in-place.
-- @param self Vector.
-- @param v Vector.
-- @param f Scalar in range [0, 1], where 0 corresponds to the own value.
Vector.lerp = function(self, v, f)
	self.x = self.x * (1 - f) + v.x * f
	self.y = self.y * (1 - f) + v.y * f
	self.z = self.z * (1 - f) + v.z * f
end

--- Multiplies the vector by a scalar in-place.
-- @param self Vector.
-- @param value Scalar.
-- @return Self.
Vector.multiply = function(self, value)
	Los.vector_mul(self.handle, value)
	return self
end

--- Normalizes the vector in-place.
-- @param self Vector.
-- @return Self.
Vector.normalize = function(self)
	Los.vector_normalize(self.handle)
	return self
end

--- Calculates the vector rounded to the nearest integers in-place.
-- @param self Vector.
-- @return Self.
Vector.round = function(self)
	self.x = math.ceil(self.x + 0.5)
	self.y = math.ceil(self.y + 0.5)
	self.z = math.ceil(self.z + 0.5)
	return self
end

--- Set the components of the vector in-place.
-- @param self Vector.
-- @param x X component.
-- @param y Y component.
-- @param z Z component.
-- @return Self.
Vector.set_xyz = function(self, x, y, z)
	if x then self.x = x end
	if y then self.y = y end
	if z then self.z = z end
	return self
end

--- Subtracts a vector from another in-place.
-- @param self Vector.
-- @param v Vector.
-- @return Self.
Vector.subtract = function(self, v)
	Los.vector_sub(self.handle, v.handle)
	return self
end

--- Substracts components from the vector in-place.
-- @param self Vector.
-- @param x X component.
-- @param y Y component.
-- @param z Z component.
-- @return Self.
Vector.subtract_xyz = function(self, x, y, z)
	if x then self.x = self.x - x end
	if y then self.y = self.y - y end
	if z then self.z = self.z - z end
	return self
end

--- Multiplies the vector by a quaternion in-place.<br/>
--
-- If the second argument is given, the vector is transformed around
-- that point.
--
-- @param self Vector.
-- @param quat Quaternion.
-- @param vec Vector, or nil.
-- @return Self.
Vector.transform = function(self, quat, vec)
	Los.vector_transform(self.handle, quat.handle, vec and vec.handle)
	return self
end

return Vector
