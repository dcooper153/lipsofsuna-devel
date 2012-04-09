Program:add_path(Mod.path)

Species{
	name = "slime",
	base = "base",
	ai_enable_block = false,
	ai_offense_factor = 0.5,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"random"},
	difficulty = 0,
	factions = {"evil"},
	feat_anims = {"bite"},
	feat_effects = {"physical damage"},
	inventory_size = 0,
	jump_force = 4,
	model = "slime2",
	speed_walk = 2,
	skills = {"Nerf health lv2", "Nerf combat lv2"},
	vulnerabilities = {["cold damage"] = 1.5, ["fire damage"] = 1.5, ["physical damage"] = 0.6}}

Species{
	name = "plague slime",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = 1,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"plague"},
	difficulty = 0.7,
	factions = {"evil"},
	feat_anims = {"bite"},
	feat_effects = {"physical damage"},
	inventory_size = 0,
	jump_force = 4,
	model = "slime2",
	speed_walk = 2,
	skills = {"Health lv2", "Nerf combat lv1"},
	vulnerabilities = {["cold damage"] = 1.5, ["fire damage"] = 1.5, ["physical damage"] = 0.6}}
