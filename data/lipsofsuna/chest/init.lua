Program:add_path(Mod.path)

Itemspec{
	name = "chest",
	model = "chest1",
	icon = "chest1",
	mass = 30,
	action_use = "loot",
	animations = {
		["loot"] = {animation = "opening", channel = 1, permanent = true, repeat_start = 10},
		["looted"] = {animation = "open", channel = 1, permanent = true, repeat_start = 10, time = 10}},
	categories = {"container", "generate"},
	crafting_materials = {log = 10},
	effect_craft = "craftwood1",
	inventory_size = 8,
	inventory_type = "chest",
	loot_categories = {"armor", "book", "material", "misc", "potion", "shield", "weapon"},
	loot_count = {2, 4},
	stacking = false,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "weapons",
	model = "chest1",
	icon = "chest1",
	mass = 30,
	action_use = "loot",
	animations = {
		["loot"] = {animation = "opening", channel = 1, permanent = true, repeat_start = 10},
		["looted"] = {animation = "open", channel = 1, permanent = true, repeat_start = 10, time = 10}},
	categories = {"container"},
	inventory_size = 20,
	inventory_type = "chest",
	inventory_items = {"wooden sword", "wooden hatchet", "wooden mattock", "wooden bow", "wooden crossbow", "log", "log", "log", "iron musket", "iron revolver", "bullet", "bullet", "bullet", "bullet", "bullet", "bullet", "bullet", "bullet", "bullet", "bullet", "bullet", "bullet", "bullet", "bullet", "bullet"},
	stacking = false,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "logs",
	model = "chest1",
	icon = "chest1",
	mass = 30,
	action_use = "loot",
	animations = {
		["loot"] = {animation = "opening", channel = 1, permanent = true, repeat_start = 10},
		["looted"] = {animation = "open", channel = 1, permanent = true, repeat_start = 10, time = 10}},
	categories = {"container"},
	inventory_size = 50,
	inventory_type = "chest",
	inventory_items = {"log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log", "log" },
	stacking = false,
	water_gravity = Vector(0,6,0)}
