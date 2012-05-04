Program:add_path(Mod.path)
Program:add_path(Mod.path .. "materials")
Program:add_path(Mod.path .. "shaders")

if not Settings then
	-- Modelbuilder only
	require "common/model"
	require "common/material"
else
	Voxel.blocks_per_line = 1
	Voxel.tiles_per_line = 12
	require "common/color"
	require "common/crafting"
	require "common/feat"
	require "common/material"
	require "common/marker"
	require "common/model"
	require "common/names"
	require "common/object"
	require "common/protocol"
	require "common/serialize"
	require "common/sectors"
	require "common/voxel"
	require "system/lobby"
end
