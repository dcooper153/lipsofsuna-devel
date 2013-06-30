Program:add_path(Mod.path)

Animationspec{
	name = "stoneimp attack bite",
	animation = "stoneimp1-bite",
	channel = 2,
	weight = 30}

Animationspec{
	name = "stoneimp dead",
	animation = "stoneimp1-death",
	channel = 1,
	permanent = true,
	repeat_start = 10,
	time = 10}

Animationspec{
	name = "stoneimp death",
	animation = "stoneimp1-death",
	channel = 1,
	permanent = true,
	repeat_start = 10}

Animationspec{
	name = "stoneimp idle",
	animation = "stoneimp1-idle",
	channel = 1,
	permanent = true}

Animationspec{
	name = "stoneimp jump",
	animation = "stoneimp1-jump",
	channel = 5,
	fade_out = 0.4,
	permanent = true,
	repeat_start = 10,
	weight = 10}

Animationspec{
	name = "stoneimp walk",
	animation = "stoneimp1-walk",
	channel = 1,
	permanent = true}

Animationspec{
	name = "stoneimp walk back",
	animation = "stoneimp1-walk-back",
	channel = 1,
	permanent = true}

Animationspec{
	name = "stoneimp strafe left",
	animation = "stoneimp1-strafe-left",
	channel = 1,
	permanent = true}

Animationspec{
	name = "stoneimp strafe right",
	animation = "stoneimp1-strafe-right",
	channel = 1,
	permanent = true}

AnimationProfileSpec{
	name = "imp",
	animations = {
		["attack bite"] = "stoneimp attack bite",
		["dead"] = "stoneimp dead",
		["death"] = "stoneimp death",
		["idle"] = "stoneimp idle",
		["jump"] = "stoneimp jump",
		["run"] = "stoneimp walk",
		["run left"] = "stoneimp walk",
		["run right"] = "stoneimp walk",
		["strafe left"] = "stoneimp strafe left",
		["strafe right"] = "stoneimp strafe right",
		["walk"] = "stoneimp walk",
		["walk back"] = "stoneimp walk back"}}

Effectspec{
	name = "impflame1",
	light = true,
	light_diffuse = {1,0.5,0.25,1},
	light_equation = {1,1,1},
	node = "#flame",
	particle = "torchfx1"}

AnimationProfileSpec{
	name = "default stone equipped",
	animations = {
		["hold"] = "default hold bulky"}}

Itemspec{
	name = "impstone",
	categories = {["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default stone equipped"},
	equipment_slot = "hand.R",
	icon = "stone1",
	influences = {["physical damage"] = 1},
	mass = 4,
	model = "firestone1",
	stacking = false,
	water_gravity = Vector(0,6,0)}

Itemspec{
	name = "firestone",
	categories = {["explosive"] = true, ["melee"] = true, ["weapon"] = true},
	animations_equipped = {["default"] = "default stone equipped"},
	destroy_actions = {"explode"},
	destroy_timer = 5,
	equipment_slot = "hand.R",
	health = 1,
	icon = "stone2",
	influences = {["fire damage"] = 10, ["physical damage"] = 10},
	influences_bonus = {"melee", "explosives"},
	mass = 4,
	model = "firestone1",
	special_effects = {"impflame1"},
	stacking = false}

Actorspec{
	name = "stone imp",
	categories = {["enemy"] = true, ["mining"] = true},
	ai_enable_block = false,
	ai_enable_strafe = false,
	ai_enable_weapon_switch = false,
	ai_offense_factor = 0.8,
	animations = {["default"] = "imp"},
	difficulty = 0,
	factions = {["evil"] = true},
	actions = {["right"] = "bite", ["left"] = "tackle"},
	equipment_slots = {["hand.R"] = "#hand.R", ["hand.L"] = "#hand.L"},
	inventory_items = {["impstone"] = 1},
	inventory_size = 2,
	jump_force = 4,
	model = "stoneimp2",
	speed_walk = 1,
	skills = {["Nerf health lv2"] = true, ["Nerf combat lv2"] = true},
	vulnerabilities = {["cold damage"] = 1.1, ["fire damage"] = 1.1, ["physical damage"] = 0.8}}

Actorspec{
	name = "fire imp",
	categories = {["enemy"] = true, ["mining"] = true},
	base = "stone imp",
	ai_enable_block = false,
	ai_enable_strafe = false,
	ai_enable_weapon_switch = false,
	ai_offense_factor = 0.8,
	difficulty = 0.2,
	equipment_slots = {["hand.R"] = "#hand.R", ["hand.L"] = "#hand.L"},
	factions = {["evil"] = true},
	actions = {["right"] = "bite", ["left"] = "explode"},
	jump_force = 4,
	model = "stoneimp2",
	speed_walk = 1,
	inventory_items = {["firestone"] = 1},
	inventory_size = 2,
	skills = {["Nerf health lv1"] = true, ["Nerf combat lv1"] = true},
	vulnerabilities = {["cold damage"] = 2, ["fire damage"] = 0.4, ["physical damage"] = 0.8}}
