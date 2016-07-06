--- A front end for changing named textures globally.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.texture
-- @alias Texture

local Class = require("system/class")

if not Los.program_load_extension("render") then
	error("loading extension `render' failed")
end

------------------------------------------------------------------------------

--- A front end for changing named textures globally.
--
-- This class is intended to give access to named, 'permanent', global textures, and to be able to change the texture's image for all instandces of its use. For changing textures of a single model, look for the replace_texture() member function in the relevant class.
--
-- @type Texture
local Texture = Class("Texture")

--- Instantiates a new named texture.
-- @param clss Texture class.
-- @param name The name of the texture.
Texture.new = function(clss, name)
	local self = Class.new(clss)
	self.handle = Los.texture_new(name)
	print(self.handle)
	self.name = name
	return self
end

--- Replaces a texture with the supplied Image.
-- @param self Texture.
-- @param image The Image with which to replace the texture.
Texture.set_image = function(self, image)
	Los.texture_set_image(self.handle, image.handle)
end


return Texture
