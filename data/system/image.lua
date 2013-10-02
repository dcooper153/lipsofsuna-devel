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

--- Creates a new image from an internal handle.
-- @param clss Image class.
-- @param handle Handle.
-- @return New image.
Image.new_from_handle = function(clss, handle)
	local self = Class.new(clss)
	self.handle = handle
	return self
end

--- Alters the color of the image in the HSV color space.
-- @param self Image.
-- @param hue_add Amount to add to hue. The hue range is [0,1].
-- @param sat_add Amount to add to saturation. The saturation range is [0,1].
-- @param val_add Amount to add to value. The value range is [0,1].
Image.add_hsv = function(self, hue_add, sat_add, val_add)
	Los.image_add_hsv(self.handle, hue_add, sat_add, val_add)
end

--- Alters the color of the image in the HSV color space, using value weighting.
-- @param self Image.
-- @param hue_add Amount to add to hue. The hue range is [0,1].
-- @param sat_add Amount to add to saturation. The saturation range is [0,1].
-- @param val_add Amount to add to value. The value range is [0,1].
-- @param val_range Value weighting range. The value range is [0,0.5].
Image.add_hsv_weightv = function(self, hue_add, sat_add, val_add, val_range)
	Los.image_add_hsv_weightv(self.handle, hue_add, sat_add, val_add, val_range)
end

--- Blits an image.
-- @param self Image.
-- @param image Image.
Image.blit = function(self, image)
	Los.image_blit(self.handle, image.handle)
end

--- Blits an HSV altered image over this one.
-- @param self Image.
-- @param image Image.
-- @param hue_add Amount to add to hue. The hue range is [0,1].
-- @param sat_add Amount to add to saturation. The saturation range is [0,1].
-- @param val_add Amount to add to value. The value range is [0,1].
Image.blit_hsv_add = function(self, image, hue_add, sat_add, val_add)
	Los.image_blit_hsv_add(self.handle, image.handle, hue_add, sat_add, val_add)
end

--- Blits an HSV altered image over this one, using value weighting.
-- @param self Image.
-- @param image Image.
-- @param hue_add Amount to add to hue. The hue range is [0,1].
-- @param sat_add Amount to add to saturation. The saturation range is [0,1].
-- @param val_add Amount to add to value. The value range is [0,1].
-- @param val_range Value weighting range. The value range is [0,0.5].
Image.blit_hsv_add_weightv = function(self, image, hue_add, sat_add, val_add, val_range)
	Los.image_blit_hsv_add_weightv(self.handle, image.handle, hue_add, sat_add, val_add, val_range)
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
