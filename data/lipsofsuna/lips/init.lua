Program:add_path(Mod.path)

Quest{name = "Elise's Sorrow"}
Quest{name = "Imaginary Friend"}
Quest{name = "Sword on the Stone"}
Quest{name = "Mining Guild"}

Species{
	name = "lipscitizen",
	base = "aer",
	dialog = "lips citizen",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"leather top", "leather pants", "leather leggings"}}

Species{
	name = "Lips guard archer",
	base = "aer",
	dialog = "lips guard",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"iron greaves", "iron breastplate", "iron gauntlets", "crimson crossbow", ["arrow"] = 100},
	skills = {
		{name = "dexterity", max = 80, val = 80},
		{name = "health", max = 100, val = 70},
		{name = "intelligence", max = 100, val = 0},
		{name = "perception", max = 60, val = 20},
		{name = "strength", max = 40, val = 20},
		{name = "willpower", max = 80, val = 0}}}

Species{
	name = "Lips guard knight",
	base = "aer",
	dialog = "lips guard",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"iron greaves", "iron breastplate", "iron gauntlets", "crimson sword", "round shield"},
	skills = {
		{name = "dexterity", max = 80, val = 60},
		{name = "health", max = 100, val = 70},
		{name = "intelligence", max = 100, val = 0},
		{name = "perception", max = 60, val = 20},
		{name = "strength", max = 40, val = 40},
		{name = "willpower", max = 80, val = 0}}}

Species{
	name = "Celine",
	base = "aer",
	dialog = "celine",
	marker = "celine",
	ai_enable_combat = false,
	ai_enable_wander = false,
	hair_style = {"hair1",0,255,0},
	inventory_items = {"iron hatchet", "dress"},
	factions = {"peculiar pet"}}

Species{
	name = "Elise",
	base = "aer",
	animations = {
		["idle"] = {animation = "tired", channel = 1, permanent = true}},
	dialog = "elise",
	marker = "elise",
	ai_enable_combat = false,
	ai_enable_wander = false,
	hair_style = {"hair2",200,180,20},
	inventory_items = {"iron hatchet", "dress"},
	factions = {"neutral"}}

Species{
	name = "Merchant",
	base = "aer",
	ai_enable_combat = false,
	ai_enable_wander = false,
	dialog = "merchant",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"}}

require(Mod.path .. "lips1")
require(Mod.path .. "lipsguard")
require(Mod.path .. "lipscitizen")
require(Mod.path .. "mentor")
require(Mod.path .. "roland")
require(Mod.path .. "celine")
require(Mod.path .. "elise")
require(Mod.path .. "col")
require(Mod.path .. "merchant")
require(Mod.path .. "guildmastersam")
