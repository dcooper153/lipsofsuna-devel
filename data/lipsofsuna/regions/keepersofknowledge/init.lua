Program:add_path(Mod.path)

Questspec{
	name = "Keepers of Knowledge"}

-- The guardians of the sealed library.
Factionspec{
	name = "sealed library",
	enemies = {["good"] = true}}

-- The staff of the great library and the enforcer of the sealed library
Factionspec{
	name = "keepers of knowledge"}

Itemspec{
	name = "security robot arm",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	categories = {["ranged"] = true, ["weapon"] = true},
	effect_attack = "musket1",
	influences = {["physical damage"] = 30},
	equipment_slot = "robotarm.R",
	mass = 15}

Itemspec{
	name = "security turret barrel",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	categories = {["ranged"] = true, ["weapon"] = true},
	effect_attack = "musket1",
	influences = {["physical damage"] = 15},
	equipment_slot = "turretbarrel",
	mass = 15}

Obstaclespec{
	name = "great library steam lever",
	model = "torch1",
	categories = {["special"] = true},
	dialog = "greatlibrarysteamlever",
	marker = "great library"}

Actorspec{
	name = "library security enforcer",
	base = "base",
	ai_offense_factor = 1,
	ai_type = "anchored",
	aim_ray_start = 0.7,
	dialog = "librarysecurityenforcer",
	equipment_slots = {["robotarm.L"] = "#hand.L", ["robotarm.R"] = "#hand.R"},
	factions = {["keepers of knowledge"] = true},
	feat_types = {["ranged"] = true},
	feat_effects = {["physical damage"] = true},
	inventory_size = 4,
	inventory_items = {["bullet"] = 100, ["security robot arm"] = 1},
	important = true,
	marker = "sealed library",
	model = "barrel1",
	skills = {["Health lv4"] = true, ["Willpower lv4"] = true},
	weapon_slot = "robotarm.R"}

Actorspec{
	name = "library security robot",
	base = "base",
	model = "barrel1",
	aim_ray_start = 0.7,
	equipment_slots = {["robotarm.L"] = "#hand.L", ["robotarm.R"] = "#hand.R"},
	factions = {["sealed library"] = true},
	feat_types = {["ranged"] = true},
	feat_effects = {["physical damage"] = true},
	important = true,
	inventory_size = 4,
	inventory_items = {["bullet"] = 100, ["security robot arm"] = 1},
	skills = {["Health lv4"] = true, ["Willpower lv4"] = true},
	weapon_slot = "robotarm.R"}

Actorspec{
	name = "library security turret",
	base = "base",
	model = "barrel1",
	ai_enable_backstep = false,
	ai_enable_block = false,
	ai_enable_jump = false,
	ai_enable_strafe = false,
	ai_enable_wander = false,
	ai_enable_walk = false,
	ai_enable_weapon_switch = false,
	aim_ray_start = 0.7,
	-- TODO: Should have no gravity, shouldn't be pushable
	factions = {["sealed library"] = true},
	feat_types = {["ranged"] = true},
	feat_effects = {["physical damage"] = true},
	important = true,
	inventory_size = 2,
	inventory_items = {["bullet"] = 100, ["security turret barrel"] = 1},
	equipment_slots = {["turretbarrel"] = "#barrel"},
	skills = {["Health lv4"] = true, ["Willpower lv4"] = true},
	weapon_slot = "turretbarrel"}

require(Mod.path .. "greatlibrary1")
require(Mod.path .. "sealedlibrary1")
require(Mod.path .. "greatlibrarysteamlever")
require(Mod.path .. "librarysecurityenforcer")
require(Mod.path .. "sealedlibrarybook")
require(Mod.path .. "thesealedlibrary")
