Program:add_path(Mod.path)

Species{
	name = "bitingbark",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_enable_jump = false,
	ai_enable_weapon_switch = false,
	ai_offense_factor = 1,
	animations = {
		["attack left claw"] = {animation = "claw-left", channel = 2, weight = 30},
		["attack right claw"] = {animation = "claw-right", channel = 2, weight = 30}},
	categories = {"enemy"},
	difficulty = 0,
	effect_physical_damage = "fleshimpact1",
	factions = {"evil"},
	feat_anims = {"left claw", "right claw"},
	feat_effects = {"strength to physical damage"},
	inventory_size = 3,
	inventory_items = {["log"] = 2},
	jump_force = 0,
	model = "bitingbark1",
	speed_walk = 1,
	skills = {
		{name = "health", max = 30, val = 30},
		{name = "strength", max = 5, val = 5}}}
