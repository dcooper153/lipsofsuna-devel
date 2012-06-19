Unittest:add(1, "ai", function()
	require "system/ai"
	require "system/math"
	-- Create the maze.
	for x=98,102 do
		for z=98,102 do
			Voxel:set_tile(Vector(x,99,z),1)
		end
	end
	Voxel:set_tile(Vector(100,100,99), 1)
	Voxel:set_tile(Vector(100,100,100), 1)
	Voxel:set_tile(Vector(100,100,101), 1)
	assert(Voxel:get_tile(Vector(100,100,100)) == 1)
	Voxel:update()
	-- Path solving.
	local p = Ai:solve_path{start = Vector(99,100,100), target = Vector(101,100,100)}
	assert(#p == 5)
end)
