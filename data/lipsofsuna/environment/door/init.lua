Program:add_path(Mod.path)

Itemspec{
	name = "broken door",
	model = "door1",
	icon = "chest1", -- FIXME
	mass = 20,
	categories = {["furniture"] = true},
	crafting_materials = {log = 7},
	effect_craft = "craftwood1",
	health = 30,
	stacking = false,
	water_gravity = Vector(0,6,0)}

Constraintspec{
	name = "doorhinge1",
	constraint = "hinge",
	offset = Vector(0,1.333,0)}

Obstaclespec{
	name = "wooden door",
	model = "door1",
	categories = {["prop"] = true},
	collision_mask = 0xFFFF,
	constraints = {"doorhinge1"},
	destroy_items = {{"broken door",Vector(0,0,0)}},
	health = 100,
	interactive = false,
	physics = "rigid"}

Obstaclespec{
	name = "closable wooden door",
	model = "door1",
	categories = {["prop"] = true},
	collision_mask = 0xFFFF,
	constraints = {"doorhinge1"},
	destroy_items = {{"broken door",Vector(0,0,0)}},
	health = 100,
	interactive = true,
	physics = "rigid"}
