Unittest:add(1, "system", "array", function()
	local NumberArray2d = require("system/array/number-2d")
	local a = NumberArray2d(100, 200)
	assert(a)
	assert(a:get(0, 0) == 0)
	assert(a:get(-1, 0) == nil)
	assert(a:get(0, -1) == nil)
	assert(a:get(0, 0) == 0)
	assert(a:get(99, 199) == 0)
	assert(a:get(100, 199) == nil)
	assert(a:get(99, 200) == nil)
	a:set(50, 0, 7.5)
	for x=0,99 do
		for z=0,199 do
			if x == 50 and z == 0 then
				assert(a:get(x, z) == 7.5)
			else
				assert(a:get(x, z) == 0)
			end
		end
	end
end)

Unittest:add(1, "system", "array: out of memory", function()
	local NumberArray2d = require("system/array/number-2d")
	local ok,e = pcall(NumberArray2d.new, NumberArray2d, 0xFFFF, 0xFFFF)
	assert(not ok)
end)
