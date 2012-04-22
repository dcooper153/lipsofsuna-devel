Program:add_path(Mod.path)

Itemspec{
	name = "granite ramp",
	model = "granitestone1",
	icon = "stone1",
	categories = {"build","material"},
	crafting_materials = {["granite stone"] = 1},
	description = "Left click to build a ramp block",
	construct_tile = "concrete2",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	health = 50,
	mass = 5,
	stacking = true}

Obstaclespec{
	name = "roof 1",
	model = "roof1",
	categories = {"prop"},
	health = 100,
	interactive = false}

Obstaclespec{
	name = "rooftile 1",
	model = "rooftile1",
	categories = {"prop"},
	health = 100,
	interactive = false}

Obstaclespec{
	name = "roofside 1",
	model = "rooffill1",
	categories = {"prop"},
	health = 100,
	interactive = false}

Obstaclespec{
	name = "windowsill 1",
	model = "windowsill1",
	categories = {"prop"},
	health = 100,
	interactive = false}

Obstaclespec{
	name = "arch 1",
	model = "arch1",
	categories = {"prop"},
	health = 100,
	interactive = false}

Obstaclespec{
	name = "wooden stairs",
	model = "stairs1",
	categories = {"prop"},
	health = 50,
	interactive = false}

Obstaclespec{
	name = "iron gate",
	model = "gate01",
	categories = {"prop"},
	collision_mask = 0xFFFF,
	constraints = {{"hinge",Vector(0,1.333,0)}},
	destroy_items = {{"broken door",Vector(0,0,0)}},
	health = 100,
	interactive = false,
	physics = "rigid"}

Obstaclespec{
	name = "wooden shutter",
	model = "shutter1",
	categories = {"prop"},
	collision_mask = 0xFFFF,
	constraints = {{"hinge",Vector(0,1.333,0)}},
	destroy_items = {{"broken door",Vector(0,0,0)}},
	health = 100,
	interactive = false,
	physics = "rigid"}

Staticspec{
	name = "statichouse2",
	model = "house2"}
