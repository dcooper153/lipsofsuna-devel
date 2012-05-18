Program:add_path(Mod.path)

Itemspec{
	name = "glass ingot",
	model = "ingot1",
	icon = "ingot1",
	categories = {["material"] = true},
	crafting_materials = {["sand chunk"] = 1},
	health = 10,
	mass = 1.5,
	stacking = true}

Itemspec{
	name = "iron ingot",
	model = "ingot1",
	icon = "ingot1",
	mass = 2,
	categories = {["build"] = true, ["material"] = true},
	construct_tile = "iron1",
	construct_tile_count = 1,
	crafting_materials = {["ferrostone"] = 1},
	equipment_slot = "hand.R",
	stacking = true}
