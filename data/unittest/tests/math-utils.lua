Unittest:add(1, "system", "math utils: radian wrap", function()
	local MathUtils = require("system/math/utils")
	for i = -100,100 do
		local a1 = 0.1 * i
		local a2 = MathUtils:radian_wrap(a1)
		assert(a2 >= -math.pi)
		assert(a2 <= math.pi)
		assert(math.abs(math.sin(a1) - math.sin(a2)) < 0.000001)
		assert(math.abs(math.cos(a1) - math.cos(a2)) < 0.000001)
	end
end)
