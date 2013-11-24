--- Renderable billboard.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.billboard
-- @alias Billboard

local Class = require("system/class")

if not Los.program_load_extension("billboard") then
	error("loading extension `billboard' failed")
end

------------------------------------------------------------------------------

--- Renderable billboard.
-- @type Billboard
local Billboard = Class("Billboard")

--- Creates a new billboard set.
-- @param clss Billboard class.
-- @return Billboard.
Billboard.new = function(clss, model)
	local self = Class.new(clss)
	self.handle = Los.billboard_new()
	return self
end

--- Adds a billboard to the billboard set.
-- @param self Billboard.
-- @param pos Position vector.
Billboard.add = function(self, pos)
	Los.billboard_add(self.handle, pos.x, pos.y, pos.z)
end

--- Clears the billboard set.
-- @param self Billboard.
Billboard.clear = function(self)
	Los.billboard_clear(self.handle)
end

--- Sets the material of the billboard set.
-- @param self Billboard.
-- @param name Material name.
Billboard.set_material = function(self, name)
	Los.billboard_set_material(self.handle, name)
end

--- Sets the position of the billboard set.
-- @param self Billboard.
-- @param v Vector.
Billboard.set_position = function(self, v)
	Los.billboard_set_position(self.handle, v.x, v.y, v.z)
end

--- Sets the render queue of the billboard set.
-- @param self Billboard.
-- @param name Queue name.
Billboard.set_render_queue = function(self, name)
	Los.billboard_set_render_queue(self.handle, name)
end

--- Sets the rotation of the billboard set.
-- @param self Billboard.
-- @param v Quaternion.
Billboard.set_rotation = function(self, v)
	Los.billboard_set_rotation(self.handle, v.x, v.y, v.z, v.w)
end

--- Sets the size of the billboard set.
-- @param self Billboard.
-- @param w Width.
-- @param h Height.
Billboard.set_size = function(self, w, h)
	Los.billboard_set_size(self.handle, w, h)
end

--- Sets the size of the billboard set.
-- @param self Billboard.
-- @param v True to make visible. False otherwise.
Billboard.set_visible = function(self, v)
	Los.billboard_set_visible(self.handle, v)
end

return Billboard
