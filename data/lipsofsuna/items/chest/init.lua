Program:add_path(Mod.path)

Itemspec{
	name = "chest",
	model = "chest1",
	icon = "chest1",
	mass = 30,
	animations = {["loot"] = "chest loot", ["looted"] = "chest looted"},
	categories = {["container"] = true, ["generate"] = true},
	crafting_materials = {log = 10},
	effect_craft = "craftwood1",
	important = true,
	inventory_size = 8,
	inventory_type = "chest",
	loot_categories = {"armor", "book", "material", "misc", "potion", "shield", "weapon"},
	loot_count_min = 2,
	loot_count_max = 4,
	stacking = false,
	usages = {["loot"] = true},
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "treasure chest",
	model = "chest1",
	icon = "chest1",
	mass = 30,
	animations = {["loot"] = "chest loot", ["looted"] = "chest looted"},
	categories = {["container"] = true},
	crafting_materials = {["iron ingot"] = 10},
	effect_craft = "craftwood1",
	important = true,
	inventory_size = 8,
	inventory_type = "chest",
	loot_categories = {"armor", "shield", "weapon"},
	loot_count_min = 8,
	loot_count_max = 8,
	stacking = false,
	usages = {["loot"] = true},
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "weapons",
	model = "chest1",
	icon = "chest1",
	mass = 30,
	animations = {["loot"] = "chest loot", ["looted"] = "chest looted"},
	categories = {["container"] = true},
	important = true,
	inventory_size = 20,
	inventory_type = "chest",
	inventory_items = {["wooden sword"] = 1, ["wooden hatchet"] = 1, ["wooden mattock"] = 1, ["wooden bow"] = 1, ["wooden crossbow"] = 1, ["log"] = 3, ["iron musket"] = 1, ["iron revolver"] = 1, ["bullet"] = 100},
	stacking = false,
	usages = {["loot"] = true},
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "logs",
	model = "chest1",
	icon = "chest1",
	mass = 30,
	animations = {["loot"] = "chest loot", ["looted"] = "chest looted"},
	categories = {["container"] = true},
	important = true,
	inventory_size = 50,
	inventory_type = "chest",
	inventory_items = {["log"] = 50},
	stacking = false,
	usages = {["loot"] = true},
	water_gravity = Vector(0,6,0)}
