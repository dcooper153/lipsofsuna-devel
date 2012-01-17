Program:add_path(Mod.path)

Quest{name = "Keepers of Knowledge"}

-- The guardians of the sealed library.
Faction{
	name = "sealed library",
	enemies = {"good"}}

-- The staff of the great library and the enforcer of the sealed library
Faction{
	name = "keepers of knowledge",
	enemies = {}}

Itemspec{
	name = "security robot arm",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	effect_attack = "musket1",
	influences_base = {physical = -30},
	equipment_slot = "robotarm.R",
	mass = 15}

Itemspec{
	name = "security turret barrel",
	model = "musket1",
	icon = "musket1",
	ammo_type = "bullet",
	categories = {"ranged", "weapon"},
	effect_attack = "musket1",
	influences_base = {physical = -15},
	equipment_slot = "turretbarrel",
	mass = 15}

Obstaclespec{
	name = "great library steam lever",
	model = "torch1",
	categories = {"special"},
	dialog = "greatlibrarysteamlever",
	marker = "great library"}

Species{
	name = "library security enforcer",
	base = "base",
	model = "barrel1",
	dialog = "librarysecurityenforcer",
	marker = "sealed library",
	ai_enable_wander = false,
	ai_offsense_factor = 1,
	aim_ray_start = 0.7,
	factions = {"keepers of knowledge"},
	feat_anims = {"ranged"},
	feat_effects = {"physical damage"},
	inventory_size = 4,
	inventory_items = {["bullet"] = 100, "security robot arm"},
	equipment_slots = {
		{name = "robotarm.L", node = "#hand.L"},
		{name = "robotarm.R", node = "#hand.R"}},
	skills = {
		{name = "dexterity", max = 10, val = 10},
		{name = "health", max = 100, val = 100},
		{name = "intelligence", max = 10, val = 10},
		{name = "perception", max = 30, val = 30},
		{name = "strength", max = 100, val = 100},
		{name = "willpower", max = 100, val = 100}},
	weapon_slot = "robotarm.R"}

Species{
	name = "library security robot",
	base = "base",
	model = "barrel1",
	factions = {"sealed library"},
	feat_anims = {"ranged"},
	feat_effects = {"physical damage"},
	aim_ray_start = 0.7,
	inventory_size = 4,
	inventory_items = {["bullet"] = 100, "security robot arm"},
	equipment_slots = {
		{name = "robotarm.L", node = "#hand.L"},
		{name = "robotarm.R", node = "#hand.R"}},
	skills = {
		{name = "dexterity", max = 10, val = 10},
		{name = "health", max = 100, val = 100},
		{name = "intelligence", max = 10, val = 10},
		{name = "perception", max = 30, val = 30},
		{name = "strength", max = 100, val = 100},
		{name = "willpower", max = 100, val = 100}},
	weapon_slot = "robotarm.R"}

Species{
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
	factions = {"sealed library"},
	feat_anims = {"ranged"},
	feat_effects = {"physical damage"},
	inventory_size = 2,
	inventory_items = {["bullet"] = 100, "security turret barrel"},
	equipment_slots = {
		{name = "turretbarrel", node = "#barrel"}},
	skills = {
		{name = "dexterity", max = 10, val = 10},
		{name = "health", max = 100, val = 100},
		{name = "intelligence", max = 10, val = 10},
		{name = "perception", max = 30, val = 30},
		{name = "strength", max = 100, val = 100},
		{name = "willpower", max = 100, val = 100}},
	weapon_slot = "turretbarrel"}

Regionspec{
	name = "Great Library",
	categories = {"special"},
	depth = {930,1000},
	distance = {"Lips",30,60},
	pattern_name = "greatlibrary1"}

Regionspec{
	name = "Sealed Library",
	categories = {"special"},
	depth = {930,1000},
	distance = {"Lips",30,60},
	pattern_name = "sealedlibrary1"}

require(Mod.path .. "greatlibrary1")
require(Mod.path .. "sealedlibrary1")
require(Mod.path .. "greatlibrarysteamlever")
require(Mod.path .. "librarysecurityenforcer")
require(Mod.path .. "sealedlibrarybook")
require(Mod.path .. "thesealedlibrary")
