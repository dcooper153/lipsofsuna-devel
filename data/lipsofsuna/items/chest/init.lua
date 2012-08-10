Program:add_path(Mod.path)

Animationspec{
	name = "chest loot",
	animation = "chest1-opening",
	channel = 1,
	permanent = true,
	repeat_start = 10}

Animationspec{
	name = "chest looted",
	animation = "chest1-opening",
	channel = 1,
	permanent = true,
	repeat_start = 10,
	time = 10}

AnimationProfileSpec{
	name = "chest",
	animations = {
		["loot"] = "chest loot",
		["looted"] = "chest looted"}}

Itemspec{
	name = "chest",
	categories = {["container"] = true, ["generate"] = true},
	animations = {["default"] = "chest"},
	model = "chest1",
	icon = "chest1",
	mass = 30,
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
	categories = {["container"] = true},
	animations = {["default"] = "chest"},
	crafting_materials = {["iron ingot"] = 10},
	effect_craft = "craftwood1",
	icon = "chest1",
	mass = 30,
	model = "chest1",
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
	categories = {["container"] = true},
	animations = {["default"] = "chest"},
	icon = "chest1",
	important = true,
	inventory_size = 20,
	inventory_type = "chest",
	inventory_items = {["wooden sword"] = 1, ["wooden hatchet"] = 1, ["wooden mattock"] = 1, ["wooden bow"] = 1, ["wooden crossbow"] = 1, ["log"] = 3, ["iron musket"] = 1, ["iron revolver"] = 1, ["bullet"] = 100},
	mass = 30,
	model = "chest1",
	stacking = false,
	usages = {["loot"] = true},
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "logs",
	categories = {["container"] = true},
	animations = {["default"] = "chest"},
	icon = "chest1",
	important = true,
	inventory_size = 50,
	inventory_type = "chest",
	inventory_items = {["log"] = 50},
	mass = 30,
	model = "chest1",
	stacking = false,
	usages = {["loot"] = true},
	water_gravity = Vector(0,6,0)}
