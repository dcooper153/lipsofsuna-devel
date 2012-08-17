Program:add_path(Mod.path)

Itemspec{
	name = "log",
	model = "log1",
	icon = "log1",
	categories = {["build"] = true, ["material"] = true},
	construct_tile = "wood1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	health = 50,
	mass = 5,
	stacking = true,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "another log",
	model = "log2",
	icon = "log1",
	categories = {["build"] = true, ["material"] = true, ["throwable"] = true},
	construct_tile = "wood1",
	construct_tile_count = 1,
	equipment_slot = "hand.R",
	equipment_slots_reserved = {["hand.L"] = true},
	health = 50,
	mass = 5,
	stacking = true,
	water_gravity = Vector(0,6,0)}

Obstaclespec{
	name = "tree",
	model = "tree1",
	categories = {["tree"] = true, ["wilderness"] = true},
	destroy_items = {
		{"log",Vector(0,0,0)},
		{"log",Vector(0,1,0)},
		{"log",Vector(0,2,0)},
		{"log",Vector(0,3,0)}},
	health = 50,
	interactive = false,
	vulnerabilities = {["fire damage"] = 2, hatchet = 2}}

Obstaclespec{
	name = "rust sprig tree",
	model = "tree2",
	categories = {["tree"] = true, ["wilderness"] = true},
	destroy_items = {
		{"log",Vector(0,0,0)},
		{"log",Vector(0,1,0)},
		{"log",Vector(0,2,0)},
		{"log",Vector(0,3,0)}},
	health = 50,
	interactive = false,
	vulnerabilities = {["fire damage"] = 2, hatchet = 2}}
