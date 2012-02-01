Program:add_path(Mod.path)

Quest{name = "My Little Castle"}

Obstaclespec{
	name = "building marker",
	model = "minesupport1",--FIXME
	categories = {"special"},
	health= 100,
	dialog = "bigbuildings"}

Obstaclespec{
	name = "large wall scaffold",
	model = "wallscaffold1",--FIXME
	categories = {"special"},
	health= 50}

Obstaclespec{
	name = "small building marker",
	model = "torch1",--FIXME
	health= 100,
	categories = {"special"},
	dialog = "smallbuildings"}

Regionspec{
	name = "Castle Cave",
	categories = {"special"},
	depth = {1000,1000},
	links = {"Lips", "Sanctuary"},
	pattern_name = "castlecave1",
	position = {800,700}}

Species{
	name = "Castle Foreman",
	base = "aer",
	dialog = "castleforeman",
	marker = "castleforeman",
	ai_enable_combat = false,
	ai_enable_wander = false,
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"shirt", "pants", "leather boots"}}

require(Mod.path .. "bigbuildings")
require(Mod.path .. "smallbuildings")
require(Mod.path .. "castleapothecary")
require(Mod.path .. "castlebarracks")
require(Mod.path .. "castlecave1")
require(Mod.path .. "castlegarden")
require(Mod.path .. "castleshop")
require(Mod.path .. "smallbuildingnodes")
require(Mod.path .. "swallns")
require(Mod.path .. "walldiagr")
require(Mod.path .. "wallns")
require(Mod.path .. "walltower")