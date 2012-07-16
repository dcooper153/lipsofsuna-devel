require "system/class"

if not Los.program_load_extension("image") then
	error("loading extension `image' failed")
end

------------------------------------------------------------------------------

Image = Class()
Image.class_name = "Image"

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
