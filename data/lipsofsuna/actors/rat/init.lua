Program:add_path(Mod.path)

Actorspec{
	name = "small rat",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = .1,
	animations = {["attack bite"] = "rat attack bite"},
	categories = {["animal"] = true},
	difficulty = 0,
	effect_physical_damage = "fleshimpact1",
	factions = {["neutral"] = true},
	feat_anims = {["bite"] = true},
	feat_effects = {["physical damage"] = true},
	inventory_size = 3,
	inventory_items = {["hide"] = 1},
	jump_force = 6,
	model = "smallrat1",
	skills = {["Nerf health lv2"] = true, ["Nerf combat lv2"] = true},
	speed_walk = 4}

Actorspec{
	name = "rat",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = 1,
	animations = {["attack bite"] = "rat attack bite"},
	categories = {["enemy"] = true},
	difficulty = 0,
	effect_physical_damage = "fleshimpact1",
	factions = {["evil"] = true},
	feat_anims = {["bite"] = true},
	feat_effects = {["physical damage"] = true},
	inventory_size = 3,
	inventory_items = {["hide"] = 2},
	jump_force = 6,
	model = "plaguerat1", -- FIXME
	skills = {["Nerf health lv1"] = true, ["Nerf combat lv2"] = true},
	speed_walk = 4}

Actorspec{
	name = "plague rat",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = 1,
	animations = {["attack bite"] = "rat attack bite"},
	categories = {["plague"] = true},
	difficulty = 0.7,
	effect_physical_damage = "fleshimpact1",
	factions = {["evil"] = true},
	feat_anims = {["bite"] = true},
	feat_effects = {["physical damage"] = true},
	inventory_size = 3,
	inventory_items = {["hide"] = 2},
	jump_force = 6,
	model = "plaguerat1",
	skills = {["Health lv2"] = true, ["Nerf combat lv1"] = true},
	speed_walk = 4}
