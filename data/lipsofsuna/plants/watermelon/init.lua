Program:add_path(Mod.path)

Obstaclespec{
	name = "watermelon",
	model = "watermelon2",
	categories = {["small-plant"] = true, ["reagent"] = true},
	health = 10,
	harvest_behavior = "destroy",
	harvest_effect = "harvest1",
	harvest_materials = {["watermelon"] = 1},
	usages = {["harvest"] = true}}

Itemspec{
	name = "watermelon",
	categories = {["food"] = true},
	icon = "mushroom1", -- FIXME
	mass = 1,
	model = "watermelon1",
	stacking = true,
	usages = {["eat"] = true}}
