Unittest:add(2, "voxel render", function()
	require "system/graphics"
	require "system/tiles"
	require "system/tiles-render"
	-- Checks for valgrind.
	local mat = Material{name = "test1", shader = "default", type = "rounded"}
	for i=1,100 do
		Voxel:set_tile(Vector(100+10*i,100,100), mat.id)
		Voxel:update(100)
	end
end)
