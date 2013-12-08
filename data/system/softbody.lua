--- Softbody.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.softbody
-- @alias Softbody

local Class = require("system/class")

if not Los.program_load_extension("softbody") then
	error("loading extension `softbody' failed")
end

------------------------------------------------------------------------------

--- Softbody.
-- @type Softbody
local Softbody = Class("Softbody")

--- Creates a new softbody set.
-- @param clss Softbody class.
-- @param model Model.
-- @param params Array of numbers.
-- @return Softbody.
Softbody.new = function(clss, model, params)
	local self = Class.new(clss)
	self.handle = Los.softbody_new(model.handle, params[1], params[2], params[3], params[4], params[5], params[6], params[7], params[8])
	self.__collision_mask = 1
	self.__collision_group = 0xFFFF
	return self
end

--- Updates the softbody.
-- @param self Softbody.
-- @param secs Seconds since the last update.
Softbody.update = function(self, secs)
	Los.softbody_update(self.handle, secs)
end

--- Gets the collision group number of the softbody.
-- @param self Softbody.
-- @return Number.
Softbody.get_collision_group = function(self)
	return self.__collision_group
end

--- Sets the collision group number of the softbody.
-- @param self Softbody.
-- @param v Number.
Softbody.set_collision_group = function(self, v)
	Los.softbody_set_collision_group(self.handle, v)
end

--- Gets the collision bitmask of the softbody.
-- @param self Softbody.
-- @return Number.
Softbody.get_collision_mask = function(self)
	return self.__collision_mask
end

--- Sets the collision bitmask of the softbody.
-- @param self Softbody.
-- @param v Number.
Softbody.set_collision_mask = function(self, v)
	Los.softbody_set_collision_mask(self.handle, v)
end

--- Sets the position of the softbody.
-- @param self Softbody.
-- @param v Vector.
Softbody.set_position = function(self, v)
	Los.softbody_set_position(self.handle, v.x, v.y, v.z)
end

--- Sets the render queue of the softbody.
-- @param self Softbody.
-- @param name Queue name.
Softbody.set_render_queue = function(self, name)
	Los.softbody_set_render_queue(self.handle, name)
end

--- Sets the rotation of the softbody.
-- @param self Softbody.
-- @param v Quaternion.
Softbody.set_rotation = function(self, v)
	Los.softbody_set_rotation(self.handle, v.x, v.y, v.z, v.w)
end

--- Sets the visibility of the softbody.
-- @param self Softbody.
-- @param v True to make visible. False otherwise.
Softbody.set_visible = function(self, v)
	Los.softbody_set_visible(self.handle, v)
end

return Softbody
