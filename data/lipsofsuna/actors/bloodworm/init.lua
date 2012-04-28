Program:add_path(Mod.path)

Actorspec{
	name = "bloodworm",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_enable_jump = false,
	ai_offense_factor = 1,
	animations = {["attack bite"] = "bloodworm attack bite"},
	categories = {["enemy"] = true},
	difficulty = 0,
	effect_physical_damage = "fleshimpact1",
	factions = {["evil"] = true},
	feat_anims = {["bite"] = true},
	feat_effects = {["physical damage"] = true},
	inventory_size = 3,
	inventory_items = {["hide"] = 2},
	jump_force = 3,
	model = "bloodworm2",
	skills = {["Nerf health lv1"] = true, ["Nerf combat lv1"] = true},
	speed_walk = 2}
