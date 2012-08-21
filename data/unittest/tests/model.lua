Unittest:add(1, "system", "model", function()
	local Model = require("system/model")
	-- Creating models.
	local m = Model()
	assert(m)
	assert(m.handle)
	-- Copying models.
	local m1 = m:copy()
	assert(m1)
	assert(m1.handle)
	-- Function access.
	m:calculate_bounds()
end)
