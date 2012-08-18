Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "workbench",
	effect = "craftwood1",
	materials = {["log"] = 7},
	modes = {["wood"] = true}}

Itemspec{
	name = "workbench",
	categories = {["container"] = true, ["workbench"] = true},
	icon = "workbench1",
	important = true,
	inventory_size = 4,
	inventory_type = "workbench",
	mass = 21,
	model = "workbench1",
	stacking = false,
	usages = {["loot"] = true},
	water_gravity = Vector(0,6,0)}
