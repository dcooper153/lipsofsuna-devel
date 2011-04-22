------------------------------------------------------------------------------
-- Plants

Obstaclespec{
	name = "tree",
	model = "tree1",
	categories = {"tree", "wilderness"},
	destroy_items = {
		{"log",Vector(0,0,0)},
		{"log",Vector(0,1,0)},
		{"log",Vector(0,2,0)},
		{"log",Vector(0,3,0)}},
	health = 50,
	interactive = false,
	vulnerabilities = {fire = 2, hatchet = 2}}

Obstaclespec{
	name = "rust sprig tree",
	model = "tree2",
	categories = {"tree", "wilderness"},
	destroy_items = {
		{"log",Vector(0,0,0)},
		{"log",Vector(0,1,0)},
		{"log",Vector(0,2,0)},
		{"log",Vector(0,3,0)}},
	health = 50,
	interactive = false,
	vulnerabilities = {fire = 2, hatchet = 2}}

Obstaclespec{
	name = "mushroom",
	model = "mushroom-000",
	categories = {"small-plant", "reagent"},
	health = 10,
	harvest_behavior = "destroy",
	harvest_effect = "harvest1",
	harvest_materials = { mushroom = 1 }}

------------------------------------------------------------------------------
-- Portals

Obstaclespec{
	name = "portal of illusion",
	model = "portal1",
	categories = {"portal", "special"},
	dialog = "portal of illusion",
	marker = "portal of illusion"}

Obstaclespec{
	name = "erinyes' portal",
	model = "portal1",
	categories = {"portal", "special"},
	dialog = "erinyes' portal",
	marker = "erinyes' portal"}

Obstaclespec{
	name = "portal of lips",
	model = "portal1",
	categories = {"portal", "special"},
	dialog = "portal of lips",
	marker = "portal of lips"}

Obstaclespec{
	name = "portal of midguard",
	model = "portal1",
	categories = {"portal", "special"},
	dialog = "portal of midguard",
	marker = "portal of midguard"}

Obstaclespec{
	name = "chara's portal",
	model = "portal1",
	categories = {"portal", "special"},
	dialog = "chara's portal",
	marker = "chara's portal"}

------------------------------------------------------------------------------
-- Quest obstacles

Obstaclespec{
	name = "sanctuary device",
	model = "torch1",
	categories = {"special"},
	dialog = "sanctuary",
	marker = "sanctuary"}

------------------------------------------------------------------------------
-- Map objects

Obstaclespec{
	name = "wooden stairs",
	model = "stairs1",
	categories = {"prop"},
	interactive = false}

Obstaclespec{
	name = "wooden door",
	model = "door1",
	categories = {"prop"},
	collision_mask = 0xFFFF,
	constraints = {{"hinge",Vector(0,1.333,0)}},
	interactive = false,
	physics = "rigid"}
