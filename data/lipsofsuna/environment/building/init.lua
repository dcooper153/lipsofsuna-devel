Program:add_path(Mod.path)

CraftingRecipeSpec{
	name = "granite ramp",
	effect = "craftmetal1",
	materials = {["granite stone"] = 1}}

Itemspec{
	name = "granite ramp",
	model = "granitestone1",
	icon = "stone1",
	categories = {["build"] = true, ["material"] = true},
	description = "Place on ground to build a ramp block.",
	construct_tile = "concrete2",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	health = 50,
	mass = 5,
	stacking = true}

Obstaclespec{
	name = "roof 1",
	model = "roof1",
	categories = {["prop"] = true},
	health = 100,
	interactive = false}

Obstaclespec{
	name = "rooftile 1",
	model = "rooftile1",
	categories = {["prop"] = true},
	health = 100,
	interactive = false}

Obstaclespec{
	name = "roofside 1",
	model = "rooffill1",
	categories = {["prop"] = true},
	health = 100,
	interactive = false}

Obstaclespec{
	name = "windowsill 1",
	model = "windowsill1",
	categories = {["prop"] = true},
	health = 100,
	interactive = false}

Obstaclespec{
	name = "arch 1",
	model = "arch1",
	categories = {["prop"] = true},
	health = 100,
	interactive = false}

Obstaclespec{
	name = "wooden stairs",
	model = "stairs1",
	categories = {["prop"] = true},
	health = 50,
	interactive = false}

Constraintspec{
	name = "irongatehinge1",
	constraint = "hinge",
	offset = Vector(0,1.333,0)}

Obstaclespec{
	name = "iron gate",
	model = "gate01",
	categories = {["prop"] = true},
	collision_mask = 0xFFFF,
	constraints = {"irongatehinge1"},
	destroy_items = {{"broken door",Vector(0,0,0)}},
	health = 100,
	interactive = false,
	physics = "rigid"}

Constraintspec{
	name = "shutterhinge1",
	constraint = "hinge",
	offset = Vector(0,1.333,0)}

Obstaclespec{
	name = "wooden shutter",
	model = "shutter1",
	categories = {["prop"] = true},
	collision_mask = 0xFFFF,
	constraints = {"shutterhinge1"},
	destroy_items = {{"broken door",Vector(0,0,0)}},
	health = 100,
	interactive = false,
	physics = "rigid"}

Staticspec{
	name = "statichouse2",
	model = "house2"}
