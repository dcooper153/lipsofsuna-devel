Program:add_path(Mod.path)

Species{
	name = "bloodworm",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_enable_jump = false,
	ai_offense_factor = 1,
	animations = {
		["attack bite"] = {animation = "bite", channel = 2, weight = 30}},
	categories = {"enemy"},
	difficulty = 0,
	effect_physical_damage = "fleshimpact1",
	factions = {"evil"},
	feat_anims = {"bite"},
	feat_effects = {"strength to physical damage"},
	inventory_size = 3,
	inventory_items = {["hide"] = 2},
	jump_force = 3,
	model = "bloodworm2",
	speed_walk = 2,
	skills = {
		{name = "health", max = 30, val = 30},
		{name = "strength", max = 5, val = 5}}}
