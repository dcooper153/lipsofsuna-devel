--- Merges images in a background thread.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.image_merger
-- @alias ImageMerger

local Class = require("system/class")
local Image = require("system/image")

if not Los.program_load_extension("image-merge") then
	error("loading extension `image-merge' failed")
end

------------------------------------------------------------------------------

--- Merges images in a background thread.
-- @type ImageMerger
local ImageMerger = Class("ImageMerger")

--- Creates a new image merger.
-- @param clss ImageMerger class.
-- @return ImageMerger.
ImageMerger.new = function(clss)
	local self = Class.new(ImageMerger)
	self.handle = Los.image_merger_new()
	return self
end

--- Adds a HSV color to the image.
-- @param self ImageMerger.
-- @param h Hue to add.
-- @param s Saturation to add.
-- @param v Value to add.
ImageMerger.add_hsv = function(self, h, s, v)
	Los.image_merger_add_hsv(self.handle, h, s, v)
end

--- Adds a HSV color to the image.
-- @param self ImageMerger.
-- @param h Hue to add.
-- @param s Saturation to add.
-- @param v Value to add.
-- @param vw Value weighting range.
ImageMerger.add_hsv_weightv = function(self, h, s, v, vw)
	Los.image_merger_add_hsv_weightv(self.handle, h, s, v, vw)
end

--- Blits an image.
-- @param self ImageMerger.
-- @param image Image to blit.
ImageMerger.blit = function(self, image)
	Los.image_merger_blit(self.handle, image.handle)
end

--- Blits an HSV adjusted image.
-- @param self ImageMerger.
-- @param image Image to blit.
-- @param h Hue to add.
-- @param s Saturation to add.
-- @param v Value to add.
ImageMerger.blit_hsv_add = function(self, image, h, s, v)
	Los.image_merger_blit_hsv_add(self.handle, image.handle, h, s, v)
end

--- Blits an HSV adjusted image with value weighting.
-- @param self ImageMerger.
-- @param image Image to blit.
-- @param h Hue to add.
-- @param s Saturation to add.
-- @param v Value to add.
-- @param vw Value weighting range.
ImageMerger.blit_hsv_add_weightv = function(self, image, h, s, v, vw)
	Los.image_merger_blit_hsv_add_weightv(self.handle, image.handle, h, s, v, vw)
end

--- Queues finishing the build.
-- @param self ImageMerger.
ImageMerger.finish = function(self)
	Los.image_merger_finish(self.handle)
end

--- Pops a finished image.
-- @param self ImageMerger.
-- @return Image or nil.
ImageMerger.pop_image = function(self)
	local handle = Los.image_merger_pop_image(self.handle)
	if not handle then return end
	return Image:new_from_handle(handle)
end

--- Replaces the image.
-- @param self ImageMerger.
-- @param image Image.
ImageMerger.replace = function(self, image)
	Los.image_merger_replace(self.handle, image.handle)
end

return ImageMerger
