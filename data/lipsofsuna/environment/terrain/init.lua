local TerrainMaterialSpec = require("core/specs/terrain-material")

Program:add_path(Mod.path)

TerrainMaterialSpec{
	name = "soil1",
	mining_item = "soil chunk"}

TerrainMaterialSpec{
	name = "grass1",
	mining_item = "soil chunk",
	bottom_texture = 0,
	side_texture = 0}

TerrainMaterialSpec{
	name = "granite1",
	mining_item = "granite stone"}
