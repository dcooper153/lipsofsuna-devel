Unittest:add(1, "system", "vector", function()
	local Vector = require("system/math/vector")
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
	-- Multiplication.
	local e = Vector(100, -100, 50) * 0.5
	assert(e.x == 50 and e.y == -50 and e.z == 25)
	-- Subtraction.
	local f = Vector(600, 500, 400) - Vector(100, -100, 50)
	assert(f.x == 500 and f.y == 600 and f.z == 350)
	-- Getters and setters.
	local g = Vector()
	g.x = 100
	g.y = 200
	g.z = -100
	assert(g.x == 100 and g.y == 200 and g.z == -100)
	-- Transformation.
	local Quaternion = require("system/math/quaternion")
	local h = Vector(1, 2, 3)
	h:transform(Quaternion{axis = Vector(0,1,0), angle = math.pi})
	assert(math.abs(h.x + 1) < 0.0001 and math.abs(h.y - 2)  < 0.0001 and math.abs(h.z + 3) < 0.0001)
end)
