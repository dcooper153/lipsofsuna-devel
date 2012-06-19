Unittest:add(1, "voxel", function()
	require "system/tiles"
	-- Getting and setting tiles.
	assert(Voxel:get_tile(Vector(100,101,102)) == 0)
	Voxel:set_tile(Vector(100,101,102), 5)
	assert(Voxel:get_tile(Vector(100,101,102)) == 5)
end)
