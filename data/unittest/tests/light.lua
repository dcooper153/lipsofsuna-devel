Unittest:add(2, "system", "light", function()
	local Program = require("system/graphics")
	local Light = require("system/light")
	local Vector = require("system/math/vector")
	-- Position.
	local l = Light()
	l:set_position(Vector(1,2,3))
	assert(l:get_position().x == 1)
	assert(l:get_position().y == 2)
	assert(l:get_position().z == 3)
	-- Equation.
	l:set_equation{3,2,1}
	assert(l:get_equation()[1] == 3)
	assert(l:get_equation()[2] == 2)
	assert(l:get_equation()[3] == 1)
	-- Enabling.
	l:set_enabled(false)
	assert(l:get_enabled() == false)
	l:set_enabled(true)
	assert(l:get_enabled() == true)
end)
