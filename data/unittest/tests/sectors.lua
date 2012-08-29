Unittest:add(1, "system", "sectors", function()
	local Sectors = require("system/sectors")
	local Vector = require("system/math/vector")
	-- Unloading.
	Sectors:unload_all()
	Sectors:unload_sector(1)
	assert(type(Sectors:get_sectors()) == "table")
	assert(not Sectors:get_sector_idle())
	-- Refreshing.
	Sectors:refresh(Vector(100,100,100), 1)
	local num = 0
	for k,v in pairs(Sectors:get_sectors()) do
		assert(type(k) == "number")
		assert(type(v) == "number")
		assert(v < 2)
		num = num + 1
	end
	assert(num == 1)
end)
