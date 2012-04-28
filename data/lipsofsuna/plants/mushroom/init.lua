Program:add_path(Mod.path)

Itemspec{
	name = "mushroom",
	model = "mushroom-000",--"mushroom-001"..."mushroom-008"
	icon = "mushroom1",
	description = "An ingredient for magical reagents",
	categories = {["material"] = true},
	mass = 1,
	stacking = true}

Obstaclespec{
	name = "mushroom",
	model = "mushroom-000",
	categories = {["small-plant"] = true, ["reagent"] = true},
	health = 10,
	harvest_behavior = "destroy",
	harvest_effect = "harvest1",
	harvest_materials = { mushroom = 1 }}
