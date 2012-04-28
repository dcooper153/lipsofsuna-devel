Program:add_path(Mod.path)

Itemspec{
	name = "barrel",
	model = "barrel1",
	icon = "potion3", -- FIXME
	mass = 9,
	action_use = "loot",
	categories = {["container"] = true, ["generate"] = true},
	crafting_materials = {["log"] = 7},
	inventory_size = 6,
	inventory_type = "chest",
	description = "Round, wooden container for various things",
	loot_categories = {"material", "misc", "potion"},
	loot_count_min = 1,
	loot_count_max = 6,
	stacking = false}
