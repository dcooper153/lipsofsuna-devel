Program:add_path(Mod.path)

Itemspec{
	name = "mushroom",
	model = "mushroom-000",--"mushroom-001"..."mushroom-008"
	icon = "mushroom1",
	description = "An ingredient for magical reagents",
	categories = {"material"},
	mass = 1,
	stacking = true}

Obstaclespec{
	name = "mushroom",
	model = "mushroom-000",
	categories = {"small-plant", "reagent"},
	health = 10,
	harvest_behavior = "destroy",
	harvest_effect = "harvest1",
	harvest_materials = { mushroom = 1 }}
