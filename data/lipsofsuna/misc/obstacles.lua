local EffectSpec = require("core/specs/effect")
local ObstacleSpec = require("core/specs/obstacle")

------------------------------------------------------------------------------
-- Quest obstacles

ObstacleSpec{
	name = "wanted poster",
	model = "wantedposter1",
	categories = {["special"] = true},
	usages = {["examine"] = true}}

------------------------------------------------------------------------------
-- Map objects

EffectSpec{
	name = "lamppostlight1",
	light = true,
	light_diffuse = {1,0.7,0.6,1},
	light_equation = {0.1,0.1,0.01},
	node = "#root"}

ObstacleSpec{
	name = "wooden fence",
	model = "fence1",
	categories = {["prop"] = true},
	destroy_items =
	{
		{
			["name"] = "broken wooden fence",
			["position"] = Vector(0,0,0)
		}
	},
	health = 50,
	interactive = false}

ObstacleSpec{
	name = "lamppost 1",
	model = "lamppost01",
	categories = {["prop"] = true},
	health = 100,
	special_effects = {"lamppostlight1"},
	interactive = false}
