Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "air tank",
	effect = "craftmetal1",
	materials = {["iron ingot"] = 2},
	modes = {["metal"] = true}}

Itemspec{
	name = "air tank",
	model = "airtank1",
	mass = 15,
	categories = {["explosive"] = true},
	destroy_actions = {"explode"},
	health = 1,
	stacking = false,
	water_gravity = Vector(0,6,0)}
