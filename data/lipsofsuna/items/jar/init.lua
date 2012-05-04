Program:add_path(Mod.path)

Itemspec{
	name = "jar",
	model = "jar1",
	icon = "potion3", -- FIXME
	mass = 9,
	categories = {["container"] = true, ["generate"] = true},
	crafting_materials = {["granite stone"] = 3},
	description = "Small container, used for storing things",
	inventory_size = 4,
	inventory_type = "chest",
	loot_categories = {"material", "misc", "potion"},
	loot_count_max = 4,
	stacking = false,
	usages = {["loot"] = true}}

Itemspec{
	name = "health jar",
	model = "jar1",
	icon = "potion3", -- FIXME
	mass = 5,
	categories = {["container"] = true},
	description = "Small container, used for storing things",
	inventory_size = 40,
	inventory_type = "chest",
	inventory_items = {["health potion"] = 20},
	stacking = false,
	usages = {["loot"] = true}}

Itemspec{
	name = "magic jar",
	model = "jar1",
	icon = "potion3", -- FIXME
	mass = 5,
	categories = {["container"] = true},
	description = "Small container, used for storing things",
	inventory_size = 40,
	inventory_type = "chest",
	inventory_items = {["mana potion"] = 20, ["milky powder"] = 100},
	stacking = false,
	usages = {["loot"] = true}}
