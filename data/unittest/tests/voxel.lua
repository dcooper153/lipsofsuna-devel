Unittest:add(1, "system", "voxel", function()
	local Vector = require("system/math/vector")
	local Voxel = require("system/tiles")
	-- Getting and setting tiles.
	assert(Voxel:get_tile(Vector(100,101,102)) == 0)
	Voxel:set_tile(Vector(100,101,102), 5)
	assert(Voxel:get_tile(Vector(100,101,102)) == 5)
end)

Unittest:add(1, "system", "voxel get block offsets", function()
	local Sectors = require("system/sectors")
	local Voxel = require("system/tiles")
	local n = 0
	local map = {}
	Sectors:unload_all()
	Voxel:set_block_size(1, 12)
	for x,y,z in Voxel:get_blocks_by_sector_id(1) do
		assert(x >= Voxel.blocks_per_line)
		assert(x < 2 * Voxel.blocks_per_line)
		assert(y >= 0)
		assert(y < Voxel.blocks_per_line)
		assert(z >= 0)
		assert(z < Voxel.blocks_per_line)
		local i = x + y * 1000 + z * 1000000
		assert(not map[i])
		map[i] = true
		n = n + 1
	end
	assert(n == Voxel.blocks_per_line^3)
end)
