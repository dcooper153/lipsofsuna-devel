Program:add_path(Mod.path)

Actorspec{
	name = "slime",
	base = "base",
	ai_enable_block = false,
	ai_offense_factor = 0.5,
	animations = {["attack bite"] = "slime attack bite"},
	categories = {["random"] = true},
	difficulty = 0,
	factions = {["evil"] = true},
	feat_anims = {["bite"] = true},
	feat_effects = {["physical damage"] = true},
	inventory_size = 0,
	jump_force = 4,
	model = "slime2",
	speed_walk = 2,
	skills = {["Nerf health lv2"] = true, ["Nerf combat lv2"] = true},
	vulnerabilities = {["cold damage"] = 1.5, ["fire damage"] = 1.5, ["physical damage"] = 0.6}}

Actorspec{
	name = "plague slime",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = 1,
	animations = {["attack bite"] = "slime attack bite"},
	categories = {["plague"] = true},
	difficulty = 0.7,
	factions = {["evil"] = true},
	feat_anims = {["bite"] = true},
	feat_effects = {["physical damage"] = true},
	inventory_size = 0,
	jump_force = 4,
	model = "slime2",
	speed_walk = 2,
	skills = {["Health lv2"] = true, ["Nerf combat lv1"] = true},
	vulnerabilities = {["cold damage"] = 1.5, ["fire damage"] = 1.5, ["physical damage"] = 0.6}}
