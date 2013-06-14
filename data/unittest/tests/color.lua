Unittest:add(1, "system", "color: rgb_multiply_saturation", function()
	local Color = require("system/color")
	local orig = {0.1, 0.4, 0.8}
	local mult = 1.8
	local ref = Color:rgb_to_hsv(orig)
	ref[2] = mult * ref[2]
	ref = Color:hsv_to_rgb(ref)
	local ans = Color:rgb_multiply_saturation(orig, mult)
	assert(math.abs(ans[1] - ref[1]) < 0.01)
	assert(math.abs(ans[2] - ref[2]) < 0.01)
	assert(math.abs(ans[3] - ref[3]) < 0.01)
end)

Unittest:add(1, "system", "color: rgb_multiply_value", function()
	local Color = require("system/color")
	local orig = {0.1, 0.4, 0.8}
	local mult = 1.8
	local ref = Color:rgb_to_hsv(orig)
	ref[3] = mult * ref[3]
	ref = Color:hsv_to_rgb(ref)
	local ans = Color:rgb_multiply_value(orig, mult)
	assert(math.abs(ans[1] - ref[1]) < 0.01)
	assert(math.abs(ans[2] - ref[2]) < 0.01)
	assert(math.abs(ans[3] - ref[3]) < 0.01)
end)

Unittest:add(1, "system", "color: rgb_set_saturation", function()
	local Color = require("system/color")
	local orig = {0.1, 0.4, 0.8}
	local value = 0.3
	local ref = Color:rgb_to_hsv(orig)
	ref[2] = value
	ref = Color:hsv_to_rgb(ref)
	local ans = Color:rgb_set_saturation(orig, value)
	assert(math.abs(ans[1] - ref[1]) < 0.01)
	assert(math.abs(ans[2] - ref[2]) < 0.01)
	assert(math.abs(ans[3] - ref[3]) < 0.01)
end)

Unittest:add(1, "system", "color: rgb_set_value", function()
	local Color = require("system/color")
	local orig = {0.1, 0.4, 0.8}
	local value = 0.3
	local ref = Color:rgb_to_hsv(orig)
	ref[3] = value
	ref = Color:hsv_to_rgb(ref)
	local ans = Color:rgb_set_value(orig, value)
	assert(math.abs(ans[1] - ref[1]) < 0.01)
	assert(math.abs(ans[2] - ref[2]) < 0.01)
	assert(math.abs(ans[3] - ref[3]) < 0.01)
end)
