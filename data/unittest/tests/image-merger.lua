Unittest:add(1, "system", "image merger: blit", function()
	local Image = require("system/image")
	local ImageMerger = require("system/image-merger")
	-- Create the merger.
	local m = ImageMerger()
	assert(m)
	assert(m.handle)
	-- Replace.
	local i1 = Image("image.png")
	m:replace(i1)
	-- Blit.
	local i2 = Image(32, 32)
	i2:fill(255, 255, 0, 128)
	m:blit(i2)
	-- Finish.
	assert(not m:pop_image())
	m:finish()
	local res
	repeat res = m:pop_image() until res
	-- Validate.
	local r,g,b,a = res:get_pixel(0, 0)
	assert(r == 255 and g == 255 and b == 127 and a == 255)
	local r,g,b,a = res:get_pixel(31, 31)
	assert(r == 255 and g == 127 and b == 0 and a == 255)
end)

Unittest:add(1, "system", "image merger: blit rect", function()
	local Image = require("system/image")
	local ImageMerger = require("system/image-merger")
	-- Create the merger.
	local m = ImageMerger()
	assert(m)
	assert(m.handle)
	-- Replace.
	local i1 = Image("image.png")
	m:replace(i1)
	-- Blit.
	local i2 = Image(32, 32)
	i2:fill(255, 255, 0, 128)
	m:blit_rect(i2, -16, -16, 1000000, 1000000, 0, 0)
	-- Finish.
	assert(not m:pop_image())
	m:finish()
	local res
	repeat res = m:pop_image() until res
	-- Validate.
	local r,g,b,a = res:get_pixel(0, 0)
	assert(r == 255 and g == 255 and b == 127 and a == 255)
	local r,g,b,a = res:get_pixel(31, 31)
	assert(r == 255 and g == 0 and b == 0 and a == 255)
end)

Unittest:add(1, "system", "image merger: replace", function()
	local Image = require("system/image")
	local ImageMerger = require("system/image-merger")
	-- Create the merger.
	local m = ImageMerger()
	assert(m)
	assert(m.handle)
	-- Replace.
	local i1 = Image("image.png")
	m:replace(i1)
	-- Finish.
	assert(not m:pop_image())
	m:finish()
	local res
	repeat res = m:pop_image() until res
	-- Validate.
	local r,g,b,a = res:get_pixel(31, 31)
	assert(r == 255 and g == 0 and b == 0 and a == 255)
	-- Replace.
	i1:fill(0, 255, 0, 255)
	m:replace(i1)
	-- Finish.
	assert(not m:pop_image())
	m:finish()
	local res
	repeat res = m:pop_image() until res
	-- Validate.
	local r,g,b,a = res:get_pixel(31, 31)
	assert(r == 0 and g == 255 and b == 0 and a == 255)
end)
