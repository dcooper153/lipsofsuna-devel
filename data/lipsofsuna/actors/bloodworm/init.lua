Program:add_path(Mod.path)

Animationspec{
	name = "bloodworm attack bite",
	animation = "bloodworm2-bite",
	channel = 2,
	weight = 30}

Animationspec{
	name = "bloodworm dead",
	animation = "bloodworm2-death",
	channel = 1,
	permanent = true,
	repeat_start = 10,
	time = 10}

Animationspec{
	name = "bloodworm death",
	animation = "bloodworm2-death",
	channel = 1,
	permanent = true,
	repeat_start = 10}

Animationspec{
	name = "bloodworm idle",
	animation = "bloodworm2-idle",
	channel = 1,
	permanent = true}

Animationspec{
	name = "bloodworm walk",
	animation = "bloodworm2-walk",
	channel = 1,
	permanent = true}

AnimationProfileSpec{
	name = "bloodworm",
	animations = {
		["attack bite"] = "bloodworm attack bite",
		["dead"] = "bloodworm dead",
		["death"] = "bloodworm death",
		["idle"] = "bloodworm idle",
		["run"] = "bloodworm walk",
		["run left"] = "bloodworm walk",
		["run right"] = "bloodworm walk",
		["strafe left"] = "bloodworm walk",
		["strafe right"] = "bloodworm walk",
		["walk"] = "bloodworm walk",
		["walk back"] = "bloodworm walk"}}

Actorspec{
	name = "bloodworm",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_enable_jump = false,
	ai_offense_factor = 1,
	animations = {["default"] = "bloodworm"},
	categories = {["enemy"] = true},
	difficulty = 0,
	effect_physical_damage = "fleshimpact1",
	factions = {["evil"] = true},
	actions = {["right"] = "bite"},
	feat_effects = {["physical damage"] = true},
	inventory_size = 3,
	inventory_items = {["hide"] = 2},
	jump_force = 3,
	model = "bloodworm2",
	skills = {["Nerf health lv1"] = true, ["Nerf combat lv1"] = true},
	speed_walk = 2}
