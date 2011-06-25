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

Obstaclespec{
	name = "dewspring",
	model = "dewspring1",
	categories = {"small-plant", "reagent"},
	health = 10,
	harvest_behavior = "destroy",
	harvest_effect = "harvest1",
	harvest_materials = {["dewspring leaf"] = 1}}

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

Obstaclespec{
	name = "bridge chest",
	model = "chest1",
	categories = {"special"},
	dialog = "bridgechest",
	marker = "bridgechest"}

------------------------------------------------------------------------------
-- Map objects

Obstaclespec{
	name = "wooden fence",
	model = "fence1",
	categories = {"prop"},
	destroy_items = {{"broken wooden fence",Vector(0,0,0)}},
	health = 50,
	interactive = false}

Obstaclespec{
	name = "wooden mine support (forked)",
	model = "minesupport1",
	categories = {"prop"},
	health = 100,
	interactive = false}

Obstaclespec{
	name = "wooden mine support (wall)",
	model = "minesupport2",
	categories = {"prop"},
	health = 100,
	interactive = false}

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
	name = "wooden mine support (simple)",
	model = "minesupport3",
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
	name = "wooden door",
	model = "door1",
	categories = {"prop"},
	collision_mask = 0xFFFF,
	constraints = {{"hinge",Vector(0,1.333,0)}},
	destroy_items = {{"broken door",Vector(0,0,0)}},
	health = 100,
	interactive = false,
	physics = "rigid"}

Obstaclespec{
	name = "closable wooden door",
	model = "door1",
	categories = {"prop"},
	collision_mask = 0xFFFF,
	constraints = {{"hinge",Vector(0,1.333,0)}},
	destroy_items = {{"broken door",Vector(0,0,0)}},
	health = 100,
	interactive = true,
	physics = "rigid"}

Obstaclespec{
	name = "lips sun",
	model = "torch1",
	categories = {"prop"},
	collision_mask = 0xFFFF,
	special_effects = {
	  {type = "light", diffuse = {1,0.7,0.6,1}, equation = {0.1,0.1,0.01}, node = "#root"}
	},
	health = 100}
