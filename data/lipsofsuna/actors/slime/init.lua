Program:add_path(Mod.path)

Animationspec{
	name = "slime dead",
	animation = "slime2-death",
	channel = 1,
	permanent = true,
	repeat_start = 10,
	time = 10}

Animationspec{
	name = "slime death",
	animation = "slime2-death",
	channel = 1,
	permanent = true,
	repeat_start = 10}

Animationspec{
	name = "slime idle",
	animation = "slime2-idle",
	channel = 1,
	permanent = true}

Animationspec{
	name = "slime jump",
	animation = "slime2-bounce",
	channel = 2,
	weight = 30}

Animationspec{
	name = "slime land",
	animation = "slime2-bounce",
	channel = 2,
	weight = 30}

Animationspec{
	name = "slime tackle",
	animation = "slime2-tackle",
	channel = 2,
	weight = 30}

AnimationProfileSpec{
	name = "slime",
	animations = {
		["dead"] = "slime dead",
		["death"] = "slime death",
		["idle"] = "slime idle",
		["jump"] = "slime jump",
		["land ground"] = "slime land",
		["land water"] = "slime land",
		["tackle"] = "slime tackle",
		["walk"] = "slime jump"},
	inherit = {"default"}}

Actorspec{
	name = "slime",
	categories = {["enemy"] = true},
	ai_combat_actions = {["bounce"] = true, ["melee"] = true},
	ai_offense_factor = 0.5,
	ai_enable_backstep = false,
	ai_enable_strafe = false,
	ai_enable_walk = false,
	ai_enabled_states = {["bounce"] = true, ["combat"] = true, ["idle"] = true},
	animations = {["default"] = "slime"},
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
	categories = {["plague"] = true},
	ai_combat_actions = {["bounce"] = true, ["melee"] = true},
	ai_offense_factor = 0.5,
	ai_enable_backstep = false,
	ai_enable_strafe = false,
	ai_enable_walk = false,
	ai_enabled_states = {["bounce"] = true, ["combat"] = true, ["idle"] = true},
	animations = {["default"] = "slime"},
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
