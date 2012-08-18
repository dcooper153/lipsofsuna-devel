Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "glass ingot",
	effect = "craftmetal1",
	materials = {["sand chunk"] = 1}}

Itemspec{
	name = "glass ingot",
	categories = {["material"] = true},
	health = 10,
	icon = "ingot1",
	mass = 1.5,
	model = "ingot1",
	stacking = true}

CraftingRecipeSpec{
	name = "iron ingot",
	effect = "craftmetal1",
	materials = {["ferrostone"] = 1},
	modes = {["metal"] = true, ["default"] = true}}

Itemspec{
	name = "iron ingot",
	categories = {["build"] = true, ["material"] = true},
	icon = "ingot1",
	construct_tile = "iron1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	mass = 2,
	model = "ingot1",
	stacking = true}
