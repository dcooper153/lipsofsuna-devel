------------------------------------------------------------------------------
-- Quest obstacles

Obstaclespec{
	name = "wanted poster",
	model = "wantedposter1",
	categories = {["special"] = true},
	dialog = "castlewanted"}

Obstaclespec{
	name = "sanctuary device",
	model = "torch1",
	categories = {["special"] = true},
	dialog = "sanctuary",
	marker = "sanctuary"}

Obstaclespec{
	name = "bridge chest",
	model = "chest1",
	categories = {["special"] = true},
	dialog = "bridgechest",
	marker = "bridgechest"}

------------------------------------------------------------------------------
-- Map objects

Effectspec{
	name = "lamppostlight1",
	light = true,
	light_diffuse = {1,0.7,0.6,1},
	light_equation = {0.1,0.1,0.01},
	node = "#root"}

Effectspec{
	name = "lipssunlight1",
	light = true,
	light_diffuse = {1,0.7,0.6,1},
	light_equation = {0.5,0.5,0.5},
	node = "#root"}

Obstaclespec{
	name = "wooden fence",
	model = "fence1",
	categories = {["prop"] = true},
	destroy_items = {{"broken wooden fence",Vector(0,0,0)}},
	health = 50,
	interactive = false}

Obstaclespec{
	name = "lamppost 1",
	model = "lamppost01",
	categories = {["prop"] = true},
	health = 100,
	special_effects = {"lamppostlight1"},
	interactive = false}

Obstaclespec{
	name = "lips sun",
	model = "torch1",
	categories = {["prop"] = true},
	collision_mask = 0xFFFF,
	special_effects = {"lipssunlight1"},
	health = 100}
