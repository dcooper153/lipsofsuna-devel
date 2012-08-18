Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "barrel",
	effect = "craftwood1",
	materials = {["log"] = 7},
	modes = {["wood"] = true}}

Itemspec{
	name = "barrel",
	model = "barrel1",
	icon = "potion3", -- FIXME
	mass = 9,
	categories = {["container"] = true, ["generate"] = true},
	important = true,
	inventory_size = 6,
	inventory_type = "chest",
	description = "Round, wooden container for various things",
	loot_categories = {"material", "misc", "potion"},
	loot_count_min = 1,
	loot_count_max = 6,
	stacking = false,
	usages = {["loot"] = true}}
