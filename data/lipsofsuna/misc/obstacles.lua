------------------------------------------------------------------------------
-- Quest obstacles

Obstaclespec{
	name = "wanted poster",
	model = "wantedposter1",
	categories = {"special"},
	dialog = "castlewanted"}

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
	name = "lamppost 1",
	model = "lamppost01",
	categories = {"prop"},
	health = 100,
	special_effects = {
	  {type = "light", diffuse = {1,0.7,0.6,1}, equation = {0.5,0.5,0.5}, node = "#root"}
	},
	interactive = false}

Obstaclespec{
	name = "lips sun",
	model = "torch1",
	categories = {"prop"},
	collision_mask = 0xFFFF,
	special_effects = {
	  {type = "light", diffuse = {1,0.7,0.6,1}, equation = {0.1,0.1,0.01}, node = "#root"}
	},
	health = 100}
