Program:add_path(Mod.path)

Quest{name = "Elise's Sorrow"}
Quest{name = "Imaginary Friend"}
Quest{name = "Sword on the Stone"}
Quest{name = "Mining Guild"}

Faction{name = "peculiar pet"}

Regionspec{
	name = "Lips",
	categories = {"special"},
	depth = {1000,1000},
	links = {"Portal of Lips", "Sanctuary", "Chara's Root Grove"},
	pattern_name = "lips1",
	position = {700,700},
	spawn_point = {12,5,12}}

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
	skills = {"Health lv2"}}

Species{
	name = "Lips guard knight",
	base = "aer",
	dialog = "lips guard",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"iron greaves", "iron breastplate", "iron gauntlets", "crimson sword", "round shield"},
	skills = {"Health lv4"}}

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
