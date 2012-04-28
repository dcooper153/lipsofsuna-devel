Program:add_path(Mod.path)

Obstaclespec{
	name = "dewspring",
	model = "dewspring1",
	categories = {["small-plant"] = true, ["reagent"] = true},
	health = 10,
	harvest_behavior = "destroy",
	harvest_effect = "harvest1",
	harvest_materials = {["dewspring leaf"] = 1}}

Itemspec{
	name = "dewspring leaf",
	model = "dewspringleaf1",
	icon = "mushroom1", -- FIXME
	categories = {["material"] = true},
	mass = 1,
	mass_inventory = 0.1,
	stacking = true}
