Unittest:add(2, "system", "voxel render", function()
	require "system/graphics"
	local Voxel = require("system/tiles")
	require "system/tiles-render"
	local Material = require("system/material")
	local Vector = require("system/math/vector")
	-- Checks for valgrind.
	local mat = Material{name = "test1", shader = "default", type = "rounded"}
	for i=1,100 do
		Voxel:set_tile(Vector(100+10*i,100,100), mat.id)
		Voxel:update(100)
	end
end)
