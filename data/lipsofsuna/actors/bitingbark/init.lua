Program:add_path(Mod.path)

Actorspec{
	name = "bitingbark",
	base = "base",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_enable_jump = false,
	ai_enable_weapon_switch = false,
	ai_offense_factor = 1,
	animations = {["attack left claw"] = "bitingbark attack left claw", ["attack right claw"] = "bitingbark attack right claw"},
	categories = {["enemy"] = true},
	difficulty = 0.3,
	effect_physical_damage = "fleshimpact1",
	factions = {["evil"] = true},
	feat_types = {["left claw"] = true, ["right claw"] = true},
	feat_effects = {["physical damage"] = true},
	inventory_size = 3,
	inventory_items = {["log"] = 2},
	jump_force = 0,
	model = "bitingbark1",
	skills = {["Nerf combat lv1"] = true},
	speed_walk = 1}
