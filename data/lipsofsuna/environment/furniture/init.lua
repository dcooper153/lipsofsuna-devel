Program:add_path(Mod.path)

Itemspec{
	name = "bookcase",
	model = "bookcase1",
	icon = "chest1",
	mass = 30,
	categories = {["container"] = true, ["generate"] = true},
	crafting_materials = {["log"] = 10},
	effect_craft = "craftwood1",
	important = true,
	inventory_size = 8,
	inventory_type = "chest",
	loot_categories = {"book"},
	loot_count_min = 4,
	loot_count_max = 8,
	stacking = false,
	usages = {["loot"] = true},
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "chair",
	icon = "chest1", -- FIXME
	model = "chair1",
	mass = 10,
	categories = {["furniture"] = true},
	crafting_materials = {["log"] = 5},
	effect_craft = "craftwood1",
	health = 30,
	stacking = false,
	water_gravity = Vector(0,6,0)}

Obstaclespec{
	name = "shelf",
	categories = {["furniture"] = true},
	collision_mask = 0xFFFF,
	health = 100,
	mass = 20,
	model = "shelf1",
	physics = "rigid"}

Itemspec{
	name = "wood bench",
	model = "bench1",
	icon = "chest1", -- FIXME
	mass = 15,
	categories = {["furniture"] = true},
	crafting_materials = {["log"] = 6},
	effect_craft = "craftwood1",
	health = 30,
	stacking = false,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "wood chair",
	model = "chair2",
	icon = "chest1", -- FIXME
	mass = 10,
	categories = {["furniture"] = true},
	crafting_materials = {["log"] = 5},
	effect_craft = "craftwood1",
	health = 30,
	stacking = false,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "wood table",
	model = "table1",
	icon = "chest1", -- FIXME
	mass = 20,
	categories = {["furniture"] = true},
	crafting_materials = {["log"] = 7},
	effect_craft = "craftwood1",
	health = 30,
	stacking = false,
	water_gravity = Vector(0,6,0)}
