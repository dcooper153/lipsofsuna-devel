Program:add_path(Mod.path)

Animationspec{
	name = "rat attack bite",
	animation = "plaguerat1-bite",
	channel = 2,
	weight = 30}

Animationspec{
	name = "rat dead",
	animation = "plaguerat1-death",
	channel = 1,
	permanent = true,
	repeat_start = 10,
	time = 10}

Animationspec{
	name = "rat death",
	animation = "plaguerat1-death",
	channel = 1,
	permanent = true,
	repeat_start = 10}

Animationspec{
	name = "rat idle",
	animation = "plaguerat1-idle",
	channel = 1,
	permanent = true}

Animationspec{
	name = "rat run",
	animation = "plaguerat1-run",
	channel = 1,
	permanent = true}

Animationspec{
	name = "rat walk",
	animation = "plaguerat1-walk",
	channel = 1,
	permanent = true}

AnimationProfileSpec{
	name = "rat",
	animations = {
		["attack bite"] = "rat attack bite",
		["dead"] = "rat dead",
		["death"] = "rat death",
		["idle"] = "rat idle",
		["run"] = "rat run",
		["run left"] = "rat run",
		["run right"] = "rat run",
		["strafe left"] = "rat walk",
		["strafe right"] = "rat walk",
		["walk"] = "rat walk",
		["walk back"] = "rat walk"}}

Actorspec{
	name = "small rat",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = .1,
	animations = {["default"] = "rat"},
	categories = {["animal"] = true},
	difficulty = 0,
	effect_physical_damage = "fleshimpact1",
	factions = {["neutral"] = true},
	actions = {["right"] = "bite"},
	inventory_size = 3,
	inventory_items = {["hide"] = 1},
	jump_force = 6,
	model = "smallrat1",
	skills = {["Nerf health lv2"] = true, ["Nerf combat lv2"] = true},
	speed_walk = 4}

Actorspec{
	name = "rat",
	base = "small rat",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = 1,
	categories = {["enemy"] = true},
	difficulty = 0,
	effect_physical_damage = "fleshimpact1",
	factions = {["evil"] = true},
	actions = {["right"] = "bite"},
	inventory_size = 3,
	inventory_items = {["hide"] = 2},
	jump_force = 6,
	model = "plaguerat1", -- FIXME
	skills = {["Nerf health lv1"] = true, ["Nerf combat lv2"] = true},
	speed_walk = 4}

Actorspec{
	name = "plague rat",
	base = "small rat",
	ai_enable_strafe = false,
	ai_enable_block = false,
	ai_offense_factor = 1,
	categories = {["plague"] = true},
	difficulty = 0.7,
	effect_physical_damage = "fleshimpact1",
	factions = {["evil"] = true},
	actions = {["right"] = "bite"},
	inventory_size = 3,
	inventory_items = {["hide"] = 2},
	jump_force = 6,
	model = "plaguerat1",
	skills = {["Health lv2"] = true, ["Nerf combat lv1"] = true},
	speed_walk = 4}
