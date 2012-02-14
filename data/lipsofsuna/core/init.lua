Program:add_path(Mod.path)
Program:add_path(Mod.path .. "materials")
Program:add_path(Mod.path .. "shaders")

if not Settings then
	-- Modelbuilder only
	require "common/dialogspec"
	require "common/effect"
	require "common/faction"
	require "common/feat"
	require "common/model"
	require "common/iconspec"
	require "common/itemspec"
	require "common/material"
	require "common/obstaclespec"
	require "common/pattern"
	require "common/regionspec"
	require "common/species"
	require "common/staticspec"
	require "common/quest"
else
	Voxel.blocks_per_line = 1
	Voxel.tiles_per_line = 12
	require "common/actionspec"
	require "common/color"
	require "common/crafting"
	require "common/dialogspec"
	require "common/effect"
	require "common/faction"
	require "common/feat"
	require "common/iconspec"
	require "common/influencespec"
	require "common/itemspec"
	require "common/material"
	require "common/marker"
	require "common/model"
	require "common/modelspec"
	require "common/names"
	require "common/object"
	require "common/obstaclespec"
	require "common/pattern"
	require "common/protocol"
	require "common/quest"
	require "common/regionspec"
	require "common/serialize"
	require "common/sectors"
	require "common/species"
	require "common/staticspec"
	require "common/voxel"
	require "system/lobby"
end

require(Mod.path .. "factions")
require(Mod.path .. "feats")
require(Mod.path .. "icons")
require(Mod.path .. "materials")
require(Mod.path .. "models")
require(Mod.path .. "species")
