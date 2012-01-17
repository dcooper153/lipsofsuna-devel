require "content/species"

Species{
	name = "Agathe",
	base = "aer",
	dialog = "agathe",
	marker = "agathe",
	ai_enable_combat = false,
	ai_enable_wander = false,
	factions = {"silverspring"},
	hair_style = {"hair1",0,255,192},
	inventory_items = {"dress"}}

Species{
	name = "Chara",
	base = "seirei",
	dialog = "chara",
	marker = "chara"}

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
	name = "Erinyes",
	base = "seirei",
	dialog = "erinyes",
	marker = "erinyes"}

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

Species{
	name = "Martin",
	base = "aermale",
	dialog = "martin",
	marker = "martin",
	dead = true,
	eye_style = {"default",0,0,0},
	hair_style = {"aermale1hair1",128,128,128},
	inventory_items = {"pants"}}

Species{
	name = "Maurice",
	base = "aermale",
	dialog = "maurice",
	marker = "maurice",
	ai_enable_combat = false,
	ai_enable_wander = false,
	eye_style = {"default",64,192,64},
	factions = {"silverspring"},
	hair_style = {"bald",0,0,0},
	inventory_items = {"shirt", "pants"}}

Species{
	name = "Mentor",
	base = "aermale",
	dialog = "mentor",
	marker = "mentor",
	ai_enable_combat = false,
	ai_enable_wander = false,
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"shirt", "pants", "leather boots"}}

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

Species{
	name = "Merchant",
	base = "aer",
	ai_enable_combat = false,
	ai_enable_wander = false,
	dialog = "merchant",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"}}

Species{
	name = "Foreman",
	base = "aer",
	ai_enable_combat = false,
	ai_enable_wander = false,
	dialog = "foreman",
	marker = "foreman",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"}}

Species{
	name = "Expedition Leader Lyra",
	base = "aer",
	ai_enable_combat = false,
	ai_enable_wander = false,
	dialog = "lyra",
	marker = "lyra",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"}}

Species{
	name = "Drill Sergent Spud",
	base = "devora",
	ai_enable_combat = false,
	ai_enable_wander = false,
	dialog = "spud",
	marker = "spud",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"lizard dress","torch"}}

Species{
	name = "Dummy",
	base = "devora",
	ai_enable_combat = false,
	ai_enable_wander = false,
	dialog = "dummy",
	marker = "dummy",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"combat token"}}

Species{
	name = "Fighting Instructor Fa",
	base = "wyrm",
	ai_enable_combat = false,
	ai_enable_wander = false,
	dialog = "fa",
	marker = "fa",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"iron breastplate", "iron greaves", "iron gauntlets","wooden staff"}}

Species{
	name = "Craftress Xsphin",
	base = "kraken",
	ai_enable_combat = false,
	ai_enable_wander = false,
	dialog = "xsphin",
	marker = "xsphin",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"}}

Species{
	name = "Zyra the Mage",
	base = "aer",
	ai_enable_combat = false,
	ai_enable_wander = false,
	dialog = "zyra",
	marker = "zyra",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"}}

Species{
	name = "Col",
	base = "aermale",
	dialog = "col",
	marker = "col",
	ai_enable_combat = false,
	ai_enable_wander = false,
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"shirt", "pants", "leather boots"}}

Species{
	name = "Guildmaster Sam",
	base = "aermale",
	dialog = "guildmastersam",
	marker = "guildmastersam",
	ai_enable_combat = false,
	ai_enable_wander = false,
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"shirt", "pants", "leather boots","adamantium mattock"}}


Species{
	name = "Noemi",
	base = "aer",
	dialog = "noemi",
	marker = "noemi",
	dead = true,
	hair_style = {"hair2",200,180,20},
	inventory_items = {"iron hatchet", "dress"},
	factions = {"good"}}

Species{
	name = "Roland",
	base = "aermale",
	dialog = "roland",
	marker = "roland",
	ai_enable_combat = false,
	ai_enable_wander = false,
	eye_style = {"default",255,0,0},
	factions = {"neutral"},
	hair_style = {"aermale1hair1",128,128,128},
	inventory_items = {"shirt", "pants", "leather boots"}}

Species{
	name = "The Hoarder",
	base = "aermale",
	dialog = "the hoarder",
	marker = "the hoarder",
	eye_style = {"default",255,32,32},
	factions = {"the hoarder"},
	hair_style = {"aermale1hair1",200,20,180},
	inventory_items = {"shirt", "pants", "leather boots"}}
