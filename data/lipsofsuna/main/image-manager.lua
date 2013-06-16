--- Implements loading and caching of images.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.main.image_manager
-- @alias ImageManager

local Class = require("system/class")
local Image = require("system/image")

--- Implements loading and caching of images.
-- @type ImageManager
local ImageManager = Class("ImageManager")

--- Creates a new image manager.
-- @param clss ImageManager class.
-- @return ImageManager.
ImageManager.new = function(clss)
	local self = Class.new(clss)
	self.images_by_name = setmetatable({}, {__mode = "v"})
	self.seconds_by_image = {}
	self.update_timer = 0
	return self
end

--- Finds or loads a image by name and returns a freely modifiable copy.
-- @param self ImageManager.
-- @param name Image filename.
-- @return Image.
ImageManager.copy_by_name = function(self, name)
	local image = self:find_by_name(name)
	if image then return image:copy() end
end

--- Finds or loads a image by name.
-- @param self ImageManager.
-- @param name Image filename.
-- @return Image.
ImageManager.find_by_name = function(self, name)
	-- Find or load the image.
	local image = self.images_by_name[name]
	if not image then
		-- Load a new image.
		image = Image(name .. ".png")
		if not image then return end
		self.images_by_name[name] = image
	end
	-- Update caching.
	self.seconds_by_image[image] = 100
	return image
end

--- Updates caching of images.
-- @param self ImageManager.
-- @param secs Seconds since the last update.
ImageManager.update = function(self, secs)
	-- Avoid updating too frequently.
	self.update_timer = self.update_timer + secs
	if self.update_timer < 3 then return end
	self.update_timer = 0
	-- Recreate the timing table.
	local res = {}
	for image,time in pairs(self.seconds_by_image) do
		if time > secs then
			res[image] = time - secs
		end
	end
	self.seconds_by_image = res
end

return ImageManager
