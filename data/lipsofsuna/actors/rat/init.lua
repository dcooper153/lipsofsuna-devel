Program:add_path(Mod.path)

Species{
	name = "small rat",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = .1,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	-- categories = {"enemy"},
	difficulty = 0,
	effect_physical_damage = "fleshimpact1",
	factions = {"neutral"},
	feat_anims = {"bite"},
	feat_effects = {"physical damage"},
	inventory_size = 3,
	inventory_items = {"hide"},
	jump_force = 6,
	model = "smallrat1",
	skills = {"Nerf health lv2", "Nerf combat lv2"},
	speed_walk = 4}

Species{
	name = "rat",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = 1,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"enemy"},
	difficulty = 0,
	effect_physical_damage = "fleshimpact1",
	factions = {"evil"},
	feat_anims = {"bite"},
	feat_effects = {"physical damage"},
	inventory_size = 3,
	inventory_items = {["hide"] = 2},
	jump_force = 6,
	model = "plaguerat1", -- FIXME
	skills = {"Nerf health lv1", "Nerf combat lv2"},
	speed_walk = 4}

Species{
	name = "plague rat",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = 1,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"plague"},
	difficulty = 0.7,
	effect_physical_damage = "fleshimpact1",
	factions = {"evil"},
	feat_anims = {"bite"},
	feat_effects = {"physical damage"},
	inventory_size = 3,
	inventory_items = {["hide"] = 2},
	jump_force = 6,
	model = "plaguerat1",
	skills = {"Health lv2", "Nerf combat lv1"},
	speed_walk = 4}
