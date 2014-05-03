--- Asynchronous image loading.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.image_loader
-- @alias ImageLoader

local Class = require("system/class")
local Image = require("system/image")

if not Los.program_load_extension("image") then
	error("loading extension `image' failed")
end

------------------------------------------------------------------------------

--- Asynchronous image loading.
-- @type ImageLoader
local ImageLoader = Class("ImageLoader")

--- Creates an image loader.
-- @param clss ImageLoader class.
-- @param file Filename.
-- @return New image loader.
ImageLoader.new = function(clss, file)
	local self = Class.new(clss)
	self.handle = Los.image_loader_new(file)
	return self
end

--- Checks whether the loader has finished.
-- @param self ImageLoader.
-- @return True if finished. False otherwise.
ImageLoader.get_done = function(self)
	if not self.handle then return true end
	return Los.image_loader_get_done(self.handle)
end

--- Returns the image if it has finished loading.
-- @param self ImageLoader.
-- @return Image if loaded successfully. Nil otherwise.
ImageLoader.get_image = function(self)
	if not self.handle then return end
	if not self.image then
		local handle = Los.image_loader_get_image(self.handle)
		if handle then self.image = Image:new_from_handle(handle) end
	end
	return self.image
end

return ImageLoader
