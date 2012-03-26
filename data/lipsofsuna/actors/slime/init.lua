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
	feat_effects = {"strength to physical damage"},
	inventory_size = 0,
	jump_force = 4,
	model = "slime2",
	speed_walk = 2,
	vulnerabilities = {cold = 1.5, fire = 1.5, physical = 0.6}}

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
	feat_effects = {"strength to physical damage"},
	inventory_size = 0,
	jump_force = 4,
	model = "slime2",
	speed_walk = 2,
	vulnerabilities = {cold = 1.5, fire = 1.5, physical = 0.6}}
