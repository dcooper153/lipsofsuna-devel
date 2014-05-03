Unittest:add(1, "system", "image loader: load", function()
	local ImageLoader = require("system/image-loader")
	local l = ImageLoader("image.png")
	while not l:get_done() do
	end
	local i = l:get_image()
	assert(i)
	local w,h = i:get_size()
	assert(w == 32)
	assert(h == 32)
end)
