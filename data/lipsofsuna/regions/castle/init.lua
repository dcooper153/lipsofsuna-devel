Program:add_path(Mod.path)

Questspec{
	name = "My Little Castle"}

Obstaclespec{
	name = "building marker",
	model = "minesupport1",--FIXME
	categories = {["special"] = true},
	health= 100,
	dialog = "bigbuildings"}

Obstaclespec{
	name = "large wall scaffold",
	model = "wallscaffold1",--FIXME
	categories = {["special"] = true},
	health= 50}

Obstaclespec{
	name = "small building marker",
	model = "torch1",--FIXME
	health= 100,
	categories = {["special"] = true},
	dialog = "smallbuildings"}

Actorspec{
	name = "Castle Foreman",
	base = "aer",
	dialog = "castleforeman",
	marker = "castleforeman",
	ai_enable_combat = false,
	ai_enable_wander = false,
	eye_style = "random",
	hair_style = "random",
	inventory_items = {["shirt"] = 1, ["pants"] = 1, ["leather boots"] = 1}}

require(Mod.path .. "bigbuildings")
require(Mod.path .. "smallbuildings")
require(Mod.path .. "castleapothecary")
require(Mod.path .. "castlebarracks")
require(Mod.path .. "castlecave1")
require(Mod.path .. "castleforeman")
require(Mod.path .. "castlegarden")
require(Mod.path .. "castleshop")
require(Mod.path .. "castlewanted")
require(Mod.path .. "smallbuildingnodes")
require(Mod.path .. "swallns")
require(Mod.path .. "walldiagr")
require(Mod.path .. "wallns")
require(Mod.path .. "walltower")
