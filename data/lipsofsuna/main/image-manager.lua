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
local ImageLoader = require("system/image-loader")

--- Implements loading and caching of images.
-- @type ImageManager
local ImageManager = Class("ImageManager")

--- Creates a new image manager.
-- @param clss ImageManager class.
-- @return ImageManager.
ImageManager.new = function(clss)
	local self = Class.new(clss)
	self.images_by_name = setmetatable({}, {__mode = "v"})
	self.loaders_by_name = {}
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

--- Finds or loads a image by name and returns a freely modifiable copy.
-- @param self ImageManager.
-- @param name Image filename.
-- @param yield Yield function.
-- @return Image.
ImageManager.copy_by_name_async = function(self, name, yield)
	local image = self:find_by_name_async(name, yield)
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
		local loader = self.loaders_by_name[name]
		if loader then
			while not loader:get_done() do end
			image = loader:get_image()
			self.images_by_name[name] = image
			self.loaders_by_name[name] = nil
		else
			image = Image(name .. ".png")
			self.images_by_name[name] = image
		end
		if not image then return end
	end
	-- Update caching.
	self.seconds_by_image[image] = 100
	return image
end

--- Finds or loads a image by name.
-- @param self ImageManager.
-- @param name Image filename.
-- @param yield Yield function.
-- @return Image.
ImageManager.find_by_name_async = function(self, name, yield)
	-- Find or load the image.
	local image = self.images_by_name[name]
	if not image then
		if not self.loaders_by_name[name] then
			self.loaders_by_name[name] = ImageLoader(name .. ".png")
		end
		while self.loaders_by_name[name] do yield() end
		image = self.images_by_name[name]
		if not image then return end
	end
	-- Update caching.
	self.seconds_by_image[image] = 100
	return image
end

--- Updates caching of images.
-- @param self ImageManager.
-- @param secs Seconds since the last update.
ImageManager.update = function(self, secs)
	-- Update the asynchronous image loaders.
	for name,loader in pairs(self.loaders_by_name) do
		if loader:get_done() then
			self.images_by_name[name] = loader:get_image()
			self.loaders_by_name[name] = nil
		end
	end
	-- Unload unused images.
	self.update_timer = self.update_timer + secs
	if self.update_timer >= 3 then
		self.update_timer = 0
		local res = {}
		for image,time in pairs(self.seconds_by_image) do
			if time > secs then
				res[image] = time - secs
			end
		end
		self.seconds_by_image = res
	end
end

return ImageManager
