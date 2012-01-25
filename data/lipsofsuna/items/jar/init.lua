Program:add_path(Mod.path)

Itemspec{
	name = "jar",
	model = "jar1",
	icon = "potion3", -- FIXME
	mass = 9,
	action_use = "loot",
	categories = {"container", "generate"},
	crafting_materials = {["granite stone"] = 3},
	description = "Small container, used for storing things",
	inventory_size = 4,
	inventory_type = "chest",
	loot_categories = {"material", "misc", "potion"},
	loot_count = {0, 4},
	stacking = false}

Itemspec{
	name = "health jar",
	model = "jar1",
	icon = "potion3", -- FIXME
	mass = 5,
	action_use = "loot",
	categories = {"container"},
	description = "Small container, used for storing things",
	inventory_size = 40,
	inventory_type = "chest",
        inventory_items = {"health potion", "health potion", "health potion", "health potion", "health potion", "health potion", "health potion", "health potion", "health potion", "health potion", "health potion", "health potion", "health potion", "health potion", "health potion", "health potion"},
	stacking = false}

Itemspec{
	name = "magic jar",
	model = "jar1",
	icon = "potion3", -- FIXME
	mass = 5,
	action_use = "loot",
	categories = {"container"},
	description = "Small container, used for storing things",
	inventory_size = 40,
	inventory_type = "chest",
        inventory_items = {"mana potion", "mana potion", "mana potion", "mana potion", "mana potion", "mana potion", "mana potion", "mana potion", "mana potion", "mana potion", "mana potion", "mana potion", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder", "milky powder"},
	stacking = false}
