Unittest:add(1, "vector", function()
	require "system/math"
	-- Addition.
	local vec = Vector(1, 100) + Vector(5,5,5)
	assert(vec.x == 6 and vec.y == 105 and vec.z == 5)
	-- Normalization.
	assert(math.abs(1-vec:normalize().length) < 0.0001)
	-- Scalar multiplication.
	local a = Vector(1,-2,3) * 5
	assert(a.class == Vector)
	assert(math.abs(a.x - 5) < 0.0001)
	assert(math.abs(a.y + 10) < 0.0001)
	assert(math.abs(a.z - 15) < 0.0001)
	-- Dot product.
	local b = a:dot(a)
	assert(math.abs(b - 350) < 0.0001)
	-- Cross product.
	local c = Vector(1):cross(Vector(0,1))
	assert(math.abs(c.x) < 0.0001)
	assert(math.abs(c.y) < 0.0001)
	assert(math.abs(c.z - 1) < 0.0001)
	-- Protection from nan and inf.
	local d = Vector(0/0, -0/0, 1/0)
	assert(d.x == 0 and d.y == 0 and d.z == 0)
	d:normalize()
	assert(d.x == 0 and d.y == 0 and d.z == 0)
end)