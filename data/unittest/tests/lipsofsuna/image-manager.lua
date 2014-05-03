Unittest:add(1, "lipsofsuna", "image manager: load", function()
	local ImageManager = require("main/image-manager")
	local mgr = ImageManager()
	local i = mgr:find_by_name("image")
	assert(i)
	local w,h = i:get_size()
	assert(w == 32)
	assert(h == 32)
end)

Unittest:add(1, "lipsofsuna", "image manager: load async", function()
	local ImageManager = require("main/image-manager")
	local mgr = ImageManager()
	local cr = coroutine.create(function()
		local i = mgr:find_by_name_async("image", coroutine.yield)
		assert(i)
		local w,h = i:get_size()
		assert(w == 32)
		assert(h == 32)
	end)
	local yields = 0
	repeat
		local ret,err = coroutine.resume(cr)
		assert(ret)
		mgr:update(0)
		yields = yields + 1
	until coroutine.status(cr) == "dead"
	assert(yields >= 2)
end)
