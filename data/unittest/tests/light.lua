Unittest:add(2, "light", function()
	require "system/graphics"
	require "system/render"
	-- Position.
	local l = Light{position = Vector(1,2,3)}
	assert(l.position.x == 1)
	assert(l.position.y == 2)
	assert(l.position.z == 3)
	-- Equation.
	l.equation = {3,2,1}
	assert(l.equation[1] == 3)
	assert(l.equation[2] == 2)
	assert(l.equation[3] == 1)
	-- Enabling.
	l.enabled = false
	assert(l.enabled == false)
	l.enabled = true
	assert(l.enabled == true)
end)
