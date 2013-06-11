--- Image loading and manipulation.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.image
-- @alias Image

local Class = require("system/class")

if not Los.program_load_extension("image") then
	error("loading extension `image' failed")
end

------------------------------------------------------------------------------

--- Image loading and manipulation.
-- @type Image
local Image = Class("Image")

--- Loads an image from a file.
-- @param clss Image class.
-- @param file_or_width Filename or width.
-- @param height Height or nil.
-- @return New image, or nil.
Image.new = function(clss, file_or_width, height)
	local handle
	if type(file_or_width) == "string" then
		handle = Los.image_new(file_or_width)
	else
		handle = Los.image_new_empty(file_or_width, height)
	end
	if not handle then return end
	local self = Class.new(clss)
	self.handle = handle
	return self
end

--- Blits an image.
-- @param self Image.
-- @param image Image.
Image.blit = function(self, image)
	Los.image_blit(self.handle, image.handle)
end

--- Copies the image.
-- @param self Image.
-- @return New image.
Image.copy = function(self)
	local handle = Los.image_copy(self.handle)
	if not handle then return end
	local image = Class.new(self.class)
	image.handle = handle
	return image
end

--- Fills the image.
-- @param self Image.
-- @param r Red.
-- @param g Green.
-- @param b Blue.
-- @param a Alpha.
Image.fill = function(self, r, g, b, a)
	Los.image_fill(self.handle, r, g, b, a)
end

--- Gets the height of the image.
-- @param self Image.
-- @return Number.
Image.get_height = function(self)
	return Los.image_get_height(self.handle)
end

--- Gets the color of the pixel.
-- @param self Image.
-- @param x X coordinate.
-- @param y Y coordinate.
-- @return R,G,B,A on success. Nil otherwise.
Image.get_pixel = function(self, x, y)
	return Los.image_get_pixel(self.handle, x, y)
end

--- Sets the color of the pixel.
-- @param self Image.
-- @param x X coordinate.
-- @param y Y coordinate.
-- @param r Red.
-- @param g Green.
-- @param b Blue.
-- @param a Alpha.
Image.set_pixel = function(self, x, y, r, g, b, a)
	return Los.image_set_pixel(self.handle, x, y, r, g, b, a)
end

--- Gets the width and height of the image.
-- @param self Image.
-- @return Number.
Image.get_size = function(self)
	return Los.image_get_size(self.handle)
end

--- Gets the width of the image.
-- @param self Image.
-- @return Number.
Image.get_width = function(self)
	return Los.image_get_width(self.handle)
end

return Image
