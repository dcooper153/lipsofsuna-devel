--- Quaternion maths class.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.math.quaternion
-- @alias Quaternion

local Class = require("system/class")
local Vector = require("system/math/vector")

if not Los.program_load_extension("math") then
	error("loading extension `math' failed")
end

------------------------------------------------------------------------------

--- Quaternion maths class.
-- @type Quaternion
local Quaternion = Class("Quaternion")

--- The conjugate of the quaternion.
-- @name Quaternion.conjugate
-- @class table

--- Euler angle presentation of the quaternion.
-- @name Quaternion.euler
-- @class table

--- Length.
-- @name Quaternion.length
-- @class table

--- W value.
-- @name Quaternion.w
-- @class table

--- X value.
-- @name Quaternion.x
-- @class table

--- Y value.
-- @name Quaternion.y
-- @class table

--- Z value.
-- @name Quaternion.z
-- @class table

local quaternion_getters = {
	conjugate = function(self) return Quaternion:new_from_handle(Los.quaternion_get_conjugate(self.handle)) end,
	euler = function(self) return Los.quaternion_get_euler(self.handle) end,
	length = function(self) return Los.quaternion_get_length(self.handle) end,
	w = function(self) return Los.quaternion_get_w(self.handle) end,
	x = function(self) return Los.quaternion_get_x(self.handle) end,
	y = function(self) return Los.quaternion_get_y(self.handle) end,
	z = function(self) return Los.quaternion_get_z(self.handle) end}

local quaternion_setters = {
	length = function(self, v) Los.vector_set_length(self.handle, v) end,
	w = function(self, v) Los.quaternion_set_w(self.handle, v) end,
	x = function(self, v) Los.quaternion_set_x(self.handle, v) end,
	y = function(self, v) Los.quaternion_set_y(self.handle, v) end,
	z = function(self, v) Los.quaternion_set_z(self.handle, v) end}

local quaternion_index = function(self, k)
	local func = quaternion_getters[k]
	if func then
		return func(self)
	else
		local res = rawget(self, k)
		if res ~= nil then return res end
		local clss = rawget(self, "class")
		return clss[k]
	end
end

local quaternion_newindex = function(self, k, v)
	local func = quaternion_setters[k]
	if func then
		func(self, v)
	else
		rawset(self, k, v)
	end
end

--- Calculates the sum of two quaternions.
-- @param self Quaternion.
-- @param quat Quaternion.
-- @return New quaternion.
local quaternion_add = function(self, quat)
	local handle = Los.quaternion_add(self.handle, quat.handle)
	return Quaternion:new_from_handle(handle)
end

--- Multiplies the quaternion with another value.<br/>
--
-- The second value can be a scalar, a vector, or another quaternion.
-- If it is a scalar, all the components of the quaternion are multiplied by it.
-- If it is a vector, the vector is rotated by the quaternion, and
-- if it is another quaternion, the rotations of the quaternions are concatenated.
--
-- @param self Quaternion.
-- @param value Quaternion, vector, or number.
-- @return New quaternion or vector.
local quaternion_mul = function(self, value)
	if type(value) == "number" then
		local quat = Quaternion:copy(self)
		Los.quaternion_multiply(quat.handle, value)
		return quat
	elseif value.class == Quaternion then
		local quat = Quaternion:copy(self)
		Los.quaternion_concat(quat.handle, value.handle)
		return quat
	else
		local vec = value:copy()
		Los.vector_transform(vec.handle, self.handle)
		return vec
	end
end

--- Subtracts a quaternion from another.
-- @param self Quaternion.
-- @param quat Quaternion.
-- @return New quaternion.
local quaternion_sub = function(self, quat)
	local handle = Los.quaternion_sub(self.handle, quat.handle)
	return Quaternion:new_from_handle(handle)
end

--- Converts the quaternion to a string.
-- @param self Quaternion.
-- @return String.
local quaternion_tostring = function(self)
	return Los.quaternion_tostring(self.handle)
end

--- Creates a new quaternion.
-- @param clss Quaternion class.
-- @param x Optional X value, default is 0.
-- @param y Optional Y value, default is 0.
-- @param z Optional Z value, default is 0.
-- @param w Optional W value, default is 1.
-- @return New quaternion.
Quaternion.new = function(clss, x, y, z, w)
	local h = Los.quaternion_new(x, y, z, w)
	return Quaternion:new_from_handle(h)
end

--- Creates a new quaternion from an axis and angle.
-- @param clss Quaternion class.
-- @param x Axis.
-- @param y Axis.
-- @param z Axis.
-- @param angle Angle.
-- @return New quaternion.
Quaternion.new_from_axis = function(clss, x, y, z, angle)
	local h = Los.quaternion_new_from_axis(x, y, z, angle)
	return Quaternion:new_from_handle(h)
end

--- Creates a new quaternion from direction and up vectors.
-- @param clss Quaternion class.
-- @param dx Direction vector.
-- @param dy Direction vector.
-- @param dz Direction vector.
-- @param ux Up vector.
-- @param uy Up vector.
-- @param uz Up vector.
-- @return New quaternion.
Quaternion.new_from_dir = function(clss, dx, dy, dz, ux, uy, uz)
	local h = Los.quaternion_new_from_dir(dx, dy, dz, ux, uy, uz)
	return Quaternion:new_from_handle(h)
end

--- Creates a new quaternion from euler angles.
-- @param clss Quaternion class.
-- @param x First angle.
-- @param y Second angle.
-- @param z Third angle.
-- @return New quaternion.
Quaternion.new_from_euler = function(clss, x, y, z)
	if type(x) == "table" then
		local h = Los.quaternion_new_from_euler(x[1], x[2], x[3])
		return Quaternion:new_from_handle(h)
	else
		local h = Los.quaternion_new_from_euler(x, y, z)
		return Quaternion:new_from_handle(h)
	end
end

--- Creates a new quaternion from an internal handle.
-- @param clss Quaternion class.
-- @param h Handle.
-- @return New quaternion.
Quaternion.new_from_handle = function(clss, h)
	local self = Class.new(clss)
	self.handle = h
	self.__index = quaternion_index
	self.__newindex = quaternion_newindex
	self.__add = quaternion_add
	self.__mul = quaternion_mul
	self.__sub = quaternion_sub
	self.__tostring = quaternion_tostring
	return self
end

--- Creates a quaternion that rotates from the source point to the destination.
-- @param clss Quaternion class.
-- @param sx Source vector.
-- @param sy Source vector.
-- @param sz Source vector.
-- @param dx Destination vector.
-- @param dy Destination vector.
-- @param dz Destination vector.
-- @return New quaternion.
Quaternion.new_from_vectors = function(clss, sx, sy, sz, dx, dy, sz)
	local h = Los.quaternion_new_from_vectors(sx, sy, sz, dx, dy, sz)
	return Quaternion:new_from_handle(h)
end

--- Returns a copy of the quaternion.
-- @param self Quaternion.
-- @return New quaternion.
Quaternion.copy = function(self)
	return Quaternion(self.x, self.y, self.z, self.w)
end

--- Concatenates a quaternion to another in-place.
-- @param self Quaternion.
-- @param quat Quaternion.
-- @return Self.
Quaternion.concat = function(self, quat)
	Los.quaternion_concat(self.handle, quat.handle)
	return self
end

--- Multiplies a quaternion by a scalar in-place.
-- @param self Quaternion.
-- @param s Scalar.
-- @return Self.
Quaternion.multiply = function(self, s)
	Los.quaternion_multiply(self.handle, s)
	return self
end

--- Normalized linear interpolation in-place.
-- @param self Quaternion.
-- @param quat Quaternion.
-- @param blend Interpolation factor.
-- @return Self.
Quaternion.nlerp = function(self, quat, blend)
	Los.quaternion_nlerp(self.handle, quat.handle, blend)
	return self
end

--- Calculates the normalized form of the quaternion.
-- @param self Quaternion.
-- @return New quaternion.
Quaternion.normalize = function(self)
	local handle = Los.quaternion_normalize(self.handle)
	return Quaternion:new_from_handle(handle)
end

--- Set the components of the quaternion in-place from another quaternion.
-- @param self Quaternion.
-- @param q Quaternion.
-- @return Self.
Quaternion.set = function(self, q)
	self.x = q.x
	self.y = q.y
	self.z = q.z
	self.w = q.w
	return self
end

--- Set the quaternion from axis and angle.
-- @param self Quaternion.
-- @param axis Axis vector.
-- @param angle Angle in radians.
-- @return Self.
Quaternion.set_axis = function(self, axis, angle)
	Los.quaternion_set_axis(self.handle, axis.handle, angle)
	return self
end

--- Set the quaternion from direction and up vectors.
-- @param self Quaternion.
-- @param dir Direction vector.
-- @param up Up vector.
-- @return Self.
Quaternion.set_dir = function(self, dir, up)
	Los.quaternion_set_dir(self.handle, dir.handle, up.handle)
	return self
end

--- Set the quaternion from euler angles.
-- @param self Quaternion.
-- @param x First angle.
-- @param y Second angle.
-- @param z Third angle.
-- @return Self.
Quaternion.set_euler = function(self, x, y, z)
	Los.quaternion_set_euler(self.handle, x, y, z)
	return self
end

--- Set the components of the quaternion in-place.
-- @param self Quaternion.
-- @param x X component.
-- @param y Y component.
-- @param z Z component.
-- @param w W component.
-- @return Self.
Quaternion.set_xyzw = function(self, x, y, z, w)
	if x then self.x = x end
	if y then self.y = y end
	if z then self.z = z end
	if w then self.w = w end
	return self
end

return Quaternion
