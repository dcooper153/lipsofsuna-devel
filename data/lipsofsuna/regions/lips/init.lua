Program:add_path(Mod.path)

Quest{name = "Elise's Sorrow"}
Quest{name = "Imaginary Friend"}
Quest{name = "Sword on the Stone"}
Quest{name = "Mining Guild"}

Faction{name = "peculiar pet"}

Species{
	name = "lipscitizen",
	base = "aer",
	dialog = "lips citizen",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"leather top", "leather pants", "leather leggings"},
	ai_type = "camper"}

Species{
	name = "Lips guard archer",
	base = "aer",
	dialog = "lips guard",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"iron greaves", "iron breastplate", "iron gauntlets", "crimson crossbow", ["arrow"] = 100},
	skills = {"Health lv2"},
	ai_type = "camper"}

Species{
	name = "Lips guard knight",
	base = "aer",
	dialog = "lips guard",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"iron greaves", "iron breastplate", "iron gauntlets", "crimson sword", "round shield"},
	skills = {"Health lv4"},
	ai_type = "camper"}

Species{
	name = "Celine",
	base = "aer",
	dialog = "celine",
	marker = "celine",
	hair_style = {"hair1",0,255,0},
	inventory_items = {"iron hatchet", "dress"},
	factions = {"peculiar pet"},
	ai_type = "camper"}

Species{
	name = "Elise",
	base = "aer",
	animations = {
		["idle"] = {animation = "tired", channel = 1, permanent = true}},
	dialog = "elise",
	marker = "elise",
	hair_style = {"hair2",200,180,20},
	inventory_items = {"iron hatchet", "dress"},
	factions = {"neutral"},
	ai_type = "anchored"}

Species{
	name = "Merchant",
	base = "aer",
	dialog = "merchant",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"},
	ai_type = "anchored"}

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
