Program:add_path(Mod.path)

Obstaclespec{
	name = "watermelon",
	model = "watermelon2",
	categories = {["small-plant"] = true, ["reagent"] = true},
	health = 10,
	harvest_behavior = "destroy",
	harvest_effect = "harvest1",
	harvest_materials = {["dewspring leaf"] = 1},
	usages = {["harvest"] = true}}

Itemspec{
	name = "watermelon",
	model = "watermelon1",
	icon = "mushroom1", -- FIXME
	categories = {["food"] = true},
	mass = 1,
	stacking = true}
