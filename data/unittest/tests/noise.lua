Unittest:add(1, "system", "noise: simplex noise", function()
	local Noise = require("system/noise")
	-- Value range.
	local min = 1000
	local max = -1000
	for x = 0,2000000,1000 do
		for y = 0,200000,1000 do
			local n = Noise:simplex_noise_2d(x, y)
			min = math.min(min, n)
			max = math.max(max, n)
		end
	end
	assert(min >= -1)
	assert(min <= -0.99)
	assert(max <= 1)
	assert(max >= 0.99)
end)

Unittest:add(1, "system", "noise: plasma noise", function()
	local Noise = require("system/noise")
	local n = Noise:plasma_noise_2d(100, 100, 0.5)
	assert(type(n) == "number")
	assert(n >= -1)
	assert(n <= 1)
end)

Unittest:add(1, "system", "noise: harmonic noise", function()
	local Noise = require("system/noise")
	-- Value range.
	local min = 1000
	local max = -1000
	for x = 0,2000000,10000 do
		for y = 0,200000,1000 do
			local n = Noise:harmonic_noise_2d(x, y, 2, 1.2, 0.3)
			min = math.min(min, n)
			max = math.max(max, n)
		end
	end
	assert(min >= -1)
	assert(min <= -0.95)
	assert(max <= 1)
	assert(max >= 0.95)
end)
