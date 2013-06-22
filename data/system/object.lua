--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.object
-- @alias Object

local Aabb = require("system/math/aabb")
local Class = require("system/class")
local Program = require("system/core")
local Quaternion = require("system/math/quaternion")
local Vector = require("system/math/vector")

if not Los.program_load_extension("object") then
	error("loading extension `object' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type Object
local Object = Class("Object")

--- Creates a new object.
-- @param clss Object class.
-- @return New object.
Object.new = function(clss)
	-- Create the object.
	local self = Class.new(clss)
	self.handle = Los.object_new()
	__userdata_lookup[self.handle] = self
	return self
end

--- Recalculates the bounding box of the model of the object.
-- @param self Object.
Object.calculate_bounds = function(self)
	Los.object_calculate_bounds(self.handle)
end

--- Detaches the object from the scene.
-- @param self Object.
Object.detach = function(self)
	self:set_visible(false)
end

--- Transforms a position vector from the local coordinate space to global.
-- @param self Object.
-- @param point Point vector.
-- @return Point vector.
Object.transform_local_to_global = function(self, point)
	local pos = self:get_position()
	if not point then return pos end
	local rot = self:get_rotation()
	return point:copy():transform(rot, pos)
end

--- Gets the local bounding box of the object.
-- @param self Object.
-- @return Aabb.
Object.get_bounding_box = function(self)
	local m = rawget(self, "__model")
	if not m then return Aabb{point = Vector(-0.1,-0.1,-0.1), size = Vector(0.2,0.2,0.2)} end
	return m:get_bounding_box()
end

--- Gets the local center offset of the bounding box of the object.
-- @param self Object.
-- @return Vector
Object.get_center_offset = function(self)
	local m = rawget(self, "__model")
	if not m then return Vector() end
	return m:get_center_offset()
end

--- Gets the ID of the object.
-- @param self Object.
-- @return Number, or nil
Object.get_id = function(self)
	return Los.object_get_id(self.handle)
end

--- Sets the ID of the object.
-- @param self Object.
-- @param value Number
Object.set_id = function(self, value)
	Los.object_set_id(self.handle, value)
end

--- Gets the model of the object.
-- @param self Object.
-- @return Model.
Object.get_model = function(self)
	return rawget(self, "__model")
end

--- Sets the model of the object.
-- @param self Object.
-- @param value Model.
Object.set_model = function(self, value)
	self.__model = value
	self.__particle = nil
	Los.object_set_model(self.handle, value and value.handle)
end

--- Gets the position of the object.
-- @param self Object.
-- @return Vector.
Object.get_position = function(self)
	local v = rawget(self, "__position")
	if not v then
		v = Vector()
		rawset(self, "__position", v)
	end
	Los.object_get_position(self.handle, v.handle)
	return v
end

--- Sets the position of the object.
-- @param self Object.
-- @param v Vector.
Object.set_position = function(self, v)
	if not v then return end
	Los.object_set_position(self.handle, v.handle)
end

--- Gets the rotation of the object.
-- @param self Object.
-- @return Quaternion.
Object.get_rotation = function(self)
	local v = rawget(self, "__rotation")
	if not v then
		v = Quaternion()
		rawset(self, "__rotation", v)
	end
	Los.object_get_rotation(self.handle, v.handle)
	return v
end

--- Sets the rotation of the object.
-- @param self Object.
-- @param value Quaternion.
Object.set_rotation = function(self, value)
	if not value then return end
	Los.object_set_rotation(self.handle, value.handle)
end

--- Gets the sector ID of the object.
-- @param self Object.
-- @return Number, or nil
Object.get_sector = function(self)
	return Los.object_get_sector(self.handle)
end

--- Returns true if the object is static.
-- @param self Object.
-- @return True if static, false if not.
Object.get_static = function(self)
	return Los.object_get_static(self.handle)
end

--- Make object static or non-static.<br/>
--
-- Static objects are not affected by the regular sector loading and unloading
-- scheme. They will persist and retain their rendering and physics status even
-- after the sector has been unloaded. On the contrary, non-static objects
-- cannot remain visible if the sector in which they are is hidden.
--
-- @param self Object.
-- @param value True for static, false for non-static.
Object.set_static = function(self, value)
	Los.object_set_static(self.handle, value)
end

--- Gets the visiblity of the object.
-- @param self Object.
-- @return True if visible, false if not.
Object.get_visible = function(self)
	return __objects_realized[self]
	--return Los.object_get_visible(self.handle)
end

--- Sets the visiblity of the object.
-- @param self Object.
-- @param v True for visible, false for not.
Object.set_visible = function(self, v)
	if v then
		if __objects_realized[self] then return end
		__objects_realized[self] = true
		Los.object_set_realized(self.handle, true)
	else
		if not __objects_realized[self] then return end
		__objects_realized[self] = nil
		Los.object_set_realized(self.handle, false)
	end
end

__objects_realized = {}

return Object


