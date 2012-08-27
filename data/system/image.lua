--- TODO:doc
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

--- TODO:doc
-- @type Image
local Image = Class("Image")

--- Loads an image from a file.
-- @param clss Image class.
-- @param file Filename.
-- @return New image, or nil.
Image.new = function(clss, file)
	local handle = Los.image_new(file)
	if not handle then return end
	local self = Class.new(clss)
	self.handle = handle
	return self
end

return Image


