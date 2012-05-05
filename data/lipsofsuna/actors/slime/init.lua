Program:add_path(Mod.path)

Animationspec{
	name = "slime jump",
	animation = "bounce",
	channel = 2,
	weight = 30}

Animationspec{
	name = "slime land",
	animation = "bounce",
	channel = 2,
	weight = 30}

Animationspec{
	name = "slime tackle",
	animation = "tackle",
	channel = 2,
	weight = 30}

Actorspec{
	name = "slime",
	base = "base",
	categories = {["enemy"] = true},
	ai_combat_actions = {["bounce"] = true, ["melee"] = true},
	ai_offense_factor = 0.5,
	ai_enable_backstep = false,
	ai_enable_strafe = false,
	ai_enable_walk = false,
	ai_enabled_states = {["bounce"] = true, ["combat"] = true, ["idle"] = true},
	animations = {["jump"] = "slime jump", ["land ground"] = "slime land", ["land water"] = "slime land", ["tackle"] = "slime tackle", ["walk"] = "slime jump"},
	difficulty = 0,
	factions = {["evil"] = true},
	falling_damage_rate = 0,
	feat_types = {["tackle"] = true},
	feat_effects = {["physical damage"] = true},
	inventory_size = 0,
	jump_force = 7,
	model = "slime2",
	speed_walk = 5,
	skills = {["Nerf health lv2"] = true, ["Nerf combat lv2"] = true},
	vulnerabilities = {["cold damage"] = 1.5, ["fire damage"] = 1.5, ["physical damage"] = 0.6}}

Actorspec{
	name = "plague slime",
	base = "base",
	categories = {["plague"] = true},
	ai_combat_actions = {["bounce"] = true, ["melee"] = true},
	ai_offense_factor = 0.5,
	ai_enable_backstep = false,
	ai_enable_strafe = false,
	ai_enable_walk = false,
	ai_enabled_states = {["bounce"] = true, ["combat"] = true, ["idle"] = true},
	animations = {["jump"] = "slime jump", ["land ground"] = "slime land", ["land water"] = "slime land", ["tackle"] = "slime tackle", ["walk"] = "slime jump"},
	difficulty = 0.7,
	factions = {["evil"] = true},
	falling_damage_rate = 0,
	feat_types = {["tackle"] = true},
	feat_effects = {["physical damage"] = true},
	inventory_size = 0,
	jump_force = 7,
	model = "slime2",
	speed_walk = 5,
	skills = {["Health lv2"] = true, ["Nerf combat lv1"] = true},
	vulnerabilities = {["cold damage"] = 1.5, ["fire damage"] = 1.5, ["physical damage"] = 0.6}}
