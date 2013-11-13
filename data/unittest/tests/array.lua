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

Unittest:add(1, "system", "array: add", function()
	local NumberArray2d = require("system/array/number-2d")
	local a = NumberArray2d(10, 10)
	for x=0,9 do
		for z=0,9 do
			a:set(x, z, 1 + x + z)
		end
	end
	assert(a:get_min() == 1)
	assert(a:get_max() == 19)
	a:add(100)
	assert(a:get_min() == 101)
	assert(a:get_max() == 119)
end)

Unittest:add(1, "system", "array: bilinear interpolation", function()
	local NumberArray2d = require("system/array/number-2d")
	local a = NumberArray2d(3, 3)
	a:set(0, 0, 0) a:set(1, 0, 4) a:set(2, 0, 1)
	a:set(0, 1, 4) a:set(1, 1, 2) a:set(2, 1, 1)
	a:set(0, 2, 4) a:set(1, 2, 2) a:set(2, 2, 1)
	assert(a:get_bilinear(0.25, 0) == 1)
	assert(a:get_bilinear(0.5, 0) == 2)
	assert(a:get_bilinear(0.5, 0.5) == 2.5)
	assert(a:get_bilinear(1.5, 2) == 1.5)
	assert(a:get_bilinear(-100, 1.5) == 4)
	assert(a:get_bilinear(1, -100) == 4)
	assert(a:get_bilinear(100, 1.5) == 1)
	assert(a:get_bilinear(1, 100) == 2)
end)

Unittest:add(1, "system", "array: gradient", function()
	local NumberArray2d = require("system/array/number-2d")
	local a = NumberArray2d(3, 3)
	a:set(0, 0, 0) a:set(1, 0, 2) a:set(2, 0, 4)
	a:set(0, 1, 2) a:set(1, 1, 4) a:set(2, 1, 6)
	a:set(0, 2, 4) a:set(1, 2, 6) a:set(2, 2, 8)
	local x,y,m,d = a:get_gradient(1, 1)
	assert(x == -4)
	assert(y == -4)
	assert(math.abs(m - math.sqrt(32)) < 0.00001)
	local x,y,m,d = a:get_gradient(0, 0)
	assert(x == -2)
	assert(y == -2)
	assert(math.abs(m - math.sqrt(8)) < 0.00001)
end)

Unittest:add(1, "system", "array: gradient bilinear", function()
	local NumberArray2d = require("system/array/number-2d")
	local a = NumberArray2d(4, 4)
	a:set(0, 0, 0) a:set(1, 0, 2) a:set(2, 0, 4)  a:set(3, 0, 6)
	a:set(0, 1, 2) a:set(1, 1, 4) a:set(2, 1, 6)  a:set(3, 1, 8)
	a:set(0, 2, 4) a:set(1, 2, 6) a:set(2, 2, 8)  a:set(3, 2, 10)
	a:set(0, 3, 6) a:set(1, 3, 8) a:set(2, 3, 10) a:set(3, 3, 12)
	local x,y,m,d = a:get_gradient_bilinear(2.5, 2.5)
	assert(x == -2.75)
	assert(y == -2.75)
end)

Unittest:add(1, "system", "array: out of memory", function()
	local NumberArray2d = require("system/array/number-2d")
	local ok,e = pcall(NumberArray2d.new, NumberArray2d, 0xFFFF, 0xFFFF)
	assert(not ok)
end)

Unittest:add(1, "system", "array: serialization", function()
	local NumberArray2d = require("system/array/number-2d")
	local a = NumberArray2d(10, 10)
	local b = NumberArray2d(10, 10)
	for x=0,9 do
		for z=0,9 do
			a:set(x, z, math.random())
		end
	end
	local d = a:get_data()
	d:read("uint8")
	b:set_data(d)
	for x=0,9 do
		for z=0,9 do
			assert(a:get(x, z) == b:get(x, z))
		end
	end
end)
