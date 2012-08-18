Program:add_path(Mod.path)

Obstaclespec{
	name = "workbench",
	categories = {["furniture"] = true},
	collision_mask = 0xFFFF,
	mass = 100,
	model = "workbench2",
	physics = "static",
	usages = {["craft wood"] = true}}

Obstaclespec{
	name = "alchemy table",
	categories = {["furniture"] = true},
	collision_mask = 0xFFFF,
	mass = 100,
	model = "workbench2",--FIXME
	physics = "static",
	usages = {["craft alchemy"] = true}}

Obstaclespec{
	name = "anvil",
	categories = {["furniture"] = true},
	collision_mask = 0xFFFF,
	mass = 100,
	model = "workbench2",--FIXME
	physics = "static",
	usages = {["craft metal"] = true}}
