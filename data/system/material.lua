--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.material
-- @alias Material

local Class = require("system/class")

if not Los.program_load_extension("tiles") then
	error("loading extension `tiles' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type Material
local Material = Class("Material")

--- Creates a new material.
-- @param clss Material class.
-- @param args Arguments.
-- @return New material.
Material.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.material_new()
	if args then
		if args.cullface then self:set_cullface(args.cullface) end
		if args.diffuse then self:set_diffuse(args.diffuse) end
		if args.flags then self:set_flags(args.flags) end
		if args.friction then self:set_friction(args.friction) end
		if args.material then self:set_material(args.material) end
		if args.name then self:set_name(args.name) end
		if args.shader then self:set_shader(args.shader) end
		if args.shininess then self:set_shininess(args.shininess) end
		if args.specular then self:set_specular(args.specular) end
		if args.texture then self:set_texture(args.texture) end
		if args.texture_scale then self:set_texture_scale(args.texture_scale) end
		if args.type then self:set_type(args.type) end
	end
	return self
end

--- Gets the face culling status.
-- @param self Material.
-- @return Boolean.
Material.get_cullface = function(self)
	return Los.material_get_cullface(self.handle)
end

--- Sets the face culling status.
-- @param self Material.
-- @param v Boolean.
Material.set_cullface = function(self, v)
	Los.material_set_cullface(self.handle, v)
end

--- Gets the diffuse color.
-- @param self Material.
-- @return Color table.
Material.get_diffuse = function(self)
	return Los.material_get_diffuse(self.handle)
end

--- Sets the diffuse color.
-- @param self Material.
-- @param v Color table.
Material.set_diffuse = function(self, v)
	Los.material_set_diffuse(self.handle, v)
end

--- Gets the flags of the material.
-- @param self Material.
-- @return Number.
Material.get_flags = function(self)
	return Los.material_get_flags(self.handle)
end

--- Sets the flags of the material.
-- @param self Material.
-- @param v Number.
Material.set_flags = function(self, v)
	Los.material_set_flags(self.handle, v)
end

--- Gets the friction coefficient of the material.
-- @param self Material.
-- @return Number.
Material.get_friction = function(self)
	return Los.material_get_friction(self.handle)
end

--- Sets the friction coefficient of the material.
-- @param self Material.
-- @param v Number.
Material.set_friction = function(self, v)
	Los.material_set_friction(self.handle, v)
end

--- Gets the ID of the material.
-- @param self Material.
-- @return Number.
Material.get_id = function(self)
	return Los.material_get_id(self.handle)
end

--- Gets the name of the Ogre material used by the material.
-- @param self Material.
-- @return String.
Material.get_material = function(self)
	return Los.material_get_material(self.handle)
end

--- Sets the name of the Ogre material used by the material.
-- @param self Material.
-- @param v String.
Material.set_material = function(self, v)
	Los.material_set_material(self.handle, v)
end

--- Gets the name of the material.
-- @param self Material.
-- @return String.
Material.get_name = function(self)
	return Los.material_get_name(self.handle)
end

--- Sets the name of the material.
-- @param self Material.
-- @param v String.
Material.set_name = function(self, v)
	Los.material_set_name(self.handle, v)
end

--- Gets the shader name of the material.
-- @param self Material.
-- @return String.
Material.get_shader = function(self)
	return Los.material_get_shader(self.handle)
end

--- Sets the shader name of the material.
-- @param self Material.
-- @param v String.
Material.set_shader = function(self, v)
	Los.material_set_shader(self.handle, v)
end

--- Gets the shininess of the material.
-- @param self Material.
-- @return Number.
Material.get_shininess = function(self)
	return Los.material_get_shininess(self.handle)
end

--- Sets the shininess of the material.
-- @param self Material.
-- @param v Number.
Material.set_shininess = function(self, v)
	Los.material_set_shininess(self.handle, v)
end

--- Gets the specular color of the material.
-- @param self Material.
-- @return Color table.
Material.get_specular = function(self)
	return Los.material_get_specular(self.handle)
end

--- Sets the specular color of the material.
-- @param self Material.
-- @param v Color table.
Material.set_specular = function(self, v)
	Los.material_set_specular(self.handle, v)
end

--- Gets the texture list of the material.
-- @param self Material.
-- @return List of strings.
Material.get_texture = function(self)
	return Los.material_get_texture(self.handle)
end

--- Sets the texture list of the material.
-- @param self Material.
-- @param v List of strings.
Material.set_texture = function(self, v)
	Los.material_set_texture(self.handle, v)
end

--- Gets the texture scale factor of the material.
-- @param self Material.
-- @return Number.
Material.get_texture_scale = function(self)
	return Los.material_get_texture_scale(self.handle)
end

--- Sets the texture scale factor of the material.
-- @param self Material.
-- @param v Number.
Material.set_texture_scale = function(self, v)
	Los.material_set_texture_scale(self.handle, v)
end

--- Gets the type of the material.
-- @param self Material.
-- @return String.
Material.get_type = function(self)
	return Los.material_get_type(self.handle)
end

--- Sets the type of the material.
-- @param self Material.
-- @param v String.
Material.set_type = function(self, v)
	Los.material_set_type(self.handle, v)
end

return Material


