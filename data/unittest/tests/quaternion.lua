Unittest:add(1, "system", "quaternion", function()
	local Quaternion = require("system/math/quaternion")
	local Vector = require("system/math/vector")
	-- XYZW presentation.
	local q1 = Quaternion(1,0,0,0)
	assert(q1.x == 1)
	assert(q1.length == 1)
	-- Axis-angle presentation.
	local q2 = Quaternion{axis = Vector(0,1,0), angle = math.pi}
	assert(q2.y == 1)
	assert(q2.length == 1)
	-- Look-at presentation.
	local q3 = Quaternion{dir = Vector(0,0,1), up = Vector(0,1,0)}
	assert(q3.y == -1)
	assert(q3.length == 1)
	-- Euler presentation.
	local q4 = Quaternion:new_from_euler(0,0,1.5)
	assert(q4.euler[1] == 0)
	assert(q4.euler[2] == 0)
	assert(q4.euler[3] == 1.5)
	-- Multiplication.
	local q5 = q4 * q3
	assert(q5.class == Quaternion)
	-- Transforming a vector.
	local q6 = Quaternion{axis = Vector(0,1,0), angle = math.pi}
	local v1 = q6 * Vector(1,0,0)
	assert(v1.class == Vector)
	assert(math.abs(v1.x + 1) < 0.0001 and math.abs(v1.y) < 0.0001 and math.abs(v1.z) < 0.0001)
end)
